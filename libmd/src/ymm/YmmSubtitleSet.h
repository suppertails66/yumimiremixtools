#ifndef YMMSUBTITLESET_H
#define YMMSUBTITLESET_H


#include "ymm/YmmSubtitle.h"
#include "util/TStream.h"
#include <vector>
#include <map>

namespace Md {


class YmmSubtitleSet {
public:
  typedef std::map<YmmSubtitleId, YmmSubtitle> SubtitleItemMap;
  
  YmmSubtitleSet();
  
  void addSubtitle(YmmSubtitleId id, YmmSubtitle subtitle);
  YmmSubtitle getSubtitle(YmmSubtitleId id) const;
  
  SubtitleItemMap subtitles;
  
protected:
  
};


}


#endif
