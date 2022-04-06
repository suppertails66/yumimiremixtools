#include "ymm/YmmScriptReader.h"
#include "ymm/YmmLineWrapper.h"
#include "ymm/YmmScene.h"
#include "md/MdPattern.h"
#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TGraphic.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TPngConversion.h"
#include "util/TFreeSpace.h"
#include "util/TBitmapFont.h"
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#define BUILD_SCENES 1

using namespace std;
using namespace BlackT;
using namespace Md;

const static int sectorSize = 0x800;

const static int textCharsStart = 0x00;
const static int textCharsEnd = textCharsStart + 0x80;

// note: leave leftmost two patterns blank,
// as they're reserved for the cursor
const static int choiceTilemapBaseX = 16;
const static int choiceTilemapBaseY = 256;
const static int choiceTilemapOptionPixelH = 16;
const static int choiceTilemapMaxRawTileW = 20;
const static int choiceTilemapMaxTextTileW
  = choiceTilemapMaxRawTileW - (choiceTilemapBaseX / MdPattern::w);
// maximum of 4 options
const static int choiceTilemapMaxTextBlockPixelH
  = choiceTilemapOptionPixelH * 4;
// not including cursor margin on left
const static int minimumChoiceW = 4;

std::string subsMagicSig = "SUBS";



TThingyTable table;
TThingyTable tableScene;
TThingyTable tableChoice;
TBitmapFont fontChoice;

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

std::string getNumStr(int num) {
  std::string str = TStringConversion::intToString(num);
  while (str.size() < 2) str = string("0") + str;
  return str;
}

std::string getHexByteNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  return string("$") + str;
}

std::string getHexWordNumStr(int num) {
  std::string str = TStringConversion::intToString(num,
    TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 4) str = string("0") + str;
  return string("$") + str;
}
                      

void binToDcb(TStream& ifs, std::ostream& ofs) {
  int constsPerLine = 16;
  
  while (true) {
    if (ifs.eof()) break;
    
    ofs << "  .db ";
    
    for (int i = 0; i < constsPerLine; i++) {
      if (ifs.eof()) break;
      
      TByte next = ifs.get();
      ofs << as2bHexPrefix(next);
      if (!ifs.eof() && (i != constsPerLine - 1)) ofs << ",";
    }
    
    ofs << std::endl;
  }
}




void buildScene(std::string sceneName) {
  std::string inPath = std::string("out/scenes/") + sceneName
    + "/";
  
  YmmScene scene;
  scene.load(inPath.c_str());
  TBufStream ofs;
  scene.write(ofs);
  ofs.save((std::string("out/files/") + sceneName).c_str());
}

void exportGenericRegion(YmmScriptReader::ResultRegion& region,
                         std::string prefix) {
  for (YmmScriptReader::ResultCollection::iterator it = region.strings.begin();
       it != region.strings.end();
       ++it) {
    if (it->str.size() <= 0) continue;
    
    YmmScriptReader::ResultString str = *it;
    
    std::string outName = prefix + str.id + ".bin";
    TFileManip::createDirectoryForFile(outName);
    
    TBufStream ofs;
    ofs.writeString(str.str);
    ofs.save(outName.c_str());
  }
}

