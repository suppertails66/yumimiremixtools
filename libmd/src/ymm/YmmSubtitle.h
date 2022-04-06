#ifndef YMMSUBTITLE_H
#define YMMSUBTITLE_H


#include "util/TStream.h"
#include <vector>
#include <string>

namespace Md {


typedef double YmmSubtitleId;
class YmmAblkEnv;

class YmmSubtitle {
public:
  struct SubObjEntry {
    int graphicIndex;
    int x;
    int y;
  };
  
  typedef std::vector<SubObjEntry> SubObjEntryCollection;

  YmmSubtitle();
  
//  int graphicIndex;
  SubObjEntryCollection subObjEntries;
  std::string text;
  std::string styleName;
  
  void write(BlackT::TStream& dst,
             YmmAblkEnv& env) const;
protected:
  
};


}


#endif
