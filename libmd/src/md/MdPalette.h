#ifndef MDPALETTE_H
#define MDPALETTE_H


#include "util/TColor.h"
#include "util/TByte.h"
#include "util/TTwoDArray.h"
#include "util/TTwoDByteArray.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "md/MdPaletteLine.h"

namespace Md {


struct MdPalette {
  const static int numLines = 4;
  
  MdPaletteLine lines[numLines];
  
  MdPalette();
  
  MdPalette(const BlackT::TByte* src);
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
};


}


#endif 