void prepAndExportChoiceScene(YmmScriptReader::ResultRegion& region,
                     std::string inPrefix,
                     std::string outPrefix) {
  YmmTranslationData translationData;
  
  int baseStringIndex = 0;
  for (int i = 0; i < region.choiceCount; i++) {
    std::string choiceBaseId =
      region.filename + "-choice-"
      + TStringConversion::intToString(i)
      + "-";
//    std::string propsBaseId = choiceBaseId + "props";
    
    YmmScriptReader::ResultString propsResult
      = region.strings.at(baseStringIndex);
    
    // read in choice properties
    int choiceTilemapId = TStringConversion::stringToInt(
      propsResult.properties["tilemapId"]);
    int choiceX = TStringConversion::stringToInt(
      propsResult.properties["x"]);
    int choiceY = TStringConversion::stringToInt(
      propsResult.properties["y"]);
    // subtract 2 to account for reserved cursor space on left side
    int choiceW = TStringConversion::stringToInt(
      propsResult.properties["w"]) - 2;
    int choiceH = TStringConversion::stringToInt(
      propsResult.properties["h"]);
    bool choiceAutoW = propsResult.propertyIsTrue("autoW");
    
    int numChoiceOptions = choiceH / 2;
    
    // if auto-width mode on, set width to minimum value
    // that the engine can handle correctly
    // (we'll use it later as the minimum width value
    // to be exceeded)
    if (choiceAutoW) choiceW = minimumChoiceW;
    
    // get target graphic to put generated choices in
    // (note that choiceTilemapId is 1 less than the actual target)
    std::string choiceGraphicName =
      std::string("out/scenes/")
      + region.filename
      + "/scene0_bgmap"
      + TStringConversion::intToString(choiceTilemapId + 1)
      + ".png";
    
//    std::cerr << choiceGraphicName << std::endl;
//    char c;
//    std::cin >> c;
    // read in grayscale choice bitmap
    TGraphic choiceRawGrp;
    TPngConversion::RGBAPngToGraphic(choiceGraphicName, choiceRawGrp);
    
    // blank out old options
    choiceRawGrp.fillRect(
      choiceTilemapBaseX, choiceTilemapBaseY,
      choiceTilemapMaxTextTileW * MdPattern::w,
      choiceTilemapMaxTextBlockPixelH,
      TColor(0xFF, 0xFF, 0xFF));
    
    // generate and place each option
    for (int j = 0; j < numChoiceOptions; j++) {
      // put target string in a vector for rendering
//      TBufStream msgIfs;
//      msgIfs.writeString(region.strings.at(1 + j).str);
//      msgIfs.seek(0);
      std::string msgString
        = region.strings.at((baseStringIndex + 1) + j).str;
      std::vector<int> msgIds;
      msgIds.reserve(msgString.size());
      for (unsigned int k = 0; k < msgString.size(); k++) {
        if (msgString[k] == 0) break;
        msgIds.push_back(msgString[k]);
      }
      
      // render string content to graphic
      TGraphic optionGrpFg;
      fontChoice.renderPrecodedString(optionGrpFg, msgIds);
      // i may, in fact, not be a very good programmer
      optionGrpFg.regenerateTransparencyModel();
      
      // determine output size
      int outputTileW = (optionGrpFg.w() + (MdPattern::w - 1)) / MdPattern::w;
      int outputTileH = choiceTilemapOptionPixelH / MdPattern::w;
      int outputPixelW = outputTileW * MdPattern::w;
      int outputPixelH = outputTileH * MdPattern::h;
      
      // blit generated string onto background of appropriate color
/*      TGraphic optionGrpFinal(outputPixelW, outputPixelH);
      optionGrpFinal.clear(TColor(0xFF, 0xFF, 0xFF));
      optionGrpFinal.blit(optionGrpFg, TRect(0, 0, 0, 0));*/
      
      // copy to appropriate position of target graphic
      choiceRawGrp.blit(optionGrpFg,
        TRect(choiceTilemapBaseX,
              choiceTilemapBaseY + (choiceTilemapOptionPixelH * j),
              outputPixelW,
              outputPixelH));
      
      // if autowidth on, update if longest choice seen
      if (choiceAutoW && (outputTileW > choiceW)) choiceW = outputTileW;
    }
    
    if (choiceW > choiceTilemapMaxTextTileW) {
      throw TGenericException(T_SRCANDLINE,
                "prepAndExportChoiceScene()",
                std::string("Choice text exceeds maximum width of box: ")
                + "longest option was "
                + TStringConversion::intToString(choiceW)
                + " tiles, of an allowed "
                + TStringConversion::intToString(choiceTilemapMaxTextTileW));
    }
    
    // output updated graphic
    TPngConversion::graphicToRGBAPng(choiceGraphicName, choiceRawGrp);
    
    // update variable substitution data
    std::string translationVarBaseName
      = std::string("CHOICE")
        + TStringConversion::intToString(i)
        + "_";
    translationData.variables[translationVarBaseName + "X"]
      = TStringConversion::intToString(choiceX);
    translationData.variables[translationVarBaseName + "Y"]
      = TStringConversion::intToString(choiceY);
    translationData.variables[translationVarBaseName + "W"]
      = TStringConversion::intToString(choiceW + 2);
    translationData.variables[translationVarBaseName + "H"]
      = TStringConversion::intToString(choiceH);
    
    // move to next choice base index
    baseStringIndex += (numChoiceOptions + 1);
  }
  
  //========
  // export updated scene
  //========
  
  std::string inPath = std::string("out/scenes/") + region.filename
    + "/";
  
  YmmScene scene;
  // add variable substitution data to scene before building
  scene.translationData() = translationData;
  
/*  for (YmmTranslationData::NameToVarMap::iterator it = scene.translationData().variables.begin();
       it != scene.translationData().variables.end();
       ++it) {
    std::cerr << it->first << " " << it->second << std::endl;
  }*/
  
  scene.load(inPath.c_str());
  TBufStream ofs;
  scene.write(ofs);
  ofs.save((std::string("out/files/") + region.filename).c_str());
//  buildScene(region.filename);
}

