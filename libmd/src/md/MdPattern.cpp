#include "md/MdPattern.h"
#include "util/ByteConversion.h"

using namespace std;
using namespace BlackT;

namespace Md {


MdPattern::MdPattern()
  : pattern(w, h) { }
  
void MdPattern::toGrayscaleGraphic(TGraphic& dst,
                          int x, int y) const {
//    dst.resize(w, h);
  
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int raw = pattern.data(i, j);
      TColor color = TColor(raw * 16, raw * 16, raw * 16,
                    (raw == 0) ? TColor::fullAlphaTransparency
                               : TColor::fullAlphaOpacity);
      
      dst.setPixel(x + i, y + j, color);
    }
  }
}
  
void MdPattern::toColorGraphic(TGraphic& dst, MdPaletteLine line,
                          int x, int y) const {
//    dst.resize(w, h);
  
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int raw = pattern.data(i, j);
      TColor color = line.colorAtIndex(raw);
      dst.setPixel(x + i, y + j, color);
    }
  }
}
  
int MdPattern::fromColorGraphic(const BlackT::TGraphic& src,
               MdPaletteLine line,
               int xoffset, int yoffset) {
  
  // note that this simply chooses _a_ color that matches the
  // one from the source (if one exists in the palette).
  // if the same color is present multiple times, you may get
  // a different index from the original, which could cause problems
  // with dynamic palettes
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int x = xoffset + i;
      int y = yoffset + j;
    
      TColor color = src.getPixel(x, y);
      int raw = line.indexOfColor(color);
      if (raw < 0) return raw;
      pattern.data(i, j) = raw;
    }
  }
  
  return 0;
}
  
int MdPattern::fromGrayscaleGraphic(const BlackT::TGraphic& src,
               int xoffset, int yoffset) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int x = xoffset + i;
      int y = yoffset + j;
                               
      TColor color = src.getPixel(x, y);
      
      int raw;
      if (color.a() == TColor::fullAlphaTransparency) {
        raw = 0;
      }
      else {
        raw = color.r() / 16;
      }
      
//        if (raw < 0) return raw;
      pattern.data(i, j) = raw;
    }
  }
  
  return 0;
}
  
void MdPattern::toGraphic(TGraphic& dst, MdPaletteLine line,
                          int x, int y) const {
//    dst.resize(w, h);
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int raw = pattern.data(i, j);
      TColor color = line.colorAtIndex(raw);
      dst.setPixel(x + i, y + j, color);
    }
  }
}

void MdPattern::read(const char* src) {
  int pos = 0;
  for (int j = 0; j < MdPattern::h; j++) {
    for (int i = 0; i < MdPattern::w; i += 2) {
      unsigned char b = (unsigned char)(src[pos++]);
      
      int second = (b & 0x0F);
      int first = (b & 0xF0) >> 4;
      
      pattern.data(i, j) = first;
      pattern.data(i + 1, j) = second;
    }
  }
}

void MdPattern::write(char* dst) const {
  for (int j = 0; j < MdPattern::h; j++) {
    for (int i = 0; i < MdPattern::w; i += 2) {
      int first = (char)(pattern.data(i, j));
      int second = (char)(pattern.data(i + 1, j));
      
      unsigned char b = second;
      b |= (first << 4);
      *(dst++) = b;
    }
  }
}

bool MdPattern::operator==(const MdPattern& m) const {
  for (int j = 0; j < MdPattern::h; j++) {
    for (int i = 0; i < MdPattern::w; i++) {
      if (pattern.data(i, j) != m.pattern.data(i, j)) return false;
    }
  }
  
  return true;
}

void MdPattern::flipHorizontal() {
  for (int j = 0; j < pattern.h(); j++) {
    int s = 0;
    int f = pattern.w() - 1;
    
    while (s < f) {
      TByte temp = pattern.data(s, j);
      pattern.data(s, j) = pattern.data(f, j);
      pattern.data(f, j) = temp;
      ++s;
      --f;
    }
  }
}

void MdPattern::flipVertical() {
  for (int i = 0; i < pattern.w(); i++) {
    int s = 0;
    int f = pattern.h() - 1;
    
    while (s < f) {
      TByte temp = pattern.data(i, s);
      pattern.data(i, s) = pattern.data(i, f);
      pattern.data(i, f) = temp;
      ++s;
      --f;
    }
  }
}

void MdPattern::read(BlackT::TStream& ifs) {
  int pos = 0;
  for (int j = 0; j < MdPattern::h; j++) {
    for (int i = 0; i < MdPattern::w; i += 2) {
      unsigned char b = (unsigned char)(ifs.readu8be());
      
      int second = (b & 0x0F);
      int first = (b & 0xF0) >> 4;
      
      pattern.data(i, j) = first;
      pattern.data(i + 1, j) = second;
    }
  }
}

void MdPattern::write(BlackT::TStream& ofs) const {
  for (int j = 0; j < MdPattern::h; j++) {
    for (int i = 0; i < MdPattern::w; i += 2) {
      int first = (char)(pattern.data(i, j));
      int second = (char)(pattern.data(i + 1, j));
      
      unsigned char b = second;
      b |= (first << 4);
      ofs.writeu8be((int)b);
    }
  }
}



} 
