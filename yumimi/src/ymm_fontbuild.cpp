#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include "util/TFileManip.h"
#include "util/TStringConversion.h"
#include "util/TGraphic.h"
#include "util/TPngConversion.h"
#include "util/TOpt.h"
#include "util/TArray.h"
#include "util/TByte.h"
#include "util/TFileManip.h"
#include "util/TBitmapFont.h"
#include "util/TThingyTable.h"
#include <iostream>
#include <string>

using namespace std;
using namespace BlackT;
//using namespace Md;

void setPixelIfExists(TGraphic& grp, int x, int y, TColor color) {
  if ((x >= grp.w()) || (x < 0) || (y >= grp.h()) || y < 0) return;
  grp.setPixel(x, y, color);
}

void outlinePixelCross(TGraphic& grp, int i, int j,
                       TColor emptyColor,
                       TColor color,
                       int radius) {
  bool fullTrans = false;
  if (emptyColor.a() == TColor::fullAlphaTransparency) fullTrans = true;
  for (int b = -radius; b <= radius; b++) {
    for (int a = -radius; a <= radius; a++) {
      if ((std::abs(a) == radius) && (std::abs(b) == radius)) continue;
      if ((a == 0) && (b == 0)) continue;
      
      if (fullTrans) {
        if (grp.getPixel(i + a, j + b).a() == TColor::fullAlphaTransparency) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
      else {
        if (grp.getPixel(i + a, j + b) == emptyColor) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
    }
  }
}

void outlinePixelSquare(TGraphic& grp, int i, int j,
                       TColor emptyColor,
                       TColor color,
                       int radius) {
  bool fullTrans = false;
  if (emptyColor.a() == TColor::fullAlphaTransparency) fullTrans = true;
  for (int b = -radius; b <= radius; b++) {
    for (int a = -radius; a <= radius; a++) {
      if ((a == 0) && (b == 0)) continue;
      
      if (fullTrans) {
        if (grp.getPixel(i + a, j + b).a() == TColor::fullAlphaTransparency) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
      else {
        if (grp.getPixel(i + a, j + b) == emptyColor) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
    }
  }
}

void outlinePixelDownRightDiag(TGraphic& grp, int i, int j,
                       TColor emptyColor,
                       TColor color,
                       int radius) {
  bool fullTrans = false;
  if (emptyColor.a() == TColor::fullAlphaTransparency) fullTrans = true;
  for (int b = 0; b <= radius; b++) {
    for (int a = 0; a <= radius; a++) {
      if ((a == 0) && (b == 0)) continue;
      
      if (fullTrans) {
        if (grp.getPixel(i + a, j + b).a() == TColor::fullAlphaTransparency) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
      else {
        if (grp.getPixel(i + a, j + b) == emptyColor) {
          setPixelIfExists(grp, i + a, j + b, color);
        }
      }
    }
  }
}

void outlineGrp(TGraphic& grp) {
  for (int j = 0; j < grp.h(); j++) {
    for (int i = 0; i < grp.w(); i++) {
      TColor color = grp.getPixel(i, j);
      if (color == TColor(255, 255, 255, TColor::fullAlphaOpacity)) {
//      if (color.a() != TColor::fullAlphaTransparency) {
//        cout << "(" << i << "," << j << ") ";
//        cout << "X";
        outlinePixelSquare(grp, i, j,
                          TColor(255, 255, 255, TColor::fullAlphaTransparency),
                          TColor(0x11, 0x11, 0x11),
                          1);
        // drop shadow
/*        outlinePixelDownRightDiag(grp, i, j,
                          TColor(255, 255, 255, TColor::fullAlphaTransparency),
                          TColor(0x22, 0x22, 0x22),
                          2);
        outlinePixelCross(grp, i, j,
                          TColor(255, 255, 255, TColor::fullAlphaTransparency),
                          TColor(0x11, 0x11, 0x11),
                          2);
        outlinePixelCross(grp, i, j,
                          TColor(0x11, 0x11, 0x11),
                          TColor(0x33, 0x33, 0x33),
                          1);*/
      }
      else {
//        cout << " ";
      }
    }
//    cout << endl;
  }
  
  // drop shadow
  for (int j = 0; j < grp.h(); j++) {
    for (int i = 0; i < grp.w(); i++) {
      TColor color = grp.getPixel(i, j);
      if (color == TColor(255, 255, 255, TColor::fullAlphaOpacity)) {
        outlinePixelDownRightDiag(grp, i, j,
                          TColor(255, 255, 255, TColor::fullAlphaTransparency),
                          TColor(0x22, 0x22, 0x22),
                          2);
      }
    }
  }
  
//  TPngConversion::graphicToRGBAPng("test.png", grp);
//  char c;
//  cin >> c;
}

bool rightShiftGrp(TGraphic& grp, TColor emptyColor) {
  bool isAllTransparent = true;
  for (int j = 0; j < grp.h(); j++) {
    for (int i = grp.w() - 2; i >= 0; i--) {
      TColor col = grp.getPixel(i, j);
      
      if (col.a() != TColor::fullAlphaTransparency) isAllTransparent = false;
      
      grp.setPixel(i, j, emptyColor);
      grp.setPixel(i + 1, j, col);
    }
  }
  
  return isAllTransparent;
}

void outlineFont(TBitmapFont& font) {
  // add outline to all chars
  for (int i = 0; i < font.numFontChars(); i++) {
    TBitmapFontChar& fontChar = font.fontChar(i);
//    outlineFontChar(fontChar);

    bool isAllTransparent = rightShiftGrp(fontChar.grp,
      TColor(0, 0, 0, TColor::fullAlphaTransparency));
    // don't do anything to fully transparent characters
    // (i.e. space characters or empty slots)
    // FIXME: will miss characters whose rightmost column alone
    // contains nonempty data.
    // will not actually occur in practice.
    if (isAllTransparent) continue;
    
    outlineGrp(fontChar.grp);
    
    // character data is 3 pixels wider from outline
    fontChar.glyphWidth += 3;
    // account for outline in advance width,
    // but remove 2 pixels:
    // one to get rid of the automatic pixel of spacing between chars,
    // another so the outlines will overlap each other instead of
    // being adjacent
//    fontChar.advanceWidth += 1;
    // screw it, never mind, with the baked-in drop shadow
    // i'm handling this through kerning instead
    fontChar.advanceWidth += 2;
  }
}

const static int charW = 16;
const static int charH = 14;

int getColComponent(TColor col) {
  return (int)((unsigned char)col.r()) / 16;
}

void exportFontCharBitmap(TBitmapFontChar& fontChar, TStream& ofs) {
  for (int j = 0; j < charH; j++) {
    for (int i = 0; i < charW; i += 4) {
      int output = 0;
      output |= (getColComponent(fontChar.grp.getPixel(i + 0, j)) << 12);
      output |= (getColComponent(fontChar.grp.getPixel(i + 1, j)) << 8);
      output |= (getColComponent(fontChar.grp.getPixel(i + 2, j)) << 4);
      output |= (getColComponent(fontChar.grp.getPixel(i + 3, j)));
      ofs.writeu16be(output);
    }
  }
}

void exportFontBitmap(TBitmapFont& font, TStream& ofs) {
  for (int i = 0; i < font.numFontChars(); i++) {
    TBitmapFontChar& fontChar = font.fontChar(i);
    exportFontCharBitmap(fontChar, ofs);
  }
}

void exportFontWidthTable(TBitmapFont& font, TStream& ofs) {
  for (int i = 0; i < font.numFontChars(); i++) {
    TBitmapFontChar& fontChar = font.fontChar(i);
    ofs.writeu8(fontChar.advanceWidth);
  }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix font builder" << endl;
    cout << "Usage: " << argv[0]
      << " <fontprefix> <outprefix> <outwtable> <outkerndata>"
      << endl;
    
    return 0;
  }
  
  string fontPrefix(argv[1]);
  string outPrefix(argv[2]);
  
  string outBitmap = outPrefix + "_bitmap.bin";
  string outWidthTable = outPrefix + "_width.bin";
  string outKerningData = outPrefix + "_kern.bin";
  
  TBitmapFont font;
  font.load(fontPrefix);
  
  outlineFont(font);
  
  {
    TBufStream ofs;
    exportFontBitmap(font, ofs);
    ofs.save(outBitmap.c_str());
  }
  
  {
    TBufStream ofs;
    exportFontWidthTable(font, ofs);
    ofs.save(outWidthTable.c_str());
  }
  
  // kerning
  // TODO: generate something more compact than the raw matrix?
  {
    TBufStream ofs;
    font.exportKerningMatrix(ofs);
    ofs.save(outKerningData.c_str());
  }
  
/*  TBufStream ifs;
  {
    TBufStream filterIfs;
    filterIfs.open(stringfile);
    while (!filterIfs.eof()) {
      if ((filterIfs.peek() == '\r')
          || (filterIfs.peek() == '\n')) {
        break;
      }
      
      ifs.put(filterIfs.get());
    }
  }
  ifs.seek(0);
  
  TThingyTable table;
  table.readSjis(tablefile);
  
  TGraphic grp;
  font.render(grp, ifs, table);
  
  TPngConversion::graphicToRGBAPng(std::string(outfile), grp); */
  
  return 0;
}
