#ifndef MDPATTERN_H
#define MDPATTERN_H


#include "util/TColor.h"
#include "util/TByte.h"
#include "util/TTwoDArray.h"
#include "util/TTwoDByteArray.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "md/MdPaletteLine.h"


namespace Md {


struct MdPattern {
  typedef BlackT::TTwoDArray<BlackT::TByte> PatternArray;
  
  PatternArray pattern;
  
  const static int w = 8;
  const static int h = 8;
  const static int uncompressedSize = 32;
  
  MdPattern();
    
  void toGrayscaleGraphic(BlackT::TGraphic& dst,
                 int x = 0, int y = 0) const;
    
  void toColorGraphic(BlackT::TGraphic& dst, MdPaletteLine line,
                 int x = 0, int y = 0) const;
    
  int fromColorGraphic(const BlackT::TGraphic& src, MdPaletteLine line,
                 int xoffset = 0, int yoffset = 0);
    
  int fromGrayscaleGraphic(const BlackT::TGraphic& src,
                 int xoffset = 0, int yoffset = 0);
    
  void toGraphic(BlackT::TGraphic& dst, MdPaletteLine line,
                 int x = 0, int y = 0) const;

  void read(const char* src);
  void write(char* dst) const;
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  bool operator==(const MdPattern& m) const;
  
  void flipHorizontal();
  void flipVertical();
  
};


}


#endif 
