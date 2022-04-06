#include "ymm/YmmCompress.h"
#include <iostream>

using namespace BlackT;

namespace Md {


void YmmCompress::decmpYmmRleTypes(
    BlackT::TStream& src, BlackT::TStream& dst) {
  // If first byte is 0x80, use algorithm 2
  if ((unsigned char)src.peek() == 0x80) {
    src.get();
    decmpYmm4(src, dst);
  }
  else decmpYmmFillRle8(src, dst);
}

void YmmCompress::cmpYmmRleTypes(BlackT::TStream& src, BlackT::TStream& dst) {
  // compress data using both methods and use whichever is more efficient
  
  TBufStream rle8(0x100000);
  TBufStream ymm4(0x100000);
  
  src.clear();
  src.seek(0);
  
  cmpYmmFillRle8(src, rle8);
  
  src.clear();
  src.seek(0);
  
  cmpYmm4(src, ymm4);
  
  if (rle8.size() < ymm4.size() + 1) {
    dst.write((char*)rle8.data().data(), rle8.size());
  }
  else {
    dst.put((char)0x80);
    dst.write((char*)ymm4.data().data(), ymm4.size());
  }
}
  
void YmmCompress::decmpYmmFillRle8(
    BlackT::TStream& src, BlackT::TStream& dst) {
  bool done = false;
  while (!done) {
    // Read command byte
    char cmdByte = src.get();
    
    char cmd = (cmdByte & 0xC0);
    int count = (cmdByte & 0x3F) + 1;
    
    switch ((unsigned char)cmd) {
    case 0x00:  // Fill with 0x00
      for (int i = 0; i < count; i++) dst.put(0x00);
      break;
    case 0x40:  // Fill with 0xFF
      for (int i = 0; i < count; i++) dst.put(0xFF);
      break;
    case 0x80:  // Fill with next input byte (repeat run)
    {
      // Raw count of zero (we added 1 earlier) terminates input
      if (count == 1) {
        done = true;
        break;
      }
      
      char value = src.get();
      for (int i = 0; i < count; i++) dst.put(value);
    }
      break;
    case 0xC0:  // Copy bytes from input (absolute run)
      for (int i = 0; i < count; i++) dst.put(src.get());
      break;
    default:
      std::cerr << "impossible" << std::endl;
      break;
    }
  }
}

void YmmCompress::cmpYmmFillRle8(BlackT::TStream& src, BlackT::TStream& dst) {
  BlackT::TBufStream absoluteBuffer(Rle8MaxRunLen);
  
  while (true) {
    // finish up if at end of input
    if (src.eof()) {
      // flush any pending absolute command
      Rle8FlushBuffer(absoluteBuffer, dst);
      // terminate input with a repeat run with a raw count of 0 (no value
      // necessary)
      dst.put(0x80);
      break;
    }
    
    // get next character
    char next = src.get();
    
    // count number of times that character is repeated (up to max fill
    // length)
    int repCount = countByteRepeats(next, src) + 1;
//    std::cerr << std::hex << (int)(unsigned char)next
//      << " " << repCount << std::endl;
//    char c; std::cin >> c;
    
    // if character is 00 or FF and repeatcount >= 2, flush absolute and
    // add short fill
    if ((repCount >= Rle8MinShortFillLen) && (next == 0x00)) {
      Rle8FlushBuffer(absoluteBuffer, dst);
      dst.put((char)(0x00 | (repCount - 1)));
    }
    else if ((repCount >= Rle8MinShortFillLen) && (next == 0xFF)) {
      Rle8FlushBuffer(absoluteBuffer, dst);
      dst.put((char)(0x40 | (repCount - 1)));
    }
    // if repeatcount >= 3, flush absolute and add long fill
    else if ((repCount >= Rle8MinLongFillLen)) {
      Rle8FlushBuffer(absoluteBuffer, dst);
      dst.put((char)(0x80 | (repCount - 1)));
      dst.put(next);
    }
    // add character(s) to absolute buffer
    else {
      for (int i = 0; i < repCount; i++) {
        absoluteBuffer.put(next);
  
        // flush if full
        if ((absoluteBuffer.tell() >= Rle8MaxRunLen)) {
          Rle8FlushBuffer(absoluteBuffer, dst);
        }
      }
    }
  }
}
  
void YmmCompress::Rle8FlushBuffer(BlackT::TBufStream& absoluteBuffer,
                                  BlackT::TStream& dst) {
  // do nothing if buffer empty
  if (absoluteBuffer.tell() <= 0) return;
  
  int sz = absoluteBuffer.tell();
  dst.put((char)(0xC0 | (sz - 1)));
  absoluteBuffer.seek(0);
  absoluteBuffer.writeTo(dst, sz);
  absoluteBuffer.seek(0);
}

int YmmCompress::countByteRepeats(char value,
                                  BlackT::TStream& src) {
  int count = 0;
  while (!(src.eof())
         && (count < Rle8MaxRunLen - 1)
         && (src.peek() == value)) {
    ++count;
    src.get();
  }
  return count;
}
  
void YmmCompress::decmpYmm4(BlackT::TStream& src, BlackT::TStream& dst) {
  bool srcNybLow = true;
  bool dstNybLow = true;
  char srcChar = 0;
  char dstChar = 0;
  
  // Read 32-bit decompressed size (bytes)
// correct size is c160, on 5c00
//  int decmpSize = src.readu32be() * 2 + 0x20 * 75;
  int decmpSize = src.readu32be();
  
  int startPos = dst.tell();
  
  bool done = false;
  while (!done) {
    
    // Read fill nybble
    char fillNyb = fetchNextNybble(&srcChar, &srcNybLow, src);
    
    // Read command nybble
    char cmdNyb = fetchNextNybble(&srcChar, &srcNybLow, src);
    
    int fillLen;
    if ((cmdNyb & 0x08) != 0) {
      // If top bit of command nybble set, use 8-bit mode
      char subNyb = fetchNextNybble(&srcChar, &srcNybLow, src);
      unsigned char fullVal = ((cmdNyb & 0x7) << 4) | subNyb;
      fillLen = (unsigned int)fullVal + 9;
    } else {
      // If top bit of command nybble not set, use 4-bit mode
      fillLen = cmdNyb + 1;
    }
    
    for (int i = 0; i < fillLen; i++) {
      setNextNybble(fillNyb, &dstChar, &dstNybLow, dst);
      
      // Stop as soon as we reach the target number of bytes
      if (dst.tell() - startPos >= decmpSize) {
        done = true;
        break;
      }
    }
  }
  
}

void YmmCompress::cmpYmm4(BlackT::TStream& src, BlackT::TStream& dst) {
  dst.writeu32be(src.size());
  
  bool srcNybLow = true;
  bool dstNybLow = true;
  char srcChar = 0;
  char dstChar = 0;
  
  while (true) {
    // if src is at eof, _and there are no more nybbles pending_,
    // we're done
    if (srcNybLow && src.eof()) break;
  
    char next = fetchNextNybble(&srcChar, &srcNybLow, src);
    
    // count nybble repeats up to max 8-bit length
    int count = countNybbleRepeats(next, src, Ymm4MaxLongRunLen - 1,
                                    &srcChar, &srcNybLow) + 1;
    int writeCount = count - 1;
                                   
    // write fill nybble
    setNextNybble(next, &dstChar, &dstNybLow, dst);
    
    // if count >= min 8-bit length, use 8-bit mode
    if (count >= Ymm4MinLongRunLen) {
      writeCount -= 8;
      setNextNybble((char)(0x08 | ((writeCount & 0x70) >> 4)),
        &dstChar, &dstNybLow, dst);
      setNextNybble((char)(((writeCount & 0x0F))),
        &dstChar, &dstNybLow, dst);
    }
    // otherwise, use 4-bit mode
    else {
      setNextNybble((char)(((writeCount & 0x07))),
        &dstChar, &dstNybLow, dst);
    }
  }
  
  // flush last character if final nybble not written
  if (!dstNybLow) {
    dst.put(dstChar);
  }
}

int YmmCompress::countNybbleRepeats(char value,
                            BlackT::TStream& src,
                            int maxLen,
                            char* srcChar,
                            bool* srcNybLow) {
  int count = 0;
  while (!(*srcNybLow && src.eof())
         && (count < maxLen)
         && (peekNextNybble(srcChar, srcNybLow, src) == value)) {
    ++count;
    fetchNextNybble(srcChar, srcNybLow, src);
  }
  return count;
}
  
void YmmCompress::decmpYmmTilemap(BlackT::TStream& src, BlackT::TStream& dst) {
//  std::cerr << std::hex << src.tell() << std::endl;
  int prevWord = 0;
  while (true) {
    int input = src.readu16be();
    if (input == 0xFFFF) break;
    
    int command = input & 0xC000;
    
    switch (command) {
    case 0x0000:  // copy input word to output
      dst.writeu16be(input);
      break;
    case 0x4000:  // abbreviated fill using value from last 0x8000 command
    {
      // Count is given by low byte of input
      int count = (input & 0x00FF) + 1;
      
      for (int i = 0; i < count; i++) {
        dst.writeu16be(prevWord);
      }
    }
      break;
    case 0x8000:
    case 0xC000:
    {
      // read another byte to get the repeat count
      unsigned char nextByte = src.readu8be();
      int count = nextByte + 1;
      int value = input & 0x3FFF;
      
      if (command == 0x8000) {
        // fill with input word
        for (int i = 0; i < count; i++) dst.writeu16be(value);
        // update prevWord for 0x4000 commands
        prevWord = value;
      } else {
        // incremental fill with input word: increment value after each
        // write
        for (int i = 0; i < count; i++) {
          dst.writeu16be(value);
          ++value;
        }
      }
    }
      break;
    default:
      std::cerr << "impossible" << std::endl;
      break;
    }
  }
  
}

void YmmCompress::cmpYmmTilemap(BlackT::TStream& src, BlackT::TStream& dst) {
  
  int prevWord = 0xFFFF;
  while (true) {
    if (src.eof()) {
      break;
    }
  
    int next = src.readu16be();
    int count = countWordRepeats(next, src, YmmTilemapMaxRunLen - 1) + 1;
    
//    std::cerr << std::hex << next << " " << std::dec << count << std::endl;
//    char c; std::cin >> c;
    
    if (count == 1) {
      // check for incremental fill
      int incCount = countWordIncrements(next, src, YmmTilemapMaxRunLen - 1)
        + 1;
      
      // incremental fill
      if (incCount >= YmmTilemapMinRunLen) {
        dst.writeu16be(0xC000 | next);
        dst.writeu8be(incCount - 1);
      }
      // single-word write
      else {
        dst.writeu16be(0x0000 | next);
      }
    }
    else if (next == prevWord) {
      // abbreviated fill with previous word
      dst.writeu16be(0x4000 | (count - 1));
    }
    else {
      // full-length fill
      dst.writeu16be(0x8000 | next);
      dst.writeu8be(count - 1);
      // remember most recent word for abbreviated commands
      prevWord = next;
    }
  }
  
  dst.writeu16be(0xFFFF);
}

int YmmCompress::countWordRepeats(int value,
                                  BlackT::TStream& src,
                                  int maxLen) {
  int count = 0;
  int next;
  while (!src.eof() && (count < maxLen)) {
    next = src.readu16be();
    if (next != value) {
      src.clear();
      src.seekoff(-2);
      break;
    }
    
    // ???
    // is this more fucking stupid shit with how eof works in filestreams?
    // because it's pretty fucking useless that i have to read past the
    // end of the file to detect if i've reached the end of it
//    if (!src.eof()) ++count;
    ++count;
  }
  
  return count;
}

int YmmCompress::countWordIncrements(int value,
                                     BlackT::TStream& src,
                                     int maxLen) {
  int count = 0;
  int next;
  while (!src.eof() && (count < maxLen)) {
    next = src.readu16be();
    if (next != value + 1) {
      src.clear();
      src.seekoff(-2);
      break;
    }
    ++value;
//    ++count;
    
    // ???
//    if (!src.eof()) ++count;
    ++count;
  }
  
  return count;
}
  
inline char YmmCompress::fetchNextNybble(char* srcChar, bool* srcNybLow,
                            BlackT::TStream& src) {
  char nextNyb;
  
  // if next nybble is low, get it from the current byte
  if ((*srcNybLow = !(*srcNybLow))) nextNyb = *srcChar & 0x0F;
  else {
    // if next nybble is high, fetch a new source byte
    *srcChar = src.get();
    nextNyb = (*srcChar & 0xF0) >> 4;
  }
  
  return nextNyb;
}

inline char YmmCompress::peekNextNybble(char* srcChar, bool* srcNybLow,
                            BlackT::TStream& src) {
  char nextNyb;
  
  // if next nybble is low, get it from the current byte
  if (!(*srcNybLow)) nextNyb = *srcChar & 0x0F;
  else {
    // if next nybble is high, peek at next source byte
    nextNyb = (src.peek() & 0xF0) >> 4;
  }
  
  return nextNyb;
}

inline void YmmCompress::setNextNybble(char nyb,
                            char* dstChar, bool* dstNybLow,
                            BlackT::TStream& dst) {
  // If next nybble is low, write to low position and place
  // byte in stream
  if ((*dstNybLow = !(*dstNybLow))) {
    *dstChar |= nyb;
    dst.put(*dstChar);
    // Reset dst byte
    *dstChar = 0;
  }
  // If next nybble is high, write to high position
  else *dstChar |= (nyb << 4);
}

/* inline int YmmCompress::countNybbleRepeats(char nyb,
                            char* srcChar, bool* srcNybLow,
                            BlackT::TStream& src,
                            int srcEndPos) {
  // Only count up to the maximum repeat length (minus one; the implicit
  // initial occurence of the nybble counts towards the total)
  for (int i = 0; i < maxRepeatRun - 1; i++) {
    // Don't read past end of input
    if ((src.tell() >= srcEndPos)
        && (*srcNybLow)) return i;
    
    char nextNyb = peekNextNybble(srcChar, srcNybLow, src);
    
    if (nextNyb != nyb) return i;
    // Only fetch the nybble if it's not the last one; we need to leave
    // the terminating nybble in the stream for the next iteration of the
    // compression loop to find
    else fetchNextNybble(srcChar, srcNybLow, src);
  }
  
  return maxRepeatRun - 1;
} */


}
