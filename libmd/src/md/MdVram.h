#ifndef MDVRAM_H
#define MDVRAM_H


#include "util/TByte.h"
#include "md/MdPattern.h"

namespace Md {


class MdVram {
public:
//  const static int numPatterns = 0x800;
  // purely for convenience
  const static int numPatterns = 0x8000;
  
  const static int numBytes = numPatterns * MdPattern::uncompressedSize;

  MdVram();
  
  const MdPattern& getPattern(int index) const;
  MdPattern setPattern(int index, const MdPattern& pattern);
  
  void read(const char* data, int numPatterns,
            int startPattern = 0);
protected:
  
  MdPattern patterns[numPatterns];
  
};


}


#endif
