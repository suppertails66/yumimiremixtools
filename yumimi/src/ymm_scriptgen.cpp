#include "ymm/YmmScene.h"
#include "ymm/YmmTranslationSheet.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "util/TSoundFile.h"
#include "util/TPngConversion.h"
#include <string>
#include <map>
#include <iostream>

#define EXPORT_SOUNDS 0
#define EXPORT_CHOICE_SCENES 0
#define EXPORT_CHOICE_IMAGES 0
#define GENERATE_SOUND_TEMPLATE 1
#define GENERATE_CHOICE_TEMPLATE 0

using namespace std;
using namespace BlackT;
using namespace Md;

const static int choiceTilemapBaseX = 0;
const static int choiceTilemapBaseY = 256;
const static int choiceTilemapOptionPixelH = 16;

struct FreeSpaceSpec {
  int pos;
  int size;
};

/*std::string toUtf8(std::string str) {
  // convert from SJIS to UTF8
  
  TBufStream conv;
  conv.writeString(str);
  conv.seek(0);
  
  std::string newStr;
  while (!conv.eof()) {
    if (conv.peek() == '\x0A') {
      newStr += conv.get();
    }
    else {
      TThingyTable::MatchResult result = tableSjisUtf8.matchId(conv);
      
      if (result.id == -1) {
        throw TGenericException(T_SRCANDLINE,
                                "toUtf8()",
                                "bad input string");
      }
      
      newStr += tableSjisUtf8.getEntry(result.id);
    }
  }
  
  return newStr;
}*/

class YmmGenericString {
public:
  YmmGenericString()
      // needs to not be initialized to -1
      // see YmmScriptReader::flushActiveScript()
    : offset(0),
      size(0),
      mayNotExist(false) { }
  
  enum Type {
    type_none,
    type_string,
    type_mapString,
    type_setRegion,
    type_setMap,
    type_setNotCompressible,
    type_addOverwrite,
    type_addFreeSpace,
    type_genericLine,
    type_comment,
    type_marker
  };
  
  Type type;
  
  std::string content;
  std::string prefixBase;
  std::string suffixBase;
  int offset;
  int size;
  bool mayNotExist;
  
  std::string idOverride;
  
  int scriptRefStart;
  int scriptRefEnd;
  int scriptRefCode;
  
  int regionId;
  
  int mapMainId;
  int mapSubId;
  
  bool notCompressible;
  
  std::vector<int> pointerRefs;
//  int pointerBaseAddr;

  // fuck this
  std::vector<FreeSpaceSpec> freeSpaces;

  std::string translationPlaceholder;
  
  std::vector<int> overwriteAddresses;
  std::vector<int> extraIds;
  std::vector<std::string> genericLines;
  
  void addPropSet(std::string name, std::string value) {
    if (prefixBase.size() > 0) prefixBase += "\r\n";
    prefixBase = prefixBase + "#P("
      + "\"\"" + name + "\"\", "
      + "\"\"" + value + "\"\")";
  }
  
protected:
  
};

typedef std::vector<YmmGenericString> YmmGenericStringCollection;

class YmmGenericStringSet {
public:
    
  YmmGenericStringCollection strings;
  
  static YmmGenericString readString(TStream& src, const TThingyTable& table,
                              int offset) {
    YmmGenericString result;
    result.type = YmmGenericString::type_string;
    result.offset = offset;
    
    src.seek(offset);
    while (!src.eof()) {
      if (src.peek() == 0x00) {
        src.get();
        result.size = src.tell() - offset;
        return result;
      }
      
      TThingyTable::MatchResult matchCheck
        = table.matchId(src);
      if (matchCheck.id == -1) break;
      
      std::string newStr = table.getEntry(matchCheck.id);
      result.content += newStr;
      
      // HACK
      if (newStr.compare("\\n") == 0) result.content += "\n";
    }
    
    throw TGenericException(T_SRCANDLINE,
                            "YmmGenericStringSet::readString()",
                            std::string("bad string at ")
                            + TStringConversion::intToString(offset));
  }
  
