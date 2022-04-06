#include "sm/SmFontCalc.h"
#include <iostream>

using namespace BlackT;

namespace Md {


bool SmFontCalc::isSpace(BlackT::TChar32 checkChar) {
  if ((checkChar == (unsigned char)' ')
      || (checkChar == (unsigned char)'\t')
      || (checkChar == (unsigned char)'\n')) {
    return true;
  }
  
  return false;
}

bool SmFontCalc::isLinebreak(BlackT::TChar32 checkChar) {
  if ((checkChar == (unsigned char)'\n')) {
    return true;
  }
  
  return false;
}

bool SmFontCalc::isControlCodeStart(BlackT::TChar32 checkChar) {
  if ((checkChar == (unsigned char)'[')) {
    return true;
  }
  
  return false;
}

bool SmFontCalc::isControlCodeEnd(BlackT::TChar32 checkChar) {
  if ((checkChar == (unsigned char)']')) {
    return true;
  }
  
  return false;
}

int SmFontCalc::processString(const BlackT::TUtf32Chars& src,
                  BlackT::TUtf32Chars* dstP,
                  const SmFont& font,
                  const BlackT::TThingyTable& thingy,
                  int width) {
  
  
  int x = 0;
  int y = 0;
  
  int pos = 0;
  
  TUtf32Chars& dst = *dstP;
  
  while (pos < src.size()) {
    
    TUtf32Chars nextWord;
    getNextWord(src, &pos, &nextWord);
    
/*    if (nextWord.size() > 0) {
      for (int i = 0; i < nextWord.size(); i++) {
        std::cerr << std::hex << nextWord[i] << " ";
      }
      std::cerr << std::endl;
    } */
    
    // handle linebreaks
    if ((nextWord.size() > 0)
        && (isLinebreak(nextWord[0]))) {
      x = 0;
      ++y;
      dst.push_back('\n');
    }
    else {
      int wordLength = getWordLength(nextWord, font, thingy);
      
      if (wordLength < 0) return -2;
      
      // check if this would overflow the boundaries and "wrap around" if so
      if (x + wordLength > width) {
        x = 0;
        ++y;
        dst.push_back('\n');
        
        // strip leading spaces from word and recalculate length
        TUtf32Chars nextWordOld = nextWord;
        nextWord.clear();
        stripLeadingSpaces(nextWordOld, nextWord);
        wordLength = getWordLength(nextWord, font, thingy);
      }
      
      for (TUtf32Chars::size_type i = 0; i < nextWord.size(); i++) {
        dst.push_back(nextWord[i]);
      }
      
      x += wordLength;
    }
    
    // if we overflowed the boundaries, error out
    if (y >= yLimit) {
//      std::cerr << "Warning: Script text overflow!" << std::endl;
      
      return -1;
    }
  }
  
  return 0;
}
  
void SmFontCalc::getNextWord(const BlackT::TUtf32Chars& src,
                        int* posP,
                        BlackT::TUtf32Chars* dstP) {
  int& pos = *posP;
  int startPos = pos;
  TUtf32Chars& dst = *dstP;
  
  bool pastInitialSpacing = false;
  
  while (pos < src.size()) {
    // if the very first character in the string is a linebreak, return it
    // to the caller immediately so the y-position can be updated
    if ((isLinebreak(src[pos]))
        && (pos == startPos)) {
      dst.push_back(src[pos++]);
      return;
      
    }
    // when we encounter a space, we're finished if we're past the initial
    // spacing
    else if (isSpace(src[pos])
             && pastInitialSpacing) {
      return;
    }
    // when we encounter a control code start opcode, skip past it
    else if (isControlCodeStart(src[pos])) {
/*      ++pos;
      while ((pos < src.size())
             && !isControlCodeEnd(src[pos])) ++pos;
      ++pos; */
      dst.push_back(src[pos++]);
      while ((pos < src.size())
             && !isControlCodeEnd(src[pos])) dst.push_back(src[pos++]);
      dst.push_back(src[pos++]);
    }
    // for non-special characters, mark that we've passed the initial spacing
    else {
      pastInitialSpacing = true;
      dst.push_back(src[pos++]);
    }
  }
  
  
}
  
int SmFontCalc::getWordLength(const BlackT::TUtf32Chars& src,
                    const SmFont& font,
                    const BlackT::TThingyTable& thingy) {
  int sz = 0;
  TUtf32Chars::size_type pos = 0;
  while (pos < src.size()) {
    // linebreaks shouldn't be in the input to begin with, but ignore them
    // anyway
    if (isLinebreak(src[pos])) {
      ++pos;
      continue;
    }
    
    // skip control codes
    if (isControlCodeStart(src[pos])) {
      ++pos;
      while ((pos < src.size())
             && !isControlCodeEnd(src[pos])) ++pos;
      ++pos;
      continue;
    }
    
    int charCode = thingy.getRevEntry(src[pos]);
    if (charCode == -1) return -1;
    if (charCode >= font.entries.size()) return -1;
    sz += font.entries[charCode].width;
    ++pos;
  }
  
  return sz;
}
  
void SmFontCalc::stripLeadingSpaces(const BlackT::TUtf32Chars& src,
                               BlackT::TUtf32Chars& dst) {
  TUtf32Chars::size_type pos = 0;
  while ((pos < src.size())
         && (isSpace(src[pos]))) ++pos;
  
  for ( ; pos < src.size(); pos++) {
    dst.push_back(src[pos]);
  }
}


}
