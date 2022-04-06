#include "ymm/YmmSubtitleSceneSet.h"
#include "ymm/YmmAblkOp.h"
#include "ymm/YmmAblkOps.h"
#include "exception/TGenericException.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"
#include <iostream>

using namespace BlackT;

namespace Md {


YmmSubtitleSceneSet::YmmSubtitleSceneSet() { }
  
void YmmSubtitleSceneSet::addSubtitle(std::string name, int subscene,
                   YmmSubtitleId id, YmmSubtitle subtitle) {
  subtitleSets[name][subscene].addSubtitle(id, subtitle);
//  std::cerr << name << "|" << subscene << "|" << id << "|" << subtitle.text << std::endl;
}

YmmSubtitle YmmSubtitleSceneSet::getSubtitle(std::string name, int subscene,
                   YmmSubtitleId id) {
//  return subtitleSets.at(name).at(subscene).getSubtitle(id);
  return subtitleSets[name][subscene].getSubtitle(id);
}

YmmSubtitleSet& YmmSubtitleSceneSet
    ::getSubtitleSet(std::string name, int subscene) {
//  return subtitleSets.at(name).at(subscene);
  return subtitleSets[name][subscene];
}

bool YmmSubtitleSceneSet::hasSubtitle(std::string name, int subscene,
                 YmmSubtitleId id) const {
  NameToSubsceneSetMap::const_iterator findIt = subtitleSets.find(name);
  if (findIt == subtitleSets.end()) return false;
  
  SubsceneToSetMap::const_iterator findIt2 = findIt->second.find(subscene);
  if (findIt2 == findIt->second.end()) return false;
  
  YmmSubtitleSet::SubtitleItemMap::const_iterator findIt3
    = findIt2->second.subtitles.find(id);
  if (findIt3 == findIt2->second.subtitles.end()) return false;
  
  return true;
}


}
