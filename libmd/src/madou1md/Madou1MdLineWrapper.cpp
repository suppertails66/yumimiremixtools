#include "madou1md/Madou1MdLineWrapper.h"
#include "util/TParse.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iostream>

using namespace BlackT;

namespace Md {

/*const static int controlOpsStart = 0xC0;
const static int controlOpsEnd   = 0x100;

const static int op_space   = 0x20;
//const static int op_clear   = 0xFD;
const static int op_rightbox = 0xC0;
const static int op_leftbox = 0xC1;
const static int op_bottombox = 0xC2;
const static int op_wait    = 0xC4;
const static int op_br      = 0xC3;
const static int op_end     = 0xFF;

// added for translation
const static int op_tilebr  = 0x1F; */

const static int op_space   = 0x004A;
const static int op_space1px   = 0x0070;

const static int op_end        = 0xFF04;
const static int op_delay      = 0xFF0C;
const static int op_leftbox1    = 0xFF10;
const static int op_rightbox1   = 0xFF14;
const static int op_leftbox2    = 0xFF18;
const static int op_rightbox2   = 0xFF1C;
const static int op_centerbox28   = 0xFF28;
const static int op_centerbox2C   = 0xFF2C;
const static int op_br         = 0xFF30;
const static int op_wait       = 0xFF34;
const static int op_autoend    = 0xFF38;
const static int op_buffer1char    = 0xFF44;
const static int op_buffer2char    = 0xFF48;
const static int op_buffer5char    = 0xFF4C;
const static int op_voice50       = 0xFF50;
const static int op_arleanim       = 0xFF54;
const static int op_anim       = 0xFF58;
const static int op_op5C       = 0xFF5C;
const static int op_openleftbox   = 0xFF60;
const static int op_closeleftbox  = 0xFF64;
const static int op_openrightbox  = 0xFF68;
const static int op_closerightbox = 0xFF6C;
const static int op_prompt70      = 0xFF70;
const static int op_prompt74      = 0xFF74;
const static int op_buffer7char    = 0xFF78;
const static int op_prompt7C      = 0xFF7C;
const static int op_maplabel   = 0xFF80;
const static int op_face       = 0xFF84;
const static int op_op88       = 0xFF88;
const static int op_op8C       = 0xFF8C;
const static int op_op90       = 0xFF90;
const static int op_op94       = 0xFF94;
const static int op_op98       = 0xFF98;
const static int op_prompt9C       = 0xFF9C;
const static int op_promptA0       = 0xFFA0;
const static int op_promptA4       = 0xFFA4;
const static int op_promptA8       = 0xFFA8;
const static int op_opAC       = 0xFFAC;
const static int op_resetface       = 0xFFB0;
const static int op_opB4       = 0xFFB4;
const static int op_clear      = 0xFFB8;
const static int op_clearAll   = 0xFFBC;
const static int op_opC0_end   = 0xFFC0;
const static int op_voiceC4       = 0xFFC4;
const static int op_opC8       = 0xFFC8;
const static int op_opCC       = 0xFFCC;
const static int op_opD4       = 0xFFD4;
const static int op_opD8       = 0xFFD8;
const static int op_opDC       = 0xFFDC;
const static int op_opE0       = 0xFFE0;
const static int op_opE4       = 0xFFE4;
const static int op_opE8       = 0xFFE8;
const static int op_opEC       = 0xFFEC;
const static int op_opF0       = 0xFFF0;
const static int op_terminator = 0xFFFF;

// new for hack
const static int op_clearcenter       = 0xFFFC;

Madou1MdLineWrapper::Madou1MdLineWrapper(BlackT::TStream& src__,
                ResultCollection& dst__,
                const BlackT::TThingyTable& thingy__,
                CharSizeTable sizeTable__,
                KerningMatrix kerningMatrix__,
                int xSize__,
                int ySize__)
  : TLineWrapper(src__, dst__, thingy__, xSize__, ySize__),
    sizeTable(sizeTable__),
    kerningMatrix(kerningMatrix__),
    lastNonSpaceChar(-1),
    lastChar(-1),
    firstCharKerning(0),
    xBeforeWait(-1),
//    clearMode(clearMode_default),
    breakMode(breakMode_normal) {
  
}

/*int Madou1MdLineWrapper::widthOfKey(int key) {
  if ((key == op_br)) return 0;
  else if ((key == op_end)) return 0;
  else if ((key == op_tilebr)) return 8;  // assume worst case
  else if ((key >= controlOpsStart) && (key < controlOpsEnd)) return 0;
  
  return sizeTable[key];
} */

int Madou1MdLineWrapper::widthOfKey(int key) {
  return (glyphWidthOfKey(key) + advanceWidthOfKey(key));
}

int Madou1MdLineWrapper::glyphWidthOfKey(int key) {
  // control ops
  if (key >= 0xFF00) {
    // assume worst case for dynamic content
    
    // these are always(?) used for numbers
    if (key == op_buffer1char)      return (11 * 1);
    else if (key == op_buffer2char) return (11 * 2);
    else if (key == op_buffer5char) return (11 * 5);
    // whereas this is always(?) used for item/monster names
    else if (key == op_buffer7char) return (16 * 7);
    else return 0;
  }
  
  return sizeTable.at(key).glyphWidth;
}

int Madou1MdLineWrapper::advanceWidthOfKey(int key) {
  // control ops
  if (key >= 0xFF00) {
//    return 0;
    return glyphWidthOfKey(key);
  }
  
  int advanceWidth = sizeTable.at(key).advanceWidth;
  
  return advanceWidth;
  
/*  if (isWordDivider(key)) return advanceWidth;
  
  // kerning
  int kerning = 0;
  if (lastNonSpaceChar != -1) {
    kerning = kerningMatrix.data(key, lastNonSpaceChar);
  }
  
//  std::cerr << "xPos: " << xPos << " char1: " << lastNonSpaceChar << " char2: " << key << " " << "kerning: " << kerning << " currentWordWidth: " << currentWordWidth << " maxCurrentWordWidth: " << maxCurrentWordWidth << std::endl;
  
//  if (maxCurrentWordWidth == 0) firstCharKerning = kerning;
  
  return advanceWidth + kerning; */
}

bool Madou1MdLineWrapper::isWordDivider(int key) {
  if (
      (key == op_br)
      || (key == op_space)
     ) return true;
  
  return false;
}

bool Madou1MdLineWrapper::isLinebreak(int key) {
  if (
      (key == op_br)
      ) return true;
  
  return false;
}

bool Madou1MdLineWrapper::isBoxClear(int key) {
  // END
  if ((key == op_end)
//      || (key == op_clear)
      || (key == op_clear)
      || (key == op_clearAll)
      || (key == op_wait)
      || (key == op_terminator)
      || (key == op_rightbox1)
      || (key == op_leftbox1)
      || (key == op_rightbox2)
      || (key == op_leftbox2)
      || (key == op_centerbox28)
      || (key == op_centerbox2C)
      || (key == op_autoend)
      || (key == op_opB4)
      || (key == op_opC0_end)
      
      // new for hack
      || (key == op_clearcenter)
      ) return true;
  return false;
}

void Madou1MdLineWrapper::onBoxFull() {
/*  if (clearMode == clearMode_default) {
    std::string content;
    if (lineHasContent) {
      // wait
      content += thingy.getEntry(op_wait);
      content += thingy.getEntry(op_br);
      currentScriptBuffer.write(content.c_str(), content.size());
    }
    // linebreak
    stripCurrentPreDividers();
    
    currentScriptBuffer.put('\n');
    xPos = 0;
    yPos = 0;
  }
  else if (clearMode == clearMode_messageSplit) {
    std::string content;
//      if (lineHasContent) {
      // wait
//        content += thingy.getEntry(op_wait);
//        content += thingy.getEntry(op_br);
      content += thingy.getEntry(op_end);
      content += "\n\n#ENDMSG()\n\n";
      currentScriptBuffer.write(content.c_str(), content.size());
//      }
    // linebreak
    stripCurrentPreDividers();
    
    xPos = 0;
    yPos = 0;
  } */
  
  std::string content;
  if (lineHasContent) {
    // wait
    content += thingy.getEntry(op_wait);
//    content += thingy.getEntry(op_br);
//    content += linebreakString();
    currentScriptBuffer.write(content.c_str(), content.size());
  }
  // linebreak
  stripCurrentPreDividers();
  
  currentScriptBuffer.put('\n');
  xPos = 0;
  yPos = 0;
  lastChar = -1;
  lastNonSpaceChar = -1;
  firstCharKerning = 0;

/*  std::cerr << "WARNING: line " << lineNum << ":" << std::endl;
  std::cerr << "  overflow at: " << std::endl;
  std::cerr << streamAsString(currentScriptBuffer)
    << std::endl
    << streamAsString(currentWordBuffer) << std::endl; */
}

//int Madou1MdLineWrapper::linebreakKey() {
//  return op_br;
//}

std::string Madou1MdLineWrapper::linebreakString() const {
  std::string breakString = thingy.getEntry(op_br);
  if (breakMode == breakMode_normal) {
    return breakString;
  }
  else {
//    return breakString + breakString;
    return breakString + thingy.getEntry(op_space1px);
  }
}

//int Madou1MdLineWrapper::linebreakHeight() const {
//  if (breakMode == breakMode_normal) {
//    return 1;
//  }
//  else {
//    return 2;
//  }
//}

void Madou1MdLineWrapper::onSymbolAdded(BlackT::TStream& ifs, int key) {
/*  if (isLinebreak(key)) {
    if ((yPos != -1) && (yPos >= ySize - 1)) {
      flushActiveWord();
      
    }
  } */
  
  if (key >= 0xFF00) return;
  
  // apply kerning to pending advance width
  if (lastNonSpaceChar != -1) {
    int kerning = kerningMatrix.data(key, lastNonSpaceChar);
    pendingAdvanceWidth += kerning;
  }
}

void Madou1MdLineWrapper::afterSymbolAdded(BlackT::TStream& ifs, int key) {
  if (key >= 0xFF00) {
    // control op
  }
  else {
    lastChar = key;
    if (key != op_space) { lastNonSpaceChar = key;  }
  }
}

void Madou1MdLineWrapper
    ::handleManualLinebreak(TLineWrapper::Symbol result, int key) {
  if ((key != op_br) || (breakMode == breakMode_normal)) {
    TLineWrapper::handleManualLinebreak(result, key);
  }
  else {
    outputLinebreak(linebreakString());
  }
}

void Madou1MdLineWrapper::beforeLinebreak(
    LinebreakSource clearSrc, int key) {
//  std::cerr << xPos << " " << currentWordWidth << " " << maxCurrentWordWidth << std::endl;
//  if (clearSrc == linebreakLineFull) {
//    std::cerr << "here" << std::endl;
//    currentWordWidth += -firstCharKerning;
//    maxCurrentWordWidth += -firstCharKerning;
//  }
//  std::cerr << xPos << " " << currentWordWidth << " " << maxCurrentWordWidth << std::endl;
}

void Madou1MdLineWrapper::afterLinebreak(
    LinebreakSource clearSrc, int key) {
/*  if (clearSrc != linebreakBoxEnd) {
    if (spkrOn) {
      xPos = spkrLineInitialX;
    }
  } */
  
/*  if (clearSrc == linebreakManual) {
    if (breakMode == breakMode_1pxoffset) {
      --yPos;
    }
  } */
  
  // if auto-linebreak occurs, kerning between last letter of previous word
  // and first letter of current word is now invalid.
  // recompute the length of the word buffer.
//  if (clearSrc == linebreakLineFull) {
//    std::cerr << "here" << std::endl;
//    currentWordWidth += -firstCharKerning;
//    maxCurrentWordWidth += -firstCharKerning;
    TBufStream buf = currentWordBuffer;
    buf.seek(0);
    currentWordWidth = 0;
    maxCurrentWordWidth = 0;
    int pendingAdv = 0;
    int lastChar = -1;
    while (!buf.eof()) {
      // skip literals
      if (buf.peek() == '<') {
        buf.get();
        if (buf.peek() == '$') {
          while (buf.get() != '>');
        }
        else {
          buf.unget();
        }
      }
      
      TThingyTable::MatchResult result = getSymbolId(buf);
      if (result.id != -1) {
        bool isOp = (result.id >= 0xFF00);
        
        int advanceWidth = advanceWidthOfKey(result.id);
        int glyphWidth = glyphWidthOfKey(result.id);
        
        int kerning = 0;
        if (!isOp && (lastChar != -1)) {
          kerning = kerningMatrix.data(result.id, lastChar);
        }
          
        currentWordWidth += pendingAdv;
        if (currentWordWidth > maxCurrentWordWidth)
          maxCurrentWordWidth = currentWordWidth;
        
        currentWordWidth += glyphWidth;
        if (currentWordWidth > maxCurrentWordWidth)
          maxCurrentWordWidth = currentWordWidth;
        
        pendingAdv = (advanceWidth - glyphWidth) + kerning;
        
        if (!isOp) lastChar = result.id;
      }
    }
    
    // in special offset linebreak mode, account for extra space
    if (breakMode == breakMode_1pxoffset) {
      ++currentWordWidth;
    }
    
    currentWordWidth += pendingAdv;
    if (currentWordWidth > maxCurrentWordWidth)
      maxCurrentWordWidth = currentWordWidth;
      
//      std::cerr << "new widths: " << currentWordWidth << " " << maxCurrentWordWidth << std::endl;
//  }
  
//  std::cerr << "here2" << std::endl;
  lastNonSpaceChar = -1;
  lastChar = -1;
  firstCharKerning = 0;
}

void Madou1MdLineWrapper::beforeBoxClear(
    BoxClearSource clearSrc, int key) {
//  if (((clearSrc == boxClearManual) && (key == op_wait))) {
//    xBeforeWait = xPos;
//  }
}

void Madou1MdLineWrapper::afterBoxClear(
  BoxClearSource clearSrc, int key) {
  // wait pauses but does not automatically break the line
  if (((clearSrc == boxClearManual) && (key == op_wait))) {
//    xPos = xBeforeWait;
    yPos = 0;
/*    if (breakMode == breakMode_normal) {
      yPos = -1;
    }
    else {
      yPos = -2;
    } */
  }
  
  lastNonSpaceChar = -1;
  lastChar = -1;
  firstCharKerning = 0;
}

bool Madou1MdLineWrapper::processUserDirective(BlackT::TStream& ifs) {
  TParse::skipSpace(ifs);
  
  std::string name = TParse::matchName(ifs);
  TParse::matchChar(ifs, '(');
  
  for (int i = 0; i < name.size(); i++) {
    name[i] = toupper(name[i]);
  }
  
  if (name.compare("SETBREAKMODE") == 0) {
    std::string type = TParse::matchName(ifs);
    
    if (type.compare("NORMAL") == 0) {
      breakMode = breakMode_normal;
    }
    else if (type.compare("OFFSET1PX") == 0) {
      breakMode = breakMode_1pxoffset;
    }
    else {
      throw TGenericException(T_SRCANDLINE,
                              "Madou1MdLineWrapper::processUserDirective()",
                              "Line "
                                + TStringConversion::intToString(lineNum)
                                + ": unknown break mode '"
                                + type
                                + "'");
    }
    
    return true;
  }
/*  else if (name.compare("PARABR") == 0) {
//    if (yPos >= ySize) {
//      onBoxFull();
//    }
//    else {
//      onBoxFull();
//    }
    flushActiveWord();
    outputLinebreak();
    return true;
  } */
//  else if (name.compare("ENDMSG") == 0) {
//    processEndMsg(ifs);
//    return true;
//  }
  
  return false;
}

}