  void addString(TStream& src, const TThingyTable& table,
                 int offset) {
    YmmGenericString result = readString(src, table, offset);
    strings.push_back(result);
  }
  
  void addRawString(std::string content, int offset, int size) {
    YmmGenericString result;
    result.type = YmmGenericString::type_string;
    result.content = content;
    result.offset = offset;
    result.size = size;
    strings.push_back(result);
  }
  
  void addOverwriteString(TStream& src, const TThingyTable& table,
                 int offset) {
    YmmGenericString result = readString(src, table, offset);
    result.overwriteAddresses.push_back(offset);
    strings.push_back(result);
  }
  
  void addMarker(std::string content) {
    YmmGenericString result;
    result.type = YmmGenericString::type_marker;
    result.content = content;
    strings.push_back(result);
  }
  
  void addPointerTableString(TStream& src, const TThingyTable& table,
                             int offset, int pointerOffset) {
    // check if string already exists, and add pointer ref if so
    for (unsigned int i = 0; i < strings.size(); i++) {
      YmmGenericString& checkStr = strings[i];
      // mapStrings need not apply
      if (checkStr.type == YmmGenericString::type_string) {
        if (checkStr.offset == offset) {
          checkStr.pointerRefs.push_back(pointerOffset);
          return;
        }
      }
    }
    
    // new string needed
    YmmGenericString result = readString(src, table, offset);
    result.pointerRefs.push_back(pointerOffset);
    strings.push_back(result);
  }
  
  void addComment(std::string comment) {
    YmmGenericString result;
    result.type = YmmGenericString::type_comment;
    result.content = comment;
    strings.push_back(result);
  }
  
  void addSetNotCompressible(bool notCompressible) {
    YmmGenericString result;
    result.type = YmmGenericString::type_setNotCompressible;
    result.notCompressible = notCompressible;
    strings.push_back(result);
  }
  
  void addAddOverwrite(int offset) {
    YmmGenericString result;
    result.type = YmmGenericString::type_addOverwrite;
    result.offset = offset;
    strings.push_back(result);
  }
  
  void addAddFreeSpace(int offset, int size) {
    YmmGenericString result;
    result.type = YmmGenericString::type_addFreeSpace;
    result.offset = offset;
    result.size = size;
    strings.push_back(result);
  }
  
  void addSetRegion(int regionId) {
    YmmGenericString str;
    str.type = YmmGenericString::type_setRegion;
    str.regionId = regionId;
    strings.push_back(str);
  }
  
  void addGenericLine(std::string content) {
    YmmGenericString str;
    str.type = YmmGenericString::type_genericLine;
    str.content = content;
    strings.push_back(str);
  }
  
