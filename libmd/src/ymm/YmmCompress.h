#ifndef YMM_COMPRESSH
#define YMM_COMPRESSH


#include "util/TStream.h"
#include "util/TBufStream.h"
#include "util/TByte.h"

namespace Md {


class YmmCompress {
public:
  
  static void decmpYmmRleTypes(BlackT::TStream& src, BlackT::TStream& dst);
  static void cmpYmmRleTypes(BlackT::TStream& src, BlackT::TStream& dst);
  
  static void decmpYmmFillRle8(BlackT::TStream& src, BlackT::TStream& dst);
  static void cmpYmmFillRle8(BlackT::TStream& src, BlackT::TStream& dst);
  
  static void decmpYmm4(BlackT::TStream& src, BlackT::TStream& dst);
  static void cmpYmm4(BlackT::TStream& src, BlackT::TStream& dst);
  
  static void decmpYmmTilemap(BlackT::TStream& src, BlackT::TStream& dst);
  static void cmpYmmTilemap(BlackT::TStream& src, BlackT::TStream& dst);
  
protected:
//  const static char terminator = 0;
//  const static int maxAbsoluteRun = 8;
//  const static int minRepeatRun = 4;
//  const static int maxRepeatRun = 10;

  const static int Rle8MaxRunLen = 0x40;
  const static int Rle8MinShortFillLen = 2;
  const static int Rle8MinLongFillLen = 3;
  
  const static int Ymm4MinLongRunLen = 0x9;
  const static int Ymm4MaxLongRunLen = 0x88;
  const static int Ymm4MinShortRunLen = 0x0;
  const static int Ymm4MaxShortRunLen = 0x8;
  
  const static int YmmTilemapMaxRunLen = 0x100;
  const static int YmmTilemapMinRunLen = 0x2;
  
  static void Rle8FlushBuffer(BlackT::TBufStream& absoluteBuffer,
                              BlackT::TStream& dst);
  static int countByteRepeats(char value,
                              BlackT::TStream& src);
  static int countNybbleRepeats(char value,
                              BlackT::TStream& src,
                              int maxLen,
                              char* srcChar,
                              bool* srcNybLow);
  static int countWordRepeats(int value,
                              BlackT::TStream& src,
                              int maxLen);
  static int countWordIncrements(int value,
                                 BlackT::TStream& src,
                                 int maxLen);

  static inline char fetchNextNybble(char* srcChar, bool* srcNybLow,
                              BlackT::TStream& src);

  static inline char peekNextNybble(char* srcChar, bool* srcNybLow,
                              BlackT::TStream& src);

  static inline void setNextNybble(char nyb,
                              char* dstChar, bool* dstNybLow,
                              BlackT::TStream& dst);
                              
  

/*  static inline int countNybbleRepeats(char nyb,
                              char* srcChar, bool* srcNybLow,
                              BlackT::TStream& src,
                              int srcEndPos); */
  
};


}


#endif
