#ifndef YMMSUBTITLESCENESET_H
#define YMMSUBTITLESCENESET_H


#include "ymm/YmmSubtitle.h"
#include "ymm/YmmSubtitleSet.h"
#include "util/TStream.h"
#include <vector>
#include <map>
#include <string>

namespace Md {


class YmmSubtitleSceneSet {
public:
  typedef std::map<int, YmmSubtitleSet> SubsceneToSetMap;
  
  YmmSubtitleSceneSet();
  
  void addSubtitle(std::string name, int subscene,
                   YmmSubtitleId id, YmmSubtitle subtitle);
  YmmSubtitle getSubtitle(std::string name, int subscene,
                   YmmSubtitleId id);
  YmmSubtitleSet& getSubtitleSet(std::string name, int subscene);
  bool hasSubtitle(std::string name, int subscene,
                   YmmSubtitleId id) const;
  
protected:
  typedef std::map<std::string, SubsceneToSetMap> NameToSubsceneSetMap;
  
  NameToSubsceneSetMap subtitleSets;
  
};


}


#endif
