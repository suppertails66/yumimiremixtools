#include "md/MdColor.h"
#include "util/ByteConversion.h"

using namespace std;
using namespace BlackT;

namespace Md {


MdColor::MdColor()
  : r(0), g(0), b(0) { }

MdColor::MdColor(int rawCol)
  : r(0), g(0), b(0) {
  fromRawColor(rawCol);
}

int MdColor::rawColor() const {
  return (r << 1) | (g << 5) | (b << 9);
}

TColor MdColor::toColor() const {
//    return TColor(r << 5, g << 5, b << 5,
//                  !(r && g && b)
//                    ? TColor::fullAlphaTransparency
//                    : TColor::fullAlphaOpacity);
  return TColor(r << 5, g << 5, b << 5,
                TColor::fullAlphaOpacity);
}

void MdColor::fromRawColor(int rawCol) {
  r = (rawCol & 0x000E) >> 1;
  g = (rawCol & 0x00E0) >> 5;
  b = (rawCol & 0x0E00) >> 9;
}
  
void MdColor::read(BlackT::TStream& ifs) {
  fromRawColor(ifs.readu16be());
}

void MdColor::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(rawColor());
}



}
