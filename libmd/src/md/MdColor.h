#ifndef MDCOLOR_H
#define MDCOLOR_H


#include "util/TColor.h"
#include "util/TByte.h"
#include "util/TTwoDArray.h"
#include "util/TTwoDByteArray.h"
#include "util/TGraphic.h"
#include "util/TStream.h"

namespace Md {


struct MdColor {

  MdColor();
  
  MdColor(int rawCol);
  
  int rawColor() const;
  BlackT::TColor toColor() const;
  void fromRawColor(int rawCol);
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  int r;
  int g;
  int b;
  
};


}


#endif 
