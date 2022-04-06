#include "ymm/YmmScene.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "util/TBitmapFont.h"
#include "util/TThingyTable.h"
#include "exception/TGenericException.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 8) {
    cout << "Yumimi Mix label injector" << endl;
    cout << "Usage: " << argv[0]
      << " <scene_inprefix> <text> <font> <table> <x> <y> <scene_outprefix>"
      << endl;
    
    return 0;
  }
  
  string sceneInprefix = string(argv[1]);
  string text = string(argv[2]);
  string fontPrefix = string(argv[3]);
  string tableName = string(argv[4]);
  int objX = TStringConversion::stringToInt(string(argv[5]));
  int objY = TStringConversion::stringToInt(string(argv[6]));
  string sceneOutprefix = string(argv[7]);
  
  TBitmapFont font;
  font.load(fontPrefix);
  
  TThingyTable table;
  table.readSjis(tableName);
  
  YmmScene scene;
  scene.load(sceneInprefix.c_str());
  
  for (int i = 0; i < scene.numSubscenes(); i++) {
    YmmSubscene& subscene = scene.subscene(i);
    
    std::string labelText = text;
    if (scene.numSubscenes() > 1) {
      // hyphen
/*      labelText += std::string("\x4B");
      
      std::string numstr = TStringConversion::intToString(i);
      for (int i = 0; i < numstr.size(); i++) {
        numstr[i] = numstr[i] - (int)'0' + 1;
      }
      labelText += numstr; */
      
      labelText += std::string("-") + TStringConversion::intToString(i);
    }
    
    TBufStream textStr;
    textStr.writeString(labelText);
    textStr.seek(0);
    
    TGraphic grp;
    font.render(grp, textStr, table);
    
    // resize to tile boundary
//    if (((grp.w() % 8) != 0) || ((grp.h() % 8) != 0)) {
      int targetW = 80;
//      int offsetX = (targetW - grp.w()) / 2;
      int offsetX = (targetW - grp.w());
      if (offsetX < 0) offsetX = 0;
      
      TGraphic finalgrp;
//      finalgrp.resize(grp.w() + (8 - (grp.w() % 8)),
//                      grp.h() + (8 - (grp.h() % 8)));
      finalgrp.resize(targetW,
                      grp.h() + (8 - (grp.h() % 8)));
      finalgrp.clearTransparent();
      finalgrp.copy(grp,
                    TRect(offsetX, 0, 0, 0));
      grp = finalgrp;
//    }
    
//    TPngConversion::RGBAPngToGraphic(grpName, grp);
    
    subscene.addObjMap(grp, objX, objY);
    
    int newObjIndex = subscene.numObjMaps() - 1;
    
    std::string& script = subscene.script();
    
    script = std::string(".objoverlayon ")
        + TStringConversion::intToString(0x12345678)
        + " "
        + TStringConversion::intToString(newObjIndex)
        + " "
        + TStringConversion::intToString(232)
        + " "
        + TStringConversion::intToString(200)
        + '\n'
//        + "setPaletteColors              (0x30 < 0x0000 0x0000 0x0AEE 0x0EC8 0x08CE 0x06AE 0x0046 0x0026 0x06EE 0x04CE 0x008E 0x006A 0x0024 0x0EA8 0x0620 0x0EEC > )"
        + "setPaletteColors              (0x31 < 0x0000 0x0577 > )"
        + '\n'
        + '\n'
        + script;
    
/*    MdColor textColor;
    textColor.fromRawColor(0x0AEE);
    MdColor outlineColor;
    outlineColor.fromRawColor(0x0000);
    
    subscene.initPalette_.lines[3].colors[1] = outlineColor;
    subscene.initPalette_.lines[3].colors[2] = textColor; */
    
//    script = std::string("op02                          (0x0000)")
//        + '\n'
//        + script;
    
    // HACK HACK HACK HACK HACK
    // std::search operates only on iterators.
    // and if all we have is an iterator we have to
    // add to the string a character at a time.
    // but if the string reallocates from adding a character,
    // that iterator is invalidated.
    // so do this extremely stupid and platform-unsafe hack to ensure
    // there will be no reallocation.
    // this is just for testing and will not be part of the final build step
    // anyway.
/*    script.reserve(script.size() + 256);
    
    std::string searchStr = "transferPatternData";
    std::string::iterator findIt
      = std::search(script.begin(), script.end(),
                    searchStr.begin(), searchStr.end());
    
    if (findIt != script.end()) {
      while ((findIt != script.end())
             && (*findIt != '\n')) ++findIt;
      if (findIt != script.end()) ++findIt;
      
      std::string injectedString
        = std::string("injectedObj")
          + TStringConversion::intToString(newObjIndex)
          + ": renderObjsHigh?               (< "
          + TStringConversion::intToString(newObjIndex,
                TStringConversion::baseHex)
          + " > )"
          // assume unix linebreaks...
          + '\n';
      
      for (std::string::iterator it = injectedString.begin();
           it != injectedString.end();
           ++it) {
        script.insert(findIt, *it);
        ++findIt;
      }
    } */
  }
  
  
  
  // done
  scene.save(sceneOutprefix.c_str());

  return 0;
}
