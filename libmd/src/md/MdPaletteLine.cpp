#include "md/MdPaletteLine.h"
#include "util/ByteConversion.h"

using namespace std;
using namespace BlackT;

namespace Md {

  
MdPaletteLine::MdPaletteLine() { }

MdPaletteLine::MdPaletteLine(const TByte* src) {
  for (int i = 0; i < numColors; i++) {
    int raw = ByteConversion::fromBytes(src + (i * 2),
                                         ByteSizes::uint16Size,
                                         EndiannessTypes::big,
                                         SignednessTypes::nosign);
    
    colors[i] = MdColor(raw);
  }
}

TColor MdPaletteLine::colorAtIndex(int index) const {
  TColor col = colors[index].toColor();
  
  if (index == 0) {
    col.setA(TColor::fullAlphaTransparency);
  }
  
  return col;
}

int MdPaletteLine::indexOfColor(BlackT::TColor color) const {
  // Any fully transparent color matches index 0
  if (color.a() == TColor::fullAlphaTransparency) return 0;

  for (int i = 1; i < numColors; i++) {
    if (colors[i].toColor() == color) return i;
  }
  
  return -1;
}

void MdPaletteLine::generatePreview(TGraphic& dst, int colorW, int colorH) const {
  dst.resize(colorW * numColors, colorH);

  for (int i = 0; i < numColors; i++) {
    int xPos = i * colorW;
    
    TColor color = colorAtIndex(i);
    
    dst.fillRect(xPos, 0,
                 colorW, colorH,
                 color);
  }
}

void MdPaletteLine::read(BlackT::TStream& ifs) {
  for (int i = 0; i < numColors; i++) {
    colors[i] = MdColor(ifs.readu16be());
  }
}

void MdPaletteLine::write(BlackT::TStream& ofs) const {
  for (int i = 0; i < numColors; i++) {
    colors[i].write(ofs);
//    ofs.writeu16be(colors[i].
  }
}


} 
