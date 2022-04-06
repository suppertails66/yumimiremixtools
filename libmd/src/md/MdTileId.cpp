#include "md/MdTileId.h"
#include "util/ByteConversion.h"
#include "util/TStringConversion.h"

using namespace BlackT;

namespace Md {


MdTileId::MdTileId()
  : priority(false),
    palette(0),
    vflip(false),
    hflip(false),
    pattern(0) { }

void MdTileId::read(const char* data) {
  int raw = ByteConversion::fromBytes(data, 2,
    EndiannessTypes::big, SignednessTypes::nosign);
  fromRawValue(raw);
}

void MdTileId::write(char* data) const {
  int raw = toRawValue();
  
  ByteConversion::toBytes(raw, data, 2,
    EndiannessTypes::big, SignednessTypes::nosign);
}
  
void MdTileId::read(BlackT::TStream& ifs) {
  fromRawValue(ifs.readu16be());
}

void MdTileId::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(toRawValue());
}
  
void MdTileId::fromRawValue(int raw) {
  priority = (raw & 0x8000) != 0;
  palette = (raw & 0x6000) >> 13;
  vflip = (raw & 0x1000) != 0;
  hflip = (raw & 0x0800) != 0;
  pattern = (raw & 0x07FF);
}

int MdTileId::toRawValue() const {
  int raw = 0;
  if (priority) raw |= 0x8000;
  raw |= (palette << 13);
  if (vflip) raw |= 0x1000;
  if (hflip) raw |= 0x0800;
  raw |= pattern;
  
  return raw;
}
  
void MdTileId::toColorGraphic(TGraphic& dst,
                    const MdVram& vram,
                    const MdPalette& pal,
                    int x, int y) const {
  const MdPattern& pat = vram.getPattern(pattern);
  
  TGraphic temp(MdPattern::w, MdPattern::h);
  pat.toColorGraphic(temp, pal.lines[palette], 0, 0);
  
  if (vflip) temp.flipVertical();
  if (hflip) temp.flipHorizontal();
  
  dst.copy(temp,
           TRect(x, y, 0, 0),
           TRect(0, 0, 0, 0));
}
  
void MdTileId::toGrayscaleGraphic(TGraphic& dst,
                    const MdVram& vram,
                    int x, int y) const {
  const MdPattern& pat = vram.getPattern(pattern);
  
  TGraphic temp(MdPattern::w, MdPattern::h);
  temp.clearTransparent();
  pat.toGrayscaleGraphic(temp, 0, 0);
  
  if (vflip) temp.flipVertical();
  if (hflip) temp.flipHorizontal();
  
  dst.copy(temp,
           TRect(x, y, 0, 0),
           TRect(0, 0, 0, 0));
}


}