void exportSceneFile(YmmScriptReader::ResultRegion& region,
                     std::string inPrefix,
                     std::string outPrefix) {
  // do nothing if no content
  if (region.strings.size() <= 0) return;
  
//  cout << "exporting scene file: " << region.filename << endl;
  
  TBufStream ofs;
  ofs.writeString(subsMagicSig);
  // reserve space for size
  ofs.writeu32be(0);
  // count of sound files.
  // this would not be necessary except that the original game
  // has at least one script bug where it attempts to play a sound which
  // does not exist (in scene C334B), and we need to be able to check for
  // this condition and prevent trying to start a subtitle script in that case.
  ofs.writeu32be(region.soundCount);
  
//  bool hasContent = false;
/*  for (YmmScriptReader::ResultCollection::iterator it = region.strings.begin();
       it != region.strings.end();
       ++it) {
//    if (it->str.size() <= 0) continue;
    
    YmmScriptReader::ResultString str = *it;
    
    std::string outName = prefix + str.id + ".bin";
    TFileManip::createDirectoryForFile(outName);
    
    TBufStream ofs;
    ofs.writeString(str.str);
    ofs.save(outName.c_str());
    
    ++strNum;
  }*/
  
  // we expect to read in the following strings, in order:
  // 1. one string per sound file
  // 2. (optional, required if (3.) present) scene-start string
  // 3. (optional) any additional strings
  // 
  // these are output in the following order:
  // 1. scene-start string (dummy/null if it doesn't exist)
  // 2. each sound file string
  // 3. additional strings
  
  YmmScriptReader::ResultCollection sortedResults;
  
  // scene-start string
  bool hasSceneStartString = false;
  if (region.strings.size() > region.soundCount) {
    sortedResults.push_back(region.strings[region.soundCount]);
    hasSceneStartString = true;
  }
  else {
    // dummy
    sortedResults.push_back(YmmScriptReader::ResultString());
  }
  
  // sound strings
  for (int i = 0; i < region.soundCount; i++) {
    sortedResults.push_back(region.strings[i]);
  }
  
  // anything else
  for (int i = region.soundCount + 1; i < region.strings.size(); i++) {
    sortedResults.push_back(region.strings[i]);
  }
  
  // generate offset table
  int currentOffset = sortedResults.size() * 4;
  for (int i = 0; i < sortedResults.size(); i++) {
    // dummy entries for non-existent start string
    // and any strings with no content (size of 1 = terminator only)
    if (((i == 0) && !hasSceneStartString)
        || (sortedResults[i].str.size() <= 1)) {
      ofs.writeu32be(0);
      continue;
    }
    
    ofs.writeu32be(currentOffset);
    currentOffset += sortedResults[i].str.size();
  }
  
  // write strings
  for (int i = 0; i < sortedResults.size(); i++) {
    if (((i == 0) && !hasSceneStartString)
        || (sortedResults[i].str.size() <= 1)) {
      continue;
    }
    
    ofs.writeString(sortedResults[i].str);
  }
  
  // pad to 4-byte boundary
  ofs.alignToBoundary(4);
  
  // fill in size
  ofs.seek(4);
  ofs.writeu32be(ofs.size());
  ofs.seek(ofs.size());
  
  // prepend subtitle block to source file
  TBufStream ifs;
  ifs.open((inPrefix + region.filename).c_str());
  ofs.writeFrom(ifs, ifs.size());
  
  // write to output
  ofs.save((outPrefix + region.filename).c_str());
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix script builder" << endl;
    cout << "Usage: " << argv[0] << " [inprefix] [outprefix]"
      << endl;
    return 0;
  }
  
