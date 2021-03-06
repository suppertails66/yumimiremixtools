#include "ymm/YmmScriptReader.h"
#include "util/TBufStream.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include "exception/TException.h"
#include <cctype>
#include <algorithm>
#include <string>
#include <iostream>


// for testing: if set, unrecognized symbols will be ignored
#define YMMREADER_IGNORE_BAD_INPUT 0


using namespace BlackT;

namespace Md {


//const static int scriptBufferCapacity = 0x10000;
const static int literalSize = 5;

const static int code_end     = 0x00;
const static int scene_code_br     = 0x02;
const static int scene_code_end     = 0x07;

const static double framesPerSecond = 60;

// in italic mode, characters with ID >= this are modified
const static int italicModeMinId = 0x01;
// characters with ID < this are modified
const static int italicModeMaxId = 0x48;
// amount added to italicizable characters in italic mode
const static int italicModeOffset = 0x50;

// when applying auto-off, delay this many additional seconds
// past the end of the sound before turning off the subtitles
// (sound files are usually clipped as close to the end of the
// content as possible, and we don't want to have the subtitles cut off
// too abruptly at the end)
//const static double autoOffOverhang = 0.320;
const static double autoOffOverhang = 0.200;
// don't allow an auto-off command to generate a time shorter than this
// (every subtitle needs to be on screen long enough to be readable,
// even if it means it's significantly longer than the corresponding audio)
const static double minOnTime = 1.000;
// the end times of point pairs generated by the points command
// is reduced by this amount (in frames) to account for the manner
// in which the karaoke gradient is generated:
// the end time represents the time at which the ride side of the
// gradient will reach the target point, whereas what we generally
// want is for the gradient to completely disappear at that point
const static int pointEndLeftFrameOffset = 0;

YmmScriptReader::ResultString::ResultString()
  : initialized(false) { }

YmmScriptReader::ResultRegion::ResultRegion()
  : soundCount(0),
    choiceCount(0) { }

YmmScriptReader::YmmScriptReader(
                  BlackT::TStream& src__,
                  NameToRegionMap& dst__,
                  const BlackT::TThingyTable& thingy__)
  : src(src__),
    dst(dst__),
    thingy(thingy__),
    lineNum(0),
    breakTriggered(false),
    regionName("0"),
    sceneModeOn(true),
    stringIsActive(false),
    italicModeOn(false),
    lastWaitTime(0.0),
    timeScale(1.0)
/*    currentScriptSrcOffset(-1),
    currentScriptSrcSize(-1),
    currentScriptWidth(0),
    currentScriptIsLiteral(false),
    currentScriptIsNotCompressible(false),
    currentMapMainNum(-1),
    currentMapSubNum(-1),
    currentScriptBuffer(),
    sceneModeOn(false)*/ {
  loadThingy(thingy__);
//  spaceOfs.open((outprefix + "msg_space.txt").c_str());
//  indexOfs.open((outprefix + "msg_index.txt").c_str());
  resetScriptBuffer();
}

bool YmmScriptReader::operator()() {
  try {
    while (!src.eof()) {
      std::string line;
      src.getLine(line);
      ++lineNum;
      
  //    std::cerr << lineNum << std::endl;
      if (line.size() <= 0) continue;
      
      // discard lines containing only ASCII spaces and tabs
  //    bool onlySpace = true;
  //    for (int i = 0; i < line.size(); i++) {
  //      if ((line[i] != ' ')
  //          && (line[i] != '\t')) {
  //        onlySpace = false;
  //        break;
  //      }
  //    }
  //    if (onlySpace) continue;
      
      TBufStream ifs(line.size());
      ifs.write(line.c_str(), line.size());
      ifs.seek(0);
      
      // check for special stuff
      if (ifs.peek() == '#') {
        // directives
        ifs.get();
        processDirective(ifs);
        
        if (breakTriggered) {
          breakTriggered = false;
          return false;
        }
        
        continue;
      }
      
      while (!ifs.eof()) {
        // check for comments
        if ((ifs.remaining() >= 2)
            && (ifs.peek() == '/')) {
          ifs.get();
          if (ifs.peek() == '/') break;
          else ifs.unget();
        }
        
        outputNextSymbol(ifs);
      }
    }
    
    if (currentScriptBuffer.size() > 0) {
      flushActiveScript();
    }
    
    return true;
  }
  catch (TGenericException& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.problem() << std::endl;
    throw e;
  }
  catch (TException& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.what() << std::endl;
    throw e;
  }
  catch (std::exception& e) {
    std::cerr << "Exception on script line " << std::dec << lineNum << ": " << std::endl;
    std::cerr << e.what() << std::endl;
    throw e;
  }
}
  
void YmmScriptReader::loadThingy(const BlackT::TThingyTable& thingy__) {
  thingy = thingy__;
}
  
void YmmScriptReader::outputNextSymbol(TStream& ifs) {
//  if (currentScriptIsLiteral) {
//    currentScriptBuffer.put(ifs.get());
//    return;
//  }

  // flag that we have put content into a string so we know to terminate it later
  stringIsActive = true;

  // literal value
  if ((ifs.remaining() >= literalSize)
      && (ifs.peek() == literalOpenSymbol)) {
    int pos = ifs.tell();
    
    ifs.get();
    if (ifs.peek() == '$') {
      ifs.get();
      std::string valuestr = "0x";
//      valuestr += ifs.get();
//      valuestr += ifs.get();
      valuestr += ifs.get();
      valuestr += ifs.get();
      
      if (ifs.peek() == literalCloseSymbol) {
        ifs.get();
        int value = TStringConversion::stringToInt(valuestr);
        
//        currentScriptBuffer.writeu16be(value);
        currentScriptBuffer.writeu8(value);

        return;
      }
    }
    
    // not a literal value
    ifs.seek(pos);
  }
  
  // HACK: no matter what, a "*" in the input toggles italic mode
  // ...actually, after jumping through all those hoops to make sure
  // there was enough room in the font for an italic version,
  // even rearranging the order of the symbols to optimize for it,
  // i don't really like the way the italics look,
  // so i'm just dropping them.
  // what a shame.
  if (ifs.peek() == '*') {
    ifs.get();
//    italicModeOn = !italicModeOn;
    return;
  }
  
  TThingyTable::MatchResult result;
  result = thingy.matchTableEntry(ifs);
  
  if (result.id != -1) {
//    std::cerr << std::dec << lineNum << " " << std::hex << result.id << " " << result.size << std::endl;

    // HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK
    // TThingyTable does not support mapping multiple characters to the
    // same ID, so i've done a quick workaround
//    if ((result.id >= 0x100)) {
//      result.id >>= 8;
//    }  
    
    // if italics on and character in italicizable range,
    // add offset to get index of italic version of character
    if (italicModeOn) {
      int code = result.id;
      if ((code >= italicModeMinId) && (code < italicModeMaxId)) {
        result.id += italicModeOffset;
      }
    }
    
    int symbolSize;
    if (result.id <= 0xFF) symbolSize = 1;
    else if (result.id <= 0xFFFF) symbolSize = 2;
    else if (result.id <= 0xFFFFFF) symbolSize = 3;
    else symbolSize = 4;
    
    currentScriptBuffer.writeInt(result.id, symbolSize,
      EndiannessTypes::big, SignednessTypes::nosign);
    
    return;
  }

  #if YMMREADER_IGNORE_BAD_INPUT
    // ignore unrecognized characters
    // (and handle sjis sequences)
//    if (((ifs.readu8() & 0x80) != 0) && !ifs.eof()) ifs.get();
    ifs.get();
    return;
  #endif
  
  std::string remainder;
  ifs.getLine(remainder);
  
  // if we reached end of file, this is not an error: we're done
//  if (ifs.eof()) return;
  
  throw TGenericException(T_SRCANDLINE,
                          "YmmScriptReader::outputNextSymbol()",
                          "Line "
                            + TStringConversion::intToString(lineNum)
                            + ":\n  Couldn't match symbol at: '"
                            + remainder
                            + "'");
}
  
void YmmScriptReader::flushActiveScript() {
//  if (currentScriptBuffer.size() <= 0) return;
  // empty strings are valid, so check for valid ID instead
  if (!currentResult.initialized) return;
  
/*  // append terminator as needed
  if (sceneModeOn) {
    // scene strings are not zero-terminated
    currentScriptBuffer.put(scene_code_end);
  }
  else {
    currentScriptBuffer.put(code_end);
  } */
  
  terminateCurrentStringIfActive();
  
  // if handling a sound in scene mode and auto-off is not disabled,
  // generate appropriate wait and off commands
  // (if content exists)
  if (sceneModeOn
      && (currentResult.hasProperty("soundLen"))
      && (!currentResult.propertyIsTrue("noAutoOff"))
      && (currentScriptBuffer.size() > 0)) {
//    std::cerr << currentResult.properties.size() << std::endl;
    
    double soundEndTime = TStringConversion::fromString<double>(
      currentResult.properties.at("soundLen"));
    
    if (lastWaitTime < soundEndTime) {
      // duration between last wait and auto-off must exceed minimum
      double autoOffTime = soundEndTime + autoOffOverhang;
      if ((autoOffTime - lastWaitTime) < minOnTime) {
        autoOffTime = lastWaitTime + minOnTime;
      }
      
      outputWaitCommand(autoOffTime);
      outputOffCommand();
    }
  }
  
  // add stream terminator if in scene mode
  if (sceneModeOn) {
    outputTerminateCommand();
  }

  int outputSize = currentScriptBuffer.size();
  
  ResultString result = currentResult;
  currentScriptBuffer.seek(0);
  while (!currentScriptBuffer.eof()) {
    result.str += currentScriptBuffer.get();
  }
  
/*  result.srcOffset = currentScriptSrcOffset;
  result.srcSize = currentScriptSrcSize;
//  result.srcOpOffset = currentScriptSrcOpOffset;
//  result.index = currentScriptIndex;
//  result.name = currentScriptName;
  result.width = currentScriptWidth;
//  result.indices = currentScriptIndices;
  result.isLiteral = currentScriptIsLiteral;
  result.isNotCompressible = currentScriptIsNotCompressible;
  result.mapMainNum = currentMapMainNum;
  result.mapSubNum = currentMapSubNum; */
  
  dst[regionName].strings.push_back(result);
  
  // clear script buffer
  resetScriptBuffer();
}

void YmmScriptReader::resetScriptBuffer() {
  currentScriptBuffer = TBufStream();
/*  currentScriptSrcOffset = -1;
  currentScriptSrcSize = -1;
  currentScriptIsLiteral = false;*/
//  currentScriptIndices.clear();
  
  lastWaitTime = 0.0;
  timeScale = 1.0;
  
  currentResult = ResultString();
}
  
/*bool YmmScriptReader::checkSymbol(BlackT::TStream& ifs, std::string& symbol) {
  if (symbol.size() > ifs.remaining()) return false;
  
  int startpos = ifs.tell();
  for (int i = 0; i < symbol.size(); i++) {
    if (symbol[i] != ifs.get()) {
      ifs.seek(startpos);
      return false;
    }
  }
  
  return true;
} */

void YmmScriptReader::terminateCurrentStringIfActive() {
  if (stringIsActive) {
    outputTerminateCommand();
    stringIsActive = false;
  }
}

void YmmScriptReader::outputTerminateCommand() {
  int opcode = thingy.matchTableEntry("[null]").id;
  currentScriptBuffer.writeu8(opcode);
}

void YmmScriptReader::outputOffCommand() {
  terminateCurrentStringIfActive();
  
  int opcode = thingy.matchTableEntry("[off]").id;
  currentScriptBuffer.writeu8(opcode);
}

void YmmScriptReader::outputWaitCommand(double secTime) {
  terminateCurrentStringIfActive();
  
  // convert argument from seconds to frames
  int frameValue = secTime * framesPerSecond;
  
  // output wait command
  int opcode = thingy.matchTableEntry("[wait]").id;
  currentScriptBuffer.writeu8(opcode);
  currentScriptBuffer.writeu16be(frameValue * timeScale);
  
  lastWaitTime = secTime;
}

void YmmScriptReader::outputSlotCommand(int slotNum) {
  terminateCurrentStringIfActive();
  
  int opcode = thingy.matchTableEntry("[slot]").id;
  currentScriptBuffer.writeu8(opcode);
  currentScriptBuffer.writeu8(slotNum);
}

void YmmScriptReader::outputPalCommand(int palNum) {
  terminateCurrentStringIfActive();
  
  int opcode = thingy.matchTableEntry("[pal]").id;
  currentScriptBuffer.writeu8(opcode);
  currentScriptBuffer.writeu8(palNum);
}

void YmmScriptReader
    ::outputPointsCommand(double point1time, double point2time) {
  terminateCurrentStringIfActive();
  
  // fudge the numbers a bit:
  // we would like to pretend the span is a bit shorter
  // than actually specified, because the endpoint actually
  // indicates the time at which the gradient will *begin*
  // flowing out of the the segment rather than when
  // it will *end*.
  // the "correct" way to do this would be to compute the
  // width of the corresponding segment in pixels
  // and subtract the width of the gradient from that...
  // and wait, can't i just do that in the subtitle code itself?
  
  // convert argument from seconds to frames
  int frameValue1 = point1time * framesPerSecond;
  int frameValue2 = point2time * framesPerSecond;
  
  // apply frame offset for endpoint
  frameValue2 -= pointEndLeftFrameOffset;
  
  int opcode = thingy.matchTableEntry("[points]").id;
  currentScriptBuffer.writeu8(opcode);
  currentScriptBuffer.writeu16be(frameValue1 * timeScale);
  currentScriptBuffer.writeu16be(frameValue2 * timeScale);
}

void YmmScriptReader
    ::outputAlignModeCommand(int alignMode) {
  terminateCurrentStringIfActive();
  
  int opcode = thingy.matchTableEntry("[align]").id;
  currentScriptBuffer.writeu8(opcode);
  currentScriptBuffer.writeu8(alignMode);
}

void YmmScriptReader
    ::outputSyncCommand(int modeId, int targetValue, double newTime) {
  terminateCurrentStringIfActive();
  
  int frameTime = newTime * framesPerSecond;
  // update last seen time to new time
  lastWaitTime = newTime;
  
  int opcode = thingy.matchTableEntry("[sync]").id;
  currentScriptBuffer.writeu8(opcode);
  currentScriptBuffer.writeu8(modeId);
  currentScriptBuffer.writeu16be(targetValue);
  currentScriptBuffer.writeu16be(frameTime * timeScale);
}

void YmmScriptReader
    ::outputStartStreamCommand(int streamIndex, int scriptId) {
  terminateCurrentStringIfActive();
  
  int opcode = thingy.matchTableEntry("[startstream]").id;
  currentScriptBuffer.writeu8(opcode);
  currentScriptBuffer.writeu8(streamIndex);
  currentScriptBuffer.writeu8(scriptId);
}

void YmmScriptReader::processDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("LOADTABLE") == 0) {
    processLoadTable(ifs);
  }
  else if (name.compare("STARTSTRING") == 0) {
/*    currentResult.id = TParse::matchString(ifs);
    TParse::matchChar(ifs, ',');
    currentScriptSrcOffset = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentScriptSrcSize = TParse::matchInt(ifs);*/
    currentResult.id = TParse::matchString(ifs);
    currentResult.initialized = true;
//    processStartMsg(ifs);
  }
  else if (name.compare("ENDSTRING") == 0) {
    processEndMsg(ifs);
  }
  else if (name.compare("INCBIN") == 0) {
    processIncBin(ifs);
  }
  else if (name.compare("BREAK") == 0) {
    processBreak(ifs);
  }
  else if (name.compare("STARTREGION") == 0) {
    processStartRegion(ifs);
  }
  else if (name.compare("ENDREGION") == 0) {
    processEndRegion(ifs);
  }
