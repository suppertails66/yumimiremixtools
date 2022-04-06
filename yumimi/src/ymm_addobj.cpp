#include "ymm/YmmScene.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TStringConversion.h"
#include "util/TPngConversion.h"
#include "exception/TGenericException.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 6) {
    cout << "Yumimi Mix object map injector" << endl;
    cout << "Usage: " << argv[0]
      << " <scene_inprefix> <subscene_num> <png> <x> <y> <scene_outprefix>"
      << endl;
    
    return 0;
  }
  
  string sceneInprefix = string(argv[1]);
  int subsceneNum = TStringConversion::stringToInt(string(argv[2]));
  string grpName = string(argv[3]);
  int objX = TStringConversion::stringToInt(string(argv[4]));
  int objY = TStringConversion::stringToInt(string(argv[5]));
  string sceneOutprefix = string(argv[6]);
  
  YmmScene scene;
  scene.load(sceneInprefix.c_str());
  
  YmmSubscene& subscene = scene.subscene(subsceneNum);
  
  TGraphic grp;
  TPngConversion::RGBAPngToGraphic(grpName, grp);
  
  subscene.addObjMap(grp, objX, objY);
  
  int newObjIndex = subscene.numObjMaps() - 1;
  
  std::string& script = subscene.script();
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
  script.reserve(script.size() + 256);
  
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
  }
  
  // convert scene script to a stream for processing
  TBufStream scriptIfs;
  
  
  
  // done
  scene.save(sceneOutprefix.c_str());

  return 0;
}
