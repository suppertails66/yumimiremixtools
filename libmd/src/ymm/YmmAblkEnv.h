#ifndef YMM_ABLK_ENV_H
#define YMM_ABLK_ENV_H


#include "ymm/YmmTranslationData.h"
#include <map>
#include <string>
#include <vector>

namespace Md {


struct LabelTarget {
  std::string name;
  int offset;
  int width;
};

class YmmAblkEnv {
public:
  YmmAblkEnv();
  
  void addLabel(std::string name, int pos);
//  void addLabelTarget(std::string name, int width);
  void addLabelTarget(std::string name, int offset, int width);
  
  std::map<std::string, int> labels;
  std::vector<LabelTarget> labelTargets;
  YmmTranslationData* translationData;
protected:
  
};


}


#endif