/*  else if (name.compare("SETLASTXPOS") == 0) {
    currentScriptWidth = TParse::matchInt(ifs);
  }
  else if (name.compare("SETMAP") == 0) {
    currentMapMainNum = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentMapSubNum = TParse::matchInt(ifs);
  }
  else if (name.compare("SETSCRIPTREF") == 0) {
    currentResult.scriptRefStart = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentResult.scriptRefEnd = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    currentResult.scriptRefCode = TParse::matchInt(ifs);
  }
  else if (name.compare("ADDPOINTERREF") == 0) {
    currentResult.pointerRefs.push_back(TParse::matchInt(ifs));
  }
  else if (name.compare("ADDFREESPACE") == 0) {
    ResultString::FreeSpace space;
    
    space.pos = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
    space.size = TParse::matchInt(ifs);
    
    currentResult.freeSpaces.push_back(space);
  }
  else if (name.compare("ADDOVERWRITE") == 0) {
    currentResult.overwriteAddresses.push_back(TParse::matchInt(ifs));
  }
  else if (name.compare("ADDEXTRAID") == 0) {
    currentResult.extraIds.push_back(TParse::matchInt(ifs));
  }
  else if (name.compare("SETNOTCOMPRESSIBLE") == 0) {
    currentScriptIsNotCompressible = (TParse::matchInt(ifs) != 0);
  }
  else if (name.compare("SETSCENEMODE") == 0) {
    sceneModeOn = (TParse::matchInt(ifs) != 0);
  }
//  else if (name.compare("SETINDEXLIST") == 0) {
//    processSetIndexList(ifs);
//  }*/
  else if (name.compare("SETSCENEMODEON") == 0) {
    sceneModeOn = (TParse::matchInt(ifs) != 0);
  }
  else if (name.compare("SETFILE") == 0) {
    dst[regionName].filename = TParse::matchString(ifs);
  }
  else if (name.compare("SETSOUNDCOUNT") == 0) {
    dst[regionName].soundCount = TParse::matchInt(ifs);
  }
  else if (name.compare("SETCHOICECOUNT") == 0) {
    dst[regionName].choiceCount = TParse::matchInt(ifs);
  }
  else if (name.compare("P") == 0) {
    std::string name = TParse::matchString(ifs);
    TParse::matchChar(ifs, ',');
    std::string value = TParse::matchString(ifs);
    currentResult.properties[name] = value;
  }
  else if (name.compare("W") == 0) {
    outputWaitCommand(matchTime(ifs));
  }
  else if (name.compare("WOFF") == 0) {
    outputWaitCommand(matchTime(ifs));
    outputOffCommand();
  }
  else if (name.compare("OFF") == 0) {
    outputOffCommand();
  }
  else if (name.compare("SLOT") == 0) {
    std::string slotName = TParse::matchName(ifs);
    if (slotName.compare("bottom") == 0) {
      outputSlotCommand(0);
    }
    else if (slotName.compare("top") == 0) {
      outputSlotCommand(1);
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "YmmScriptReader::processDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ":\n  Unknown slot name: "
                                + slotName);
    }
  }
  else if (name.compare("PAL") == 0) {
    std::string slotName = TParse::matchName(ifs);
    if (slotName.compare("std") == 0) {
      outputPalCommand(0);
    }
    else if (slotName.compare("karaoke") == 0) {
      outputPalCommand(1);
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "YmmScriptReader::processDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ":\n  Unknown palette name: "
                                + slotName);
    }
  }
  else if (name.compare("KSEG") == 0) {
//    double point1 = TParse::matchDouble(ifs);
    double point1 = matchTime(ifs);
    TParse::matchChar(ifs, ',');
//    double point2 = TParse::matchDouble(ifs);
    double point2 = matchTime(ifs);
    
    outputPointsCommand(point1, point2);
  }
  else if (name.compare("ALIGN") == 0) {
    std::string alignModeName = TParse::matchName(ifs);
    if (alignModeName.compare("center") == 0) {
      outputAlignModeCommand(0);
    }
    else if (alignModeName.compare("left") == 0) {
      outputAlignModeCommand(1);
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "YmmScriptReader::processDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ":\n  Unknown alignment mode name: "
                                + alignModeName);
    }
  }
  else if (name.compare("SYNC") == 0) {
    std::string syncModeName = TParse::matchName(ifs);
    TParse::matchChar(ifs, ',');
    int targetValue = TParse::matchInt(ifs);
    TParse::matchChar(ifs, ',');
//    double newTime = TParse::matchDouble(ifs);
    double newTime = matchTime(ifs);
    
    if (syncModeName.compare("sound") == 0) {
      outputSyncCommand(0, targetValue, newTime);
    }
    else if (syncModeName.compare("cdplay") == 0) {
      outputSyncCommand(1, targetValue, newTime);
    }
    else if (syncModeName.compare("cdwait") == 0) {
      outputSyncCommand(2, targetValue, newTime);
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "YmmScriptReader::processDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ":\n  Unknown sync mode name: "
                                + syncModeName);
    }
  }
  else if (name.compare("STARTSCRIPT") == 0) {
    std::string streamModeName = TParse::matchName(ifs);
    TParse::matchChar(ifs, ',');
    int scriptId = TParse::matchInt(ifs);
    
    // we shouldn't actually be starting anything in
    // the sound stream with this command, but...
    if (streamModeName.compare("sound") == 0) {
      outputStartStreamCommand(0, scriptId);
    }
    else if (streamModeName.compare("special") == 0) {
      outputStartStreamCommand(1, scriptId);
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "YmmScriptReader::processDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ":\n  Unknown stream name: "
                                + streamModeName);
    }
  }
  else if (name.compare("SETTIMESCALE") == 0) {
    timeScale = TParse::matchDouble(ifs);
  }
  else {
    throw TGenericException(T_SRCANDLINE,
                            "YmmScriptReader::processDirective()",
                            "Line "
                              + TStringConversion::intToString(lineNum)
                              + ":\n  Unknown directive: "
                              + name);
  }
  
  TParse::matchChar(ifs, ')');
}

