#ifndef MDPALETTELINE_H
#define MDPALETTELINE_H


#include "util/TColor.h"
#include "util/TByte.h"
#include "util/TTwoDArray.h"
#include "util/TTwoDByteArray.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "md/MdColor.h"

namespace Md {


struct MdPaletteLine {
  
  const static int numColors = 16;
  const static int uncompressedSize = numColors * 2;
  
  MdPaletteLine();
  
  MdPaletteLine(const BlackT::TByte* src);
  
  BlackT::TColor colorAtIndex(int index) const;
  int indexOfColor(BlackT::TColor color) const;
  
  void generatePreview(BlackT::TGraphic& dst,
      int colorW = 16, int colorH = 16) const;
  
  MdColor colors[numColors];
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
};


}


#endif 
