#ifndef YMMSUBTITLEBUFFER_H
#define YMMSUBTITLEBUFFER_H


#include "ymm/YmmSubtitle.h"
#include "util/TStream.h"
#include <vector>
#include <map>
#include <string>

namespace Md {


class YmmAblkEnv;

class YmmSubtitleBuffer {
public:
  typedef std::map<YmmSubtitleId, YmmSubtitle> SubtitleItemMap;  
  
  YmmSubtitleBuffer();
  
  bool modifiedSinceLastDraw;
  SubtitleItemMap items;
  
  void addItem(YmmSubtitleId id, const YmmSubtitle& item);
  void removeItem(YmmSubtitleId id);
  void write(BlackT::TStream& dst,
             YmmAblkEnv& env) const;
  
  void removeItemsWithStyleName(std::string styleName);
  void clear();
protected:
  
};


}


#endif
