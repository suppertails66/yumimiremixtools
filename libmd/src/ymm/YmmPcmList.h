#ifndef YMM_PCM_LIST_H
#define YMM_PCM_LIST_H


#include "util/TStream.h"
#include "ymm/YmmEngineVersions.h"
#include <vector>

namespace Md {


struct YmmPcmIdentifier {
  int offset;
  int size;
};

class YmmPcmList {
public:
  YmmPcmList();
  
  // Reads a PCM list, starting from the first PCM entry (i.e. one position
  // after the subscene offset).
  void read(BlackT::TStream& ifs);
  void addEntry(int offset, int size);
  
  std::vector<YmmPcmIdentifier> pcmIds;
protected:
  
};


}


#endif
