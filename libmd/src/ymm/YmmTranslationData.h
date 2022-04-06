#ifndef YMM_TRANSLATION_DATA_H
#define YMM_TRANSLATION_DATA_H


#include "util/TStream.h"
#include <vector>
#include <map>

namespace Md {


struct YmmChoiceData {
  int tilemapId;
  int x;
  int y;
  int w;
  int h;
};

typedef std::vector<YmmChoiceData> YmmChoiceDataCollection;

class YmmTranslationData {
public:

  typedef std::map<std::string, std::string> NameToVarMap;

  YmmTranslationData();
  
  bool doTranslationModifications;
  
  YmmChoiceDataCollection choiceData;
  NameToVarMap variables;
  
protected:
  
};


}


#endif