  void exportToSheet(
      YmmTranslationSheet& dst,
      std::ostream& ofs,
      std::string idPrefix) const {
    int strNum = 0;
    for (unsigned int i = 0; i < strings.size(); i++) {
      const YmmGenericString& item = strings[i];
      
      if ((item.type == YmmGenericString::type_string)
          || (item.type == YmmGenericString::type_mapString)) {
        std::string idString = idPrefix
//          + TStringConversion::intToString(strNum)
//          + "-"
          + TStringConversion::intToString(strings[i].offset,
              TStringConversion::baseHex);
        if (!item.idOverride.empty()) idString = item.idOverride;
        
        std::string content = item.content;
        std::string prefix = "";
        std::string suffix = "";
        
        prefix = item.prefixBase + prefix;
        suffix = item.suffixBase + suffix;
    
//    std::cerr << content << std::endl;
        
//        content = toUtf8(content);
//        prefix = toUtf8(prefix);
//        suffix = toUtf8(suffix);
        
//        std::cerr << content << std::endl;
        
        // items flagged as possibly-not-existing do not get a dummy entry
        // in the translation sheet; we assume the user will add them
        // as needed
        if (!item.mayNotExist) {
          dst.addStringEntry(
            idString, content, prefix, suffix, item.translationPlaceholder);
        }
        
        ofs << "#STARTSTRING("
          << "\"" << idString << "\""
/*          << ", "
          << TStringConversion::intToString(item.offset,
              TStringConversion::baseHex)
          << ", "
          << TStringConversion::intToString(item.size,
              TStringConversion::baseHex)*/
          << ")" << endl;
        
        if (item.type == YmmGenericString::type_mapString) {
          ofs << "#SETSCRIPTREF("
            << TStringConversion::intToString(item.scriptRefStart,
              TStringConversion::baseHex)
            << ", "
            << TStringConversion::intToString(item.scriptRefEnd,
              TStringConversion::baseHex)
            << ", "
            << TStringConversion::intToString(item.scriptRefCode,
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.freeSpaces.size(); i++) {
          ofs << "#ADDFREESPACE("
            << TStringConversion::intToString(item.freeSpaces[i].pos,
              TStringConversion::baseHex)
            << ", "
            << TStringConversion::intToString(item.freeSpaces[i].size,
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.pointerRefs.size(); i++) {
          ofs << "#ADDPOINTERREF("
            << TStringConversion::intToString(item.pointerRefs[i],
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.overwriteAddresses.size(); i++) {
          ofs << "#ADDOVERWRITE("
            << TStringConversion::intToString(item.overwriteAddresses[i],
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.extraIds.size(); i++) {
          ofs << "#ADDEXTRAID("
            << TStringConversion::intToString(item.extraIds[i],
              TStringConversion::baseHex)
            << ")"
            << endl;
        }
        
        for (unsigned int i = 0; i < item.genericLines.size(); i++) {
          ofs << item.genericLines[i] << std::endl;
        }
        
        if (item.mayNotExist)
          ofs << "#IMPORTIFEXISTS(\"";
        else
          ofs << "#IMPORT(\"";
        
        ofs << idString << "\")" << endl;
        
        ofs << "#ENDSTRING()" << endl;
        ofs << endl;
        
        ++strNum;
      }
      else if (item.type == YmmGenericString::type_setRegion) {
        ofs << "#STARTREGION("
          << item.regionId
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == YmmGenericString::type_setMap) {
        ofs << "#SETMAP("
          << item.mapMainId
          << ", "
          << item.mapSubId
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == YmmGenericString::type_setNotCompressible) {
        ofs << "#SETNOTCOMPRESSIBLE("
          << (item.notCompressible ? 1 : 0)
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == YmmGenericString::type_addOverwrite) {
        ofs << "#ADDOVERWRITE("
          << TStringConversion::intToString(item.offset,
            TStringConversion::baseHex)
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == YmmGenericString::type_addFreeSpace) {
        ofs << "#ADDFREESPACE("
          << TStringConversion::intToString(item.offset,
            TStringConversion::baseHex)
          << ", "
          << TStringConversion::intToString(item.size,
            TStringConversion::baseHex)
          << ")" << endl;
        ofs << endl;
      }
      else if (item.type == YmmGenericString::type_genericLine) {
        ofs << item.content << endl;
        ofs << endl;
      }
      else if (item.type == YmmGenericString::type_comment) {
        dst.addCommentEntry(item.content);
        
        ofs << "//===================================" << endl;
        ofs << "// " << item.content << endl;
        ofs << "//===================================" << endl;
        ofs << endl;
      }
      else if (item.type == YmmGenericString::type_marker) {
        dst.addMarkerEntry(item.content);
        
        ofs << "// === MARKER: " << item.content << endl;
        ofs << endl;
      }
    }
  }
  
protected:
  
};

const int satsnd_headerSize = 0x20;
const unsigned int satsnd_initialDecryptionKey = 0x90BC;
const unsigned int satsnd_decryptionKeyIncrement = 0x7654;
// truncated from 16279.1015625 -- see note in dumpSatSoundFile()
// about "sample rate" value in header
const int soundSampleRate = 16279;

int dumpSatSoundFile(TStream& ifs, string outName) {
  ifs.seek(0);
  if (ifs.readu32be() != 0x00000610) {
    cerr << "Skipping sound '" << outName << "': bad header?" << endl;
//    return false;
    return 0;
  }

  // this value represents the "sample rate", more or less.
  // it appears to be a raw value for the PITCH register;
  // all files seem to set it to 0x71E8.
  // running the conversions, this corresponds to
  // ((44100 / 4) * 1.4765625 = 16279.1015625 Hz
  // why that goddamn number? i don't know, but whatever.
  // i'm not going to bother doing all the conversions here
  // when it's just this constant for all files
//  ifs.seek(0x4);
//  int rawSampleRate = ifs.readu16be();

  ifs.seek(0x10);
  int sampleCount = ifs.readu32be();
  // ???
  // this seems to be garbage data, sometimes bits of other sounds...?
//  int extraAreaStart = satsnd_headerSize + sampleCount;
  // i don't even know what this is...
//  int extraArea2Start = satsnd_headerSize + ifs.readu32be();
  
  // this happens occasionally (e.g. C437A.DAT, file 4)
  // seems to be related to looping sound effects
  if (sampleCount == 0) {
//    cout << "note: " << outName << " has nonstandard size field" << endl;
    sampleCount = ifs.readu32be();
  }
  
  TSoundFile sound;
  sound.setChannels(1);
  sound.setRate(soundSampleRate);

  #if EXPORT_SOUNDS  
    ifs.seek(satsnd_headerSize);
    // TEST
  //  sampleCount = (ifs.size() - ifs.tell()) / 2;
    unsigned int decryptionKey = satsnd_initialDecryptionKey;
    for (int i = 0; i < sampleCount; i++) {
      int nextSample = ifs.reads16be();
      nextSample ^= decryptionKey;
      decryptionKey = (decryptionKey + satsnd_decryptionKeyIncrement) & 0xFFFF;
      
      sound.addSampleS16(nextSample);
    }
    
    TBufStream ofs;
    sound.exportWav(ofs);
    ofs.save(outName.c_str());
  #endif
  
  return sampleCount;
}

void addPlaceholderString(YmmGenericStringSet& strings,
    std::string baseName, int id) {
  YmmGenericString str;
  str.type = YmmGenericString::type_string;
  str.idOverride = baseName
    + "-special-"
    + TStringConversion::intToString(id);
  str.offset = 0;
  str.size = 0;
  str.mayNotExist = true;
  strings.strings.push_back(str);
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix script template generator" << endl;
    cout << "Usage: " << argv[0] << " <infolder> <outprefix> [engine]" << endl;
    cout << "<infolder> is a directory containing the scene files to scan." << endl;
    cout << "Engine may be 'mcd' or 'sat' (default: sat)." << endl;
    
    return 0;
  }
  
  std::string inFolder(argv[1]);
  std::string outPrefix(argv[2]);
  
  TFileManip::createDirectory(outPrefix);
  
  YmmEngineVersions::YmmEngineVersion engineVersion
    = YmmEngineVersions::sat;
  if (argc >= 4) {
    std::string versionStr(argv[3]);
    if (versionStr.compare("sat") == 0)
      engineVersion = YmmEngineVersions::sat;
    else if (versionStr.compare("mcd") == 0)
      engineVersion = YmmEngineVersions::mcd;
  }
  
  TFileManip::FileInfoCollection inFileInfo;
  TFileManip::getDirectoryListing(inFolder.c_str(), inFileInfo);
  
  // sort by name
  map<string, string> sortedMap;
  for (const TFileInfo& info: inFileInfo) {
    sortedMap[info.name()] = info.path();
  }
  
  YmmGenericStringSet strings;
  YmmGenericStringSet choiceStrings;
  
//  for (FileInfoCollection::iterator it = inFileInfo.begin();
  int regionNum = 0;
  for (pair<string, string> namePair: sortedMap) {
//    cout << namePair.first << " " << namePair.second << endl;

    std::string sceneShortName = namePair.first;
    std::string sceneFullName = namePair.second;
    
    cout << "processing: " << sceneShortName << endl;
    
    YmmScene scene;
    // enable e.g. substitution of decompiled choice balloon parameters
    // with placeholder variables
    scene.translationData().doTranslationModifications = true;
    {
      TBufStream ifs;
      ifs.open(sceneFullName.c_str());
      scene.read(ifs, engineVersion);
    }
    
    #if GENERATE_CHOICE_TEMPLATE
      // palette for generating legible text
      MdPaletteLine choicePalLine;
      // bg
      choicePalLine.colors[15].r = 0xFF;
      choicePalLine.colors[15].g = 0xFF;
      choicePalLine.colors[15].b = 0xFF;
      // fg
      choicePalLine.colors[14].r = 0x00;
      choicePalLine.colors[14].g = 0x00;
      choicePalLine.colors[14].b = 0x00;
      
      TFileManip::createDirectory("rsrc_raw/choices");
      
      // HACK: scene C418, alone of all scenes in the game,
      // uses a PARAMWORD for the tilemap number to allow
      // for a simulation of a choice tree.
      // this fucks everything up, so we'll just work around it.
      if (sceneShortName.compare("C418.DAT") == 0) {
        // replace original values with new hardcoded ones
        scene.translationData().choiceData.clear();
        
        //loc_00006d64: op02  (0x0001 0x0000 0x0005 0x0006 0x0006 0x0004)
        YmmChoiceData choice0;
        choice0.tilemapId = 0;
        choice0.x = 5;
        choice0.y = 6;
        choice0.w = 6;
        choice0.h = 4;
        scene.translationData().choiceData.push_back(choice0);
        
        //loc_00006d81: op02  (0x0001 0x0080 0x0005 0x0006 0x0006 0x0006)
        YmmChoiceData choice1;
        choice1.tilemapId = 1;
        choice1.x = 5;
        choice1.y = 6;
        choice1.w = 6;
        choice1.h = 6;
        scene.translationData().choiceData.push_back(choice1);
        choice1.tilemapId = 2;
        scene.translationData().choiceData.push_back(choice1);
      }
      
      if (scene.translationData().choiceData.size() > 0) {
        #if EXPORT_CHOICE_SCENES
          // create directory to hold decompiled scene data
          string decompiledSceneDst = string("scenes_unpacked/")
            + sceneShortName;
          TFileManip::createDirectory(decompiledSceneDst);
          // dump decompiled scene
          scene.save((decompiledSceneDst + "/").c_str());
//          char c;
//          cin >> c;
        #endif
      
      
        choiceStrings.addComment(string("File: ") + sceneShortName);
        choiceStrings.addSetRegion(regionNum);
        choiceStrings.addGenericLine(string("#SETFILE(\"")
          + sceneShortName
          + "\")");
        choiceStrings.addGenericLine(string("#SETCHOICECOUNT(")
          + TStringConversion::intToString(
              scene.translationData().choiceData.size())
          + ")");
        
        // choices occur only in single-subscene scenes,
        // so this is okay
        YmmSubscene& subscene = scene.subscene(0);
        
        int choiceNum = 0;
        
        //=====
        // dump choice data
        //=====
        
        for (const YmmChoiceData& choiceData:
              scene.translationData().choiceData) {
          // yes, this is how the game figures out the number of choices:
          // height of the balloon's interior in tiles divided by 2
          int numChoices = choiceData.h / 2;
          
          //=====
          // dump choice graphics
          //=====
          
          #if EXPORT_CHOICE_IMAGES
            // set up VRAM
            MdVram vram;
            for (int i = 0; i < subscene.patterns_.size(); i++) {
              vram.setPattern(i, subscene.patterns_[i]);
            }
            
            TGraphic choiceGrpRaw;
  //          subscene.bgMaps_.at(choiceData.tilemapId + 1)
  //            .toGrayscaleGraphic(choiceGrpRaw, vram);
            subscene.bgMaps_.at(choiceData.tilemapId + 1)
              .toColorGraphic(choiceGrpRaw, vram, choicePalLine);
            
  //          TPngConversion::graphicToRGBAPng("test.png", choiceGrpRaw);
            
            string choiceStringGrpDst = string("rsrc_raw/choices/")
              + sceneShortName;
  //          TFileManip::createDirectory(choiceStringGrpDst);
        
            for (int i = 0; i < numChoices; i++) {
              int x = choiceTilemapBaseX;
              int y = choiceTilemapBaseY + (i * choiceTilemapOptionPixelH);
              int w = choiceData.w * MdPattern::w;
              int h = choiceTilemapOptionPixelH;
              
              TGraphic choiceGrp(w, h);
              choiceGrp.copy(choiceGrpRaw,
                TRect(0, 0, 0, 0),
                TRect(x, y, w, h));
              
              std::string outFileName = choiceStringGrpDst + "_"
                + "choice"
                + TStringConversion::intToString(choiceNum)
                + "-"
                + TStringConversion::intToString(i)
                + ".png";
              TPngConversion::graphicToRGBAPng(outFileName, choiceGrp);
            }
          #endif
          
          //=====
          // generate string data
          //=====
          
          {
            
            YmmGenericString str;
            str.type = YmmGenericString::type_string;
            str.idOverride = sceneShortName
              + "-choice-"
              + TStringConversion::intToString(choiceNum)
              + "-props";
            str.offset = 0;
            str.size = 0;
            
            str.addPropSet("tilemapId",
              TStringConversion::intToString(choiceData.tilemapId,
                TStringConversion::baseHex));
            str.addPropSet("x",
              TStringConversion::intToString(choiceData.x,
                TStringConversion::baseHex));
            str.addPropSet("y",
              TStringConversion::intToString(choiceData.y,
                TStringConversion::baseHex));
            str.addPropSet("w",
              TStringConversion::intToString(choiceData.w,
                TStringConversion::baseHex));
            str.addPropSet("h",
              TStringConversion::intToString(choiceData.h,
                TStringConversion::baseHex));
            str.addPropSet("autoW",
              TStringConversion::toString(1));

            choiceStrings.strings.push_back(str);
          }
          
          for (int i = 0; i < numChoices; i++) {
            YmmGenericString str;
            str.type = YmmGenericString::type_string;
            str.idOverride = sceneShortName
              + "-choice-"
              + TStringConversion::intToString(choiceNum)
              + "-"
              + TStringConversion::intToString(i);
            str.offset = 0;
            str.size = 0;

            choiceStrings.strings.push_back(str);
          }
          
//          char c;
//          cin >> c;
          
          //=====
          // prep for next choice
          //=====
          
          ++choiceNum;
        }
      }
    #endif
    
    #if GENERATE_SOUND_TEMPLATE
      // ignore the common case of a single-subscene file with no sound files
//      if (scene.numSubscenes() == 1
//          && scene.subscene(0).pcmData_.size() == 0) continue;
      if (scene.numSubscenes() == 1
          && scene.subscene(0).pcmData_.size() == 0
          && (sceneShortName.compare("C226A.CUT") != 0)
          && (sceneShortName.compare("C945.CUT") != 0)) {
        // generate a region so we can get the auto-generated
        // start line, but don't put add the comment
        // or anything else that will show up in the output sheet
/*        strings.addSetRegion(regionNum);
        strings.addGenericLine(string("#SETFILE(\"")
          + sceneShortName
          + "\")");
        strings.addGenericLine(string("#SETSOUNDCOUNT(")
          + TStringConversion::intToString(scene.subscene(0).pcmData_.size())
          + ")");*/
      }
      else {
        string sceneBaseDir = outPrefix + "/" + sceneShortName;
      
    //    TFileManip::createDirectory(sceneBaseDir);
        
        strings.addComment(string("File: ") + sceneShortName);
        
        strings.addSetRegion(regionNum);
        
        strings.addGenericLine(string("#SETFILE(\"")
          + sceneShortName
          + "\")");
        
        strings.addGenericLine(string("#SETSOUNDCOUNT(")
          + TStringConversion::intToString(scene.subscene(0).pcmData_.size())
          + ")");
        
        for (int i = 0; i < scene.numSubscenes(); i++) {
          YmmSubscene& subscene = scene.subscene(i);
          
    //      string outBaseName = sceneBaseDir + "/" + sceneShortName + "_ss"
    //        + TStringConversion::intToString(i) + "_";
          string outBaseName = sceneBaseDir + "_ss"
            + TStringConversion::intToString(i) + "_";
          
          for (int j = 0; j < subscene.pcmData_.size(); j++) {
            TArray<TByte> data = subscene.pcmData_[j];
            TBufStream ifs;
            ifs.write((const char*)data.data(), data.size());
            ifs.seek(0);
            
            string outName = outBaseName + "sound"
              + TStringConversion::intToString(j) + ".wav";
            int sampleCount = dumpSatSoundFile(ifs, outName);
            if (sampleCount != 0) {
    //          strings.addRawString("", j, 0);
              YmmGenericString str;
              str.type = YmmGenericString::type_string;
              str.idOverride = sceneShortName + "-ss"
                + TStringConversion::intToString(i)
                + "-"
                + TStringConversion::intToString(j);
              str.offset = 0;
              str.size = 0;
              
              // length of sound in seconds
              str.addPropSet("soundLen",
                TStringConversion::toString(
                  (double)sampleCount / (double)soundSampleRate));
              // this automatically generates a wait command and a
              // slot-off command at the end of the data, with the time
              // set to some fixed point beyond the end of the sound
              // (say, a quarter of a second) as given by soundLen.
              // this avoids the need to explicitly specify an off time
              // for the final line.
    //          str.addPropSet("soundAutoOff", "1");
              
              strings.strings.push_back(str);
            }
          }
        }
      }
    #endif
      
    // add "dummy" entry for scene-start script
    YmmGenericString str;
    str.type = YmmGenericString::type_string;
    str.idOverride = sceneShortName
      + "-start";
    str.offset = 0;
    str.size = 0;
    str.mayNotExist = true;
    strings.strings.push_back(str);
    
    // HACK: for scenes where we need extra scripts for
    // e.g. sign subtitles, add placeholders
    int numPlaceholderStrings = 0;
    if (sceneShortName.compare("C117.DAT") == 0) numPlaceholderStrings = 10;
    
    for (int i = 0; i < numPlaceholderStrings; i++) {
//      addPlaceholderString(strings, sceneShortName,
//        scene.subscene(0).pcmData_.size() + i + 1);
      addPlaceholderString(strings, sceneShortName, i);
    }
    
    ++regionNum;
  }
  
  //=======================================
  // export
  //=======================================
  
  YmmTranslationSheet scriptSheet;
  YmmTranslationSheet choiceSheet;
  
  {
    std::ofstream ofs((outPrefix + "/spec_scene.txt").c_str());
    strings.exportToSheet(scriptSheet, ofs, "");
    scriptSheet.exportCsv((outPrefix + "/script_scene.csv").c_str());
  }
  
  {
    std::ofstream ofs((outPrefix + "/spec_choice.txt").c_str());
    choiceStrings.exportToSheet(choiceSheet, ofs, "");
    choiceSheet.exportCsv((outPrefix + "/script_choice.csv").c_str());
  }
  
/*//  TIfstream ifs(infile, ios_base::binary);
  TBufStream ifs;
  ifs.open(infile.c_str());
  
  YmmScene scene;
  scene.read(ifs, engineVersion);
  
  // save
  scene.save(outprefix.c_str()); */
  
  return 0;
}
