#ifndef MDTILEID_H
#define MDTILEID_H


#include "util/TByte.h"
#include "util/TGraphic.h"
#include "util/TStream.h"
#include "md/MdVram.h"
#include "md/MdPalette.h"

namespace Md {


class MdTileId {
public:
  const static int size = 2;
  
  MdTileId();
  
  void read(const char* data);
  void write(char* data) const;
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  void fromRawValue(int raw);
  int toRawValue() const;
  
  void toColorGraphic(BlackT::TGraphic& dst,
                      const MdVram& vram,
                      const MdPalette& pal,
                      int x = 0, int y = 0) const;
  
  void toGrayscaleGraphic(BlackT::TGraphic& dst,
                      const MdVram& vram,
                      int x = 0, int y = 0) const;
  
  bool priority;
  int palette;
  bool vflip;
  bool hflip;
  int pattern;
  
protected:
  
};


}


#endif
