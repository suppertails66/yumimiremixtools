#ifndef YMM_TILEMAP_H
#define YMM_TILEMAP_H


#include "md/MdVram.h"
#include "md/MdPalette.h"
#include "util/TTwoDArray.h"
#include "util/TArray.h"
#include "util/TStream.h"
#include "util/TGraphic.h"
#include <vector>

namespace Md {


// In contrast to standard tilemaps, Yumimi Mix's "tilemaps" are simply
// raw 16-bit tile numbers (no flipping or priority, and may exceed size of
// VRAM) and are stored in column-order format.
class YmmTilemap {
public:
  YmmTilemap();
  
  void resize(int w__, int h__);
  int getTileId(int x, int y) const;
  void setTileId(int x, int y, int id);
  void read(BlackT::TStream& ifs, int w__, int h__);
  void write(BlackT::TStream& ofs) const;
  
  int x() const;
  int y() const;
  void setX(int x__);
  void setY(int y__);
  
  void toColorGraphic(BlackT::TGraphic& dst,
                      const MdVram& vram,
                      const MdPaletteLine& pal) const;
  
  void toGrayscaleGraphic(BlackT::TGraphic& dst,
                      const MdVram& vram) const;
                      
  void fromGrayscaleGraphic(const BlackT::TGraphic& src,
                            std::vector<MdPattern>& patterns);
  
  int w() const;
  int h() const;
  
protected:
  BlackT::TTwoDArray<int> tileIds_;
  int x_;
  int y_;
};


}


#endif
