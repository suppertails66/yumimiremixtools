#ifndef YMM_SCENE_H
#define YMM_SCENE_H


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TThingyTable.h"
#include "ymm/YmmSubscene.h"
#include "ymm/YmmSubtitleSceneSet.h"
#include "ymm/YmmEngineVersions.h"
#include "ymm/YmmTranslationData.h"
#include <vector>
#include <string>

namespace Md {


namespace YmmSceneTypes {
  enum YmmSceneType {
    ablk,
    oneshot
  };
}

class YmmScene {
public:
  YmmScene();
  
  void read(BlackT::TStream& ifs,
    YmmEngineVersions::YmmEngineVersion engineVersion__
      = YmmEngineVersions::mcd);
  void write(BlackT::TStream& ofs) const;
  
  void writeWithGeneratedSubs(BlackT::TStream& ofs);
  
  void save(const char* prefix) const;
  void load(const char* prefix);
  
  int numSubscenes() const;
  
  YmmSubscene& subscene(int index);
  
  YmmTranslationData& translationData();
  const YmmTranslationData& translationData() const;
  
  void setUpSubtitleParams(std::string fileName__,
                           BlackT::TThingyTable& thingy__,
                           YmmSubtitleSceneSet& subtitleSceneSet__,
                           YmmSubtitleResources& subtitleResources__);
protected:
  const static char* sceneName_;
  const static char* infoFileName_;
  
  // for subtitle injection
  std::string fileName_;
  YmmTranslationData translationData_;
  BlackT::TThingyTable* thingy_;
  YmmSubtitleSceneSet* subtitleSceneSet_;
  YmmSubtitleResources* subtitleResources_;

  YmmEngineVersions::YmmEngineVersion engineVersion_;
  YmmSceneTypes::YmmSceneType sceneType_;
  
  std::vector<YmmSubscene> subscenes_;
  
  void writeSub1(BlackT::TStream& ofs) const;
  void writeSub2(BlackT::TStream& ofs,
                 std::vector<BlackT::TBufStream>& sceneBuffers,
                 std::vector<YmmPcmList>& pcmLists,
                 int& numPcmFiles) const;
};


}


#endif
