#ifndef YMM_ABLK_EVALUATOR_H
#define YMM_ABLK_EVALUATOR_H


#include "ymm/YmmAblkLexStream.h"
#include "ymm/YmmAblkEnv.h"
#include "ymm/YmmRenderBuffer.h"
#include "ymm/YmmSubtitleBuffer.h"
#include "ymm/YmmSubtitleSet.h"
#include "ymm/YmmSubtitleResources.h"
#include "ymm/YmmTranslationData.h"
#include "util/TStream.h"
#include "util/TThingyTable.h"
#include <map>
#include <string>
#include <map>

namespace Md {


class YmmSubscene;

class YmmAblkEvaluator {
public:
  YmmAblkEvaluator(YmmAblkLexStream& src__,
                   BlackT::TStream& dst__);
  
  void evaluate();
  
  void setUpTranslationData(YmmTranslationData& translationData__);
  void setUpSubtitleParams(YmmSubscene& subscene__,
                           BlackT::TThingyTable& thingy__,
                           YmmSubtitleSet& subtitleSet__,
                           YmmSubtitleResources& subtitleResources__);
protected:
  YmmAblkLexStream* src_;
  BlackT::TStream* dst_;
  YmmAblkEnv env_;
  
//  typedef std::map<std::string, int> LabelToOffsetMap;
//  LabelToOffsetMap labels;
  
  YmmRenderBuffer renderBuffer;
  YmmSubtitleBuffer subtitleBuffer;
  bool patternsTransferred;
  bool pendingAfterDraw;
  
//  YmmTranslationData* translationData_;
  // for subtitle injection
  YmmSubscene* subscene_;
  BlackT::TThingyTable* thingy_;
  YmmSubtitleSet* subtitleSet_;
  YmmSubtitleResources* subtitleResources_;
  
  void evaluateStatement();
  void evaluateDirective(std::string name, int lineNum);
  
  void generateSubtitleContent(YmmSubtitle& subtitle);
  void addSubtitle(YmmSubtitleId id);
  YmmSubtitleId nextSubtitleId();
};


}


#endif