//  string infile(argv[1]);
  string inPrefix(argv[1]);
  string outPrefix(argv[2]);
  
  try {

  //  table.readUtf8("table/ymm_en.tbl");
  //  tableScene.readUtf8("table/ymm_scenes_en.tbl");
    
    tableScene.readUtf8("font/scene/table.tbl");
    tableChoice.readUtf8("font/choice/table.tbl");
    fontChoice.load("font/choice/");
    
    //=====
    // read script
    //=====
    
    YmmScriptReader::NameToRegionMap scriptResults;
    {
      TBufStream ifs;
      ifs.open((inPrefix + "spec_scene.txt").c_str());
      YmmScriptReader(ifs, scriptResults, tableScene)();
    }
    
    YmmScriptReader::NameToRegionMap choiceResults;
    {
      TBufStream ifs;
      ifs.open((inPrefix + "spec_choice.txt").c_str());
      YmmScriptReader(ifs, choiceResults, tableChoice)();
    }
    
  //  generateCompressionDictionary(
  //    scriptResults, outPrefix + "script_dictionary.bin");
  
    #if BUILD_SCENES
    
      //=====
      // export choice strings
      // (this also builds all scenes containing choices)
      //=====
      
      // keep track of which scenes have been built
      // so we don't do a redundant build later
      std::map<std::string, bool> sceneBuiltMap;
      for (YmmScriptReader::NameToRegionMap::iterator it = choiceResults.begin();
           it != choiceResults.end();
           ++it) {
        std::cout << "building choice scene file: "
          << it->second.filename << std::endl;
        
        prepAndExportChoiceScene(it->second, "out/scenes/", "out/files/");
        // mark as built
        sceneBuiltMap[it->second.filename] = true;
        
    //    std::cerr << it->second.filename << std::endl;
      }
      
      //=====
      // build any remaining scenes in the scene folder
      // that do not include choice strings
      //=====
      
    //  std::cerr << "remaining:" << std::endl;
      {
        TFileManip::FileInfoCollection sceneFolderInfo;
        TFileManip::getDirectoryListing("out/scenes", sceneFolderInfo);
        
        for (const TFileInfo& fileInfo: sceneFolderInfo) {
          // skip previously build scenes
          if (sceneBuiltMap.find(fileInfo.name()) != sceneBuiltMap.end())
            continue;
          
          std::cout << "building extra scene file: "
            << fileInfo.name() << std::endl;
          
    /*      std::string inPath = std::string("out/scenes/") + fileInfo.name()
            + "/";
          
          YmmScene scene;
          scene.load(inPath.c_str());
          TBufStream ofs;
          scene.write(ofs);
          ofs.save((std::string("out/files/") + fileInfo.name()).c_str());*/
          buildScene(fileInfo.name());
          
    //      std::cerr << fileInfo.name() << std::endl;
        }
      }
    
    #endif
    
    //=====
    // export strings to scene files;
    // this must happen last, after the scenes have been built,
    // as the new string block is simply taped to the start of each file
    //=====
    
    for (YmmScriptReader::NameToRegionMap::iterator it = scriptResults.begin();
         it != scriptResults.end();
         ++it) {
      exportSceneFile(it->second, "out/files/", "out/files/");
    }
  }
  catch (TGenericException& e) {
    std::cerr << "Exception: " << e.problem() << std::endl;
    throw e;
  }
  catch (TException& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    throw e;
  }
  
  return 0;
}
