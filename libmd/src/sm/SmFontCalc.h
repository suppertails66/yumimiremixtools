#ifndef SMFONTCALC_H
#define SMFONTCALC_H


#include "util/TCharFmt.h"
#include "util/TThingyTable.h"
#include "sm/SmFont.h"

namespace Md {


class SmFontCalc {
public:
  const static int yLimit = 2;

  static bool isSpace(BlackT::TChar32 checkChar);
  static bool isLinebreak(BlackT::TChar32 checkChar);
  static bool isControlCodeStart(BlackT::TChar32 checkChar);
  static bool isControlCodeEnd(BlackT::TChar32 checkChar);

  static int processString(const BlackT::TUtf32Chars& src,
                    BlackT::TUtf32Chars* dstP,
                    const SmFont& font,
                    const BlackT::TThingyTable& thingy,
                    int width);
  
  static int getWordLength(const BlackT::TUtf32Chars& src,
                    const SmFont& font,
                    const BlackT::TThingyTable& thingy);
protected:
  
  static void getNextWord(const BlackT::TUtf32Chars& src,
                          int* posP,
                          BlackT::TUtf32Chars* dstP);
  
  static void stripLeadingSpaces(const BlackT::TUtf32Chars& src,
                                 BlackT::TUtf32Chars& dst);
  
};


}


#endif