void YmmScriptReader::processLoadTable(BlackT::TStream& ifs) {
  std::string tableName = TParse::matchString(ifs);
  TThingyTable table(tableName);
  loadThingy(table);
}

void YmmScriptReader::processStartMsg(BlackT::TStream& ifs) {
/*  currentScriptSrcOffset = TParse::matchInt(ifs);
  TParse::matchChar(ifs, ',');
  currentScriptSrcPointer = TParse::matchInt(ifs);
  TParse::matchChar(ifs, ',');
  currentScriptSrcSize = TParse::matchInt(ifs);
  TParse::matchChar(ifs, ',');
  currentScriptIsLiteral = (TParse::matchInt(ifs) != 0); */
}

void YmmScriptReader::processEndMsg(BlackT::TStream& ifs) {
  flushActiveScript();
}

void YmmScriptReader::processIncBin(BlackT::TStream& ifs) {
  std::string filename = TParse::matchString(ifs);
  TBufStream src(1);
  src.open(filename.c_str());
  currentScriptBuffer.writeFrom(src, src.size());
}

void YmmScriptReader::processBreak(BlackT::TStream& ifs) {
  breakTriggered = true;
}

void YmmScriptReader::processStartRegion(BlackT::TStream& ifs) {
  flushActiveScript();
  if (TParse::checkInt(ifs)) {
    regionName = TStringConversion::toString(TParse::matchInt(ifs));
  }
  else {
    regionName = TParse::matchString(ifs);
  }
//  TParse::matchChar(ifs, ',');
//  int pointerTableBase = TParse::matchInt(ifs);
}

void YmmScriptReader::processEndRegion(BlackT::TStream& ifs) {
//  TParse::matchInt(ifs);
//  TParse::matchInt(ifs);
////  flushActiveScript();
//  breakTriggered = true;
}

/*void YmmScriptReader::processSetIndexList(BlackT::TStream& ifs) {
  currentScriptIndices.clear();
  while (TParse::checkInt(ifs)) {
    int next = TParse::matchInt(ifs);
    currentScriptIndices.push_back(next);
    if (!TParse::checkChar(ifs, ',')) break;
    TParse::matchChar(ifs, ',');
  }
} */

double YmmScriptReader::matchTime(BlackT::TStream& ifs) {
  double value = TParse::matchDouble(ifs);
  if (TParse::checkChar(ifs, ':')) {
    TParse::matchChar(ifs, ':');
    double seconds = TParse::matchDouble(ifs);
    value = (value * 60) + seconds;
  }
  return value;
}


}
