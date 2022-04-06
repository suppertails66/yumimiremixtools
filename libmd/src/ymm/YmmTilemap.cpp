#include "ymm/YmmTilemap.h"

using namespace BlackT;

namespace Md {


YmmTilemap::YmmTilemap()
  : x_(0), y_(0) { }
  
void YmmTilemap::resize(int w__, int h__) {
  tileIds_.resize(w__, h__);
}

int YmmTilemap::getTileId(int x, int y) const {
  return tileIds_.data(x, y);
}

void YmmTilemap::setTileId(int x, int y, int id) {
  tileIds_.data(x, y) = id;
}

void YmmTilemap::read(BlackT::TStream& ifs, int w__, int h__) {
  resize(w__, h__);
  for (int j = 0; j < w__; j++) {
    for (int i = 0; i < h__; i++) {
      tileIds_.data(j, i) = ifs.readu16be();
    }
  }
}

void YmmTilemap::write(BlackT::TStream& ofs) const {
  for (int j = 0; j < w(); j++) {
    for (int i = 0; i < h(); i++) {
      ofs.writeu16be(tileIds_.data(j, i));
    }
  }
}
  
int YmmTilemap::x() const {
  return x_;
}

int YmmTilemap::y() const {
  return y_;
}

void YmmTilemap::setX(int x__) {
  x_ = x__;
}

void YmmTilemap::setY(int y__) {
  y_ = y__;
}

void YmmTilemap::toColorGraphic(BlackT::TGraphic& dst,
                    const MdVram& vram,
                    const MdPaletteLine& pal) const {
  dst.resize(tileIds_.w() * MdPattern::w,
             tileIds_.h() * MdPattern::h);
  dst.clearTransparent();
  
  for (int j = 0; j < h(); j++) {
    for (int i = 0; i < w(); i++) {
      int tileId = tileIds_.data(i, j);
      vram.getPattern(tileId).toColorGraphic(dst, pal,
          (i * MdPattern::w), (j * MdPattern::h));
    }
  }
}

void YmmTilemap::toGrayscaleGraphic(BlackT::TGraphic& dst,
                    const MdVram& vram) const {
  dst.resize(tileIds_.w() * MdPattern::w,
             tileIds_.h() * MdPattern::h);
  dst.clearTransparent();
  
  for (int j = 0; j < h(); j++) {
    for (int i = 0; i < w(); i++) {
      int tileId = tileIds_.data(i, j);
//      if (tileId > 0x8000) continue;
      vram.getPattern(tileId).toGrayscaleGraphic(dst,
          (i * MdPattern::w), (j * MdPattern::h));
    }
  }
}
                    
void YmmTilemap::fromGrayscaleGraphic(const BlackT::TGraphic& src,
                          std::vector<MdPattern>& patterns) {
  int newW = src.w() / MdPattern::w;
  int newH = src.h() / MdPattern::h;
  resize(newW, newH);
  
  // note that normally I would nest these loops in the opposite order to get
  // a left-to-right ordering, but this appears to be how Game Arts did it
  for (int i = 0; i < newW; i++) {
    for (int j = 0; j < newH; j++) {
      int xpos = i * MdPattern::w;
      int ypos = j * MdPattern::h;
      
      MdPattern pattern;
      pattern.fromGrayscaleGraphic(src, xpos, ypos);
      
      bool found = false;
      for (int k = 0; k < patterns.size(); k++) {
        if (patterns[k] == pattern) {
          tileIds_.data(i, j) = k;
          found = true;
          break;
        }
      }
      
      if (!found) {
        tileIds_.data(i, j) = patterns.size();
        patterns.push_back(pattern);
      }
    }
  }
}

int YmmTilemap::w() const {
  return tileIds_.w();
}

int YmmTilemap::h() const {
  return tileIds_.h();
}


}
