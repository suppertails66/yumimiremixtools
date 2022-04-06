#include "ymm/YmmSubtitleSet.h"
#include "ymm/YmmAblkOp.h"
#include "ymm/YmmAblkOps.h"
#include "exception/TGenericException.h"
#include "util/TStringConversion.h"
#include "util/TBufStream.h"

using namespace BlackT;

namespace Md {


YmmSubtitleSet::YmmSubtitleSet() { }
  
void YmmSubtitleSet::addSubtitle(YmmSubtitleId id, YmmSubtitle subtitle) {
  subtitles[id] = subtitle;
}

YmmSubtitle YmmSubtitleSet::getSubtitle(YmmSubtitleId id) const {
  return subtitles.at(id);
}


}
