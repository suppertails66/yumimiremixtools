#include "ymm/YmmPcmList.h"

namespace Md {


YmmPcmList::YmmPcmList() { }
  
void YmmPcmList::read(BlackT::TStream& ifs) {
  // PCM entries are sequential, and identified by having the high bit set
  // but the high nybble not equal to 0xF (indicating scene list terminator).
  // The terminator for PCM entries will either have the high bit unset
  // (indicating a new subscene offset) or the high nybble set to 0xF
  // (indicating end of list). 
  int prev;
  int next = ifs.readu32be();
  while (((next & 0x80000000) != 0)
         && ((next & 0xF0000000) != 0xF0000000)) {
    YmmPcmIdentifier id;
    id.offset = (next & 0x00FFFFFF);
    
    // get next entry
    prev = next;
    next = ifs.readu32be();
    
    if ((unsigned int)next == 0xFFFFFFFF) {
      // ???
      // occurs in e.g. ENDC000A.DAT
      id.size = ifs.size() - (prev & 0x00FFFFFF);
    }
    else id.size = (next & 0x00FFFFFF) - (prev & 0x00FFFFFF);    
        
    pcmIds.push_back(id);
  }
  
  // Move stream back to point to next offset (or list terminator)
  ifs.seekoff(-4);
}

void YmmPcmList::addEntry(int offset, int size) {
  YmmPcmIdentifier id { offset, size };
  pcmIds.push_back(id);
}


}
