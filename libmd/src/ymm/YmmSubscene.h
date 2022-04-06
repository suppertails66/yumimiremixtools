#ifndef YMM_SUBSCENE_H
#define YMM_SUBSCENE_H


#include "ymm/YmmPcmList.h"
#include "ymm/YmmTilemap.h"
#include "ymm/YmmSubtitleSet.h"
#include "ymm/YmmSubtitleResources.h"
#include "ymm/YmmEngineVersions.h"
#include "ymm/YmmTranslationData.h"
#include "md/MdPattern.h"
#include "md/MdTilemap.h"
#include "md/MdPalette.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TStream.h"
#include "util/TThingyTable.h"
#include <string>
#include <vector>

namespace Md {


class YmmSubscene {
public:
  YmmSubscene();
  
  void read(BlackT::TStream& ifs, int baseOffset,
            const YmmPcmList& pcmList,
            YmmEngineVersions::YmmEngineVersion engineVersion
              = YmmEngineVersions::mcd);
  void write(BlackT::TStream& ofs,
            YmmPcmList& pcmList) const;
  
  void writeWithGeneratedSubs(BlackT::TStream& ofs,
            YmmPcmList& pcmList);
  
  void save(const char* prefix) const;
  void load(const char* prefix);
  
  void addObjMap(BlackT::TGraphic& grp, int x, int y);
  
  int numObjMaps() const;
  
  std::string& script();
  
  void setUpTranslationData(YmmTranslationData& translationData__);
  void setUpSubtitleParams(BlackT::TThingyTable& thingy__,
                           YmmSubtitleSet& subtitleSet__,
                           YmmSubtitleResources& subtitleResources__);

  MdPalette initPalette_;
  std::vector<MdPattern> patterns_;
//  int patternLoadAddr_;
  std::vector<YmmTilemap> bgMaps_;
  std::vector<YmmTilemap> objMaps_;
  int unknown_;
  std::string script_;
//  BlackT::TArray<BlackT::TByte> mysteryData_;
  std::vector< BlackT::TArray<BlackT::TByte> > pcmData_;
  
  YmmTranslationData* translationData_;
  // for subtitle injection
  BlackT::TThingyTable* thingy_;
  YmmSubtitleSet* subtitleSet_;
  YmmSubtitleResources* subtitleResources_;
protected:
  const static int bgMapTileWidth_ = 80;
  const static int bgMapTileHeight_ = 64;
  const static int bytesPerBgMap_ = bgMapTileWidth_ * bgMapTileHeight_
    * MdTileId::size;
  const static int pcmDetectionHeuristicCutoffSize = 0x200;
  const static int pcmSectorSize = 0x800;

  const static char* infoFileName_;
  const static char* initPaletteName_;
  const static char* patternsName_;
  const static char* bgMapsName_;
  const static char* objMapsName_;
  const static char* scriptName_;
  const static char* mysteryDataName_;
  const static char* pcmDataName_;
  
  // ugggggggggh
  void writeSub1(BlackT::TStream& ofs,
            YmmPcmList& pcmList,
            int startPos) const;
  void writeSub2(BlackT::TStream& ofs,
            YmmPcmList& pcmList,
            int startPos) const;
};


}


#endif
