#include "ymm/YmmSubtitleBuffer.h"
#include "ymm/YmmAblkOp.h"
#include "ymm/YmmAblkOps.h"
#include "ymm/YmmAblkEnv.h"
#include "exception/TGenericException.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"

using namespace BlackT;

namespace Md {


YmmSubtitleBuffer::YmmSubtitleBuffer()
  : modifiedSinceLastDraw(false) { }

void YmmSubtitleBuffer::addItem(YmmSubtitleId id, const YmmSubtitle& item) {
//  std::cerr << "add: " << id << " " << items.size() << std::endl;
  if (items.find(id) != items.end()) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmSubtitleBuffer::addItem()",
                            std::string("Added item to already-active ID: ")
                            + TStringConversion::intToString(id)
                            );
  }
  
  items[id] = item;
  modifiedSinceLastDraw = true;
  
//  std::cerr << "after add: " << id << " " << items.size() << std::endl;
}

void YmmSubtitleBuffer::removeItem(YmmSubtitleId id) {
//  std::cerr << "remove: " << id << " " << items.size() << std::endl;
  SubtitleItemMap::iterator findIt = items.find(id);
  if (findIt == items.end()) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmSubtitleBuffer::removeItem()",
                            std::string("Tried to remove inactive ID: ")
                            + TStringConversion::intToString(id)
                            );
  }
  
  items.erase(findIt);
  modifiedSinceLastDraw = true;
}

void YmmSubtitleBuffer::write(BlackT::TStream& dst,
                              YmmAblkEnv& env) const {
  for (SubtitleItemMap::const_iterator it = items.cbegin();
       it != items.cend();
       ++it) {
    const YmmSubtitle& item = it->second;
    
    item.write(dst, env);
  }
}

void YmmSubtitleBuffer::removeItemsWithStyleName(std::string styleName) {
  SubtitleItemMap::iterator it = items.begin();
  while (it != items.end()) {
    if (it->second.styleName.compare(styleName) == 0) {
      ++it;
      SubtitleItemMap::iterator nextIt = it;
      --it;
      
      items.erase(it);
      modifiedSinceLastDraw = true;
      
      it = nextIt;
    }
    else ++it;
  }
}

void YmmSubtitleBuffer::clear() {
//  std::cerr << "clear" << std::endl;
  if (items.size() > 0) modifiedSinceLastDraw = true;
  items.clear();
}


}
