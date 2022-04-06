#include "sm/SmScript.h"
#include "sm/SmFontCalc.h"
#include "exception/TGenericException.h"
#include "util/TStringConversion.h"
#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cwchar>
#include <cwctype>

using namespace BlackT;

namespace Md {


SmScriptToken* SmScriptToken::instantiate(TokenType type) {
  switch (type) {
  case printChar:
    return new SmPrintCharToken();
    break;
  case endOfScript:
    return new SmEndOfScriptToken();
    break;
  case linebreak:
    return new SmLinebreakToken();
    break;
  case clearBox:
    return new SmClearBoxToken();
    break;
  case setPortrait:
    return new SmSetPortraitToken();
    break;
  case unknown1:
    return new SmUnknown1Token();
    break;
  case unknown2:
    return new SmUnknown2Token();
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "SmScriptToken::instantiate()",
                            std::string("Unknown token type: ")
                              + TStringConversion::intToString(type));
    break;
  }
}

SmScriptToken* SmScriptToken::clone(const SmScriptToken& token) {
  SmScriptToken* newToken = instantiate(token.type());
  *newToken = token;
  return newToken;
}
  
SmScriptToken* SmScriptToken::fetchToken(BlackT::TStream& ifs) {
  int next = ifs.readu16be();
  
  SmScriptToken* tokenP = NULL;
  
  switch (next) {
  case endOfScriptOp:
    tokenP = instantiate(endOfScript);
    break;
  case linebreakOp:
    tokenP = instantiate(linebreak);
    break;
  case clearBoxOp:
    tokenP = instantiate(clearBox);
    break;
  case setPortraitOp:
    tokenP = instantiate(setPortrait);
    break;
  case unknown1Op:
    tokenP = instantiate(unknown1);
    break;
  case unknown2Op:
    tokenP = instantiate(unknown2);
    break;
  default:
    // If top bit is set, this has to be a function call, so values other than
    // the above are illegal
    if (next >= 0x8000) {
      throw TGenericException(T_SRCANDLINE,
                              "SmScriptToken::fetchToken()",
                              std::string("Unrecognized opcode at ")
          + TStringConversion::intToString(ifs.tell() - 2,
                                           TStringConversion::baseHex)
          + ": "
          + TStringConversion::intToString(next,
                                           TStringConversion::baseHex));
    }
    
    // Otherwise, this is a character token
    tokenP = instantiate(printChar);
    
    break;
  }
  
  tokenP->read(next, ifs);
  
  return tokenP;
}
  
SmScriptToken::SmScriptToken() {
  
}

SmScriptToken::~SmScriptToken() {
  
}

/*
 * printChar
 */

SmScriptToken::TokenType SmPrintCharToken::type() const {
  return SmScriptToken::printChar;
}

void SmPrintCharToken::read(int opcode, BlackT::TStream& ifs) {
  charNum = (opcode / SmScriptToken::bytesPerFontChar);
  delay = ifs.readu16be();
}

void SmPrintCharToken::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(charNum * SmScriptToken::bytesPerFontChar);
  ofs.writeu16be(delay);
}

/*
 * endOfScript
 */

SmScriptToken::TokenType SmEndOfScriptToken::type() const {
  return SmScriptToken::endOfScript;
}

void SmEndOfScriptToken::read(int opcode, BlackT::TStream& ifs) {
  // no parameters, opcode is constant
}

void SmEndOfScriptToken::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(SmScriptToken::endOfScriptOp);
}

/*
 * linebreak
 */

SmScriptToken::TokenType SmLinebreakToken::type() const {
  return SmScriptToken::linebreak;
}

void SmLinebreakToken::read(int opcode, BlackT::TStream& ifs) {
  charNum = ifs.readu16be() / SmScriptToken::bytesPerFontChar;
  delay = ifs.readu16be();
}

void SmLinebreakToken::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(SmScriptToken::linebreakOp);
  ofs.writeu16be(charNum * SmScriptToken::bytesPerFontChar);
  ofs.writeu16be(delay);
}

/*
 * clearBox
 */

SmScriptToken::TokenType SmClearBoxToken::type() const {
  return SmScriptToken::clearBox;
}

void SmClearBoxToken::read(int opcode, BlackT::TStream& ifs) {
  // no parameters, opcode is constant
}

void SmClearBoxToken::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(SmScriptToken::clearBoxOp);
}

/*
 * setPortrait
 */

SmScriptToken::TokenType SmSetPortraitToken::type() const {
  return SmScriptToken::setPortrait;
}

void SmSetPortraitToken::read(int opcode, BlackT::TStream& ifs) {
  portraitCode = ifs.readu16be();
  delay = ifs.readu16be();
}

void SmSetPortraitToken::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(SmScriptToken::setPortraitOp);
  ofs.writeu16be(portraitCode);
  ofs.writeu16be(delay);
}

/*
 * unknown1
 */

SmScriptToken::TokenType SmUnknown1Token::type() const {
  return SmScriptToken::unknown1;
}

void SmUnknown1Token::read(int opcode, BlackT::TStream& ifs) {
  delay = ifs.readu16be();
}

void SmUnknown1Token::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(SmScriptToken::unknown1);
  ofs.writeu16be(delay);
}

/*
 * unknown2
 */

SmScriptToken::TokenType SmUnknown2Token::type() const {
  return SmScriptToken::unknown2;
}

void SmUnknown2Token::read(int opcode, BlackT::TStream& ifs) {
  unknown = ifs.readu16be();
}

void SmUnknown2Token::write(BlackT::TStream& ofs) const {
  ofs.writeu16be(SmScriptToken::unknown2);
  ofs.writeu16be(unknown);
}






struct CharacterCodeEntry {
  int id;
  const char* name;
};

const static CharacterCodeEntry characterCodes[] = {
  { 0x0000, "Luna" },
  { 0x0200, "Crow" },
  { 0x0400, "Artemis" },
  { 0x0600, "Jadeite" },
  { 0x0800, "Kunzite" },
  { 0x0A00, "Nephrite" },
  { 0x0C00, "Beryl" },
  { 0x0E00, "Tuxedo" },
  { 0x1000, "Zoisite" },
  { 0x1200, "Zoisite 2" },
  { 0x1400, "Zoisite 3" },
  { 0x1600, "Endymion" },
  { 0x1800, "Metallia" },
  { 0x1A00, "Manegin" },
  { 0xFFFF, "Moon" },
  { 0xF800, "Mercury" },
  { 0xF000, "Mars" },
  { 0xE800, "Jupiter" },
  { 0xE000, "Venus" }
};

const CharacterCodeEntry* getCharacter(int code) {
  for (int i = 0;
       i < sizeof(characterCodes) / sizeof(CharacterCodeEntry);
       i++) {
    if (characterCodes[i].id == code) return &(characterCodes[i]);
  }
  
  return NULL;
}

const CharacterCodeEntry* getCharacter(const char* character) {
  for (int i = 0;
       i < sizeof(characterCodes) / sizeof(CharacterCodeEntry);
       i++) {
    if (strcmp(characterCodes[i].name, character) == 0)
      return &(characterCodes[i]);
  }
  
  return NULL;
}

SmScript::SmScript()
  : offset_(0),
    origSize_(0),
    baseVramAddr_(0),
    baseSpritePos_(0),
    spriteTypeOffset_(0) {
  
}

SmScript::~SmScript() {
  for (TokenCollection::size_type i = 0; i < tokens_.size(); i++) {
    delete tokens_[i];
  }
}
  
SmScript::SmScript(const SmScript& src) {
  copy(src);
}

void SmScript::operator=(const SmScript& src) {
  copy(src);
}
  
void SmScript::read(BlackT::TStream& ifs) {
  offset_ = ifs.tell();
  baseVramAddr_ = ifs.readu32be();
  baseSpritePos_ = ifs.readu16be();
  spriteTypeOffset_ = ifs.readu16be();

/*  while (true) {
//    std::cerr << std::hex << ifs.tell() << std::endl;
//    std::cout << "here1" << std::endl;
    tokens_.push_back(TokenPtr());
    TokenPtr& ptr = tokens_[tokens_.size() - 1];
//    std::cout << "here2" << std::endl;
    ptr.setPtr(SmScriptToken::fetchToken(ifs));
    if (ptr->type() == SmScriptToken::endOfScript) break;
  } */

  while (true) {
//    std::cerr << std::hex << ifs.tell() << std::endl;
//    std::cout << "here1" << std::endl;
    TokenPtr ptr = SmScriptToken::fetchToken(ifs);
//    std::cout << "here2" << std::endl;
    tokens_.push_back(ptr);
    if (ptr->type() == SmScriptToken::endOfScript) break;
  }
  
  origSize_ = ifs.tell() - offset_;
}

void SmScript::write(BlackT::TStream& ofs) const {
  ofs.writeu32be(baseVramAddr_);
  ofs.writeu16be(baseSpritePos_);
  ofs.writeu16be(spriteTypeOffset_);
  
  for (TokenCollection::size_type i = 0; i < tokens_.size(); i++) {
    tokens_[i]->write(ofs);
  }
}
  
void SmScript::writeCsv(BlackT::TStream& ofs,
                        const BlackT::TThingyTable& thingy,
                        TextMode mode,
                        bool showControlCodes,
                        const std::vector<int> pointers) const {
  if (!tokens_.size()) return;
  
  ofs.write("ScriptStart,"
              + TStringConversion::intToString(offset_,
                  TStringConversion::baseHex)
              + " "
              + TStringConversion::intToString(origSize_,
                  TStringConversion::baseHex)
              + " "
              + TStringConversion::intToString(baseVramAddr_,
                  TStringConversion::baseHex)
              + " "
              + TStringConversion::intToString(baseSpritePos_,
                  TStringConversion::baseHex)
              + " "
              + TStringConversion::intToString(spriteTypeOffset_,
                  TStringConversion::baseHex));
  ofs.write(" "
            + TStringConversion::intToString(pointers.size(),
                TStringConversion::baseHex));
  for (std::vector<int>::size_type i = 0; i < pointers.size(); i++) {
    ofs.write(" "
              + TStringConversion::intToString(pointers[i],
                  TStringConversion::baseHex));
  }
  ofs.write(",,----------------------------,,,\n");
  
  int curPortrait = -2;
  int curPortraitDelay = -1;
  
  int pos = 0;
  while (pos < tokens_.size()) {
    writeNextCsv(ofs, &pos, &curPortrait, &curPortraitDelay,
                 thingy, mode, showControlCodes);
  }
  
}
  
int intFromIss(std::istringstream& iss) {
  std::string str;
  iss >> str;
  return TStringConversion::stringToInt(str);
}
  
int SmScript::readCsv(const BlackT::TCsv& csv,
               int* rowP,
               const SmFont& font,
               const BlackT::TThingyTable& thingy) {
  int& row = *rowP;
  
  int speed = -1;
  int pause = -1;
  int curPortrait = -1;
  
  // first row must be ScriptStart
  if (csv.cell(0, row).compare("ScriptStart") != 0) {
    throw TGenericException(T_SRCANDLINE,
                            "SmScript::readCsv()",
                            "Error: first row not ScriptStart (got "
                              + csv.cell(0, row)
                              + ")");
  }
  
  // get parameters
  std::istringstream iss(csv.cell(1, row));
  offset_ = intFromIss(iss);
  origSize_ = intFromIss(iss);
  baseVramAddr_ = intFromIss(iss);
  baseSpritePos_ = intFromIss(iss);
  spriteTypeOffset_ = intFromIss(iss);
  int width;
  switch (spriteTypeOffset_) {
  case 0x0:
    width = 128;
    break;
  case 0x14:
    width = 224;
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "SmScript::readCsv()",
                            "Error: unknown width value "
                              + TStringConversion::intToString(
                                  spriteTypeOffset_));
    break;
  }
  
  // move to script rows
  ++row;
  
  int result = 0;
  
  while (true) {
    
    // check for script end
    if (csv.cell(0, row).compare("ScriptEnd") == 0) {
      ++row;
      break;
    }
    
    // loop over rows until ScriptEnd reached
    int subresult = readCsvRow(csv, rowP, font, thingy,
               &speed, &pause, &curPortrait,
               width);
    
    if (subresult != 0) result = subresult;
  }
    
  // add terminator
  tokens_.push_back(new SmEndOfScriptToken());
  
  return result;
}
  
int SmScript::readCsvRow(const BlackT::TCsv& csv,
                int* rowP,
                const SmFont& font,
                const BlackT::TThingyTable& thingy,
                int* speedP,
                int* pauseP,
                int* curPortraitP,
                int width) {
  int result = 0;
                
  int& row = *rowP;
  int& speed = *speedP;
  int& pause = *pauseP;
  
  // verify that this is a text command
  if (csv.cell(0, row).compare("Text") != 0) {
    throw TGenericException(T_SRCANDLINE,
                            "SmScript::readCsvRow()",
                            "Error: unrecognized row type "
                              + csv.cell(0, row));
  }
  
  
//  std::cerr << row << std::endl;
  
  // fetch initial speed and pause
  std::istringstream iss(csv.cell(1, row));
  speed = intFromIss(iss);
  pause = intFromIss(iss);
  
  // add portrait switch (if not NONE)
  std::string portraitString = csv.cell(2, row);
  const CharacterCodeEntry* ccEntry = getCharacter(portraitString.c_str());
  if (ccEntry != NULL) {
    SmSetPortraitToken* token = new SmSetPortraitToken();
    token->portraitCode = ccEntry->id;
    // fetch delay
    token->delay = intFromIss(iss);
    tokens_.push_back(token);
  }
  
  // convert translated text from UTF-8 to UTF-32
  std::string translatedRaw = csv.cell(4, row);
  TUtf32Chars translated;
  TCharFmt::utf8To32(translatedRaw, translated);
  
  // read each block of text (to double linebreak/end of cell)
  int tPos = 0;
  while (tPos < translated.size()) {
    TUtf32Chars nextStr;
    getNextTextBlock(translated,
                     &tPos,
                     &nextStr);
//    for (int i = 0; i < nextStr.size(); i++) {
//      std::cerr << thingy.getRevEntry(nextStr[i]) << " ";
//    }
//    std::cerr << std::endl;
    
    // run through font calculator to get word-wrapped, formatted
    // UTF-32 string (or fail)
    TUtf32Chars nextStrProcessed;
    int subresult = SmFontCalc::processString(nextStr, &nextStrProcessed,
                              font, thingy, width);
    if (subresult != 0) {
      result = -1;
      
      std::cerr << "  Error formatting row " << row << "!" << std::endl;
//      std::cerr << translatedRaw << std::endl;
//      std::cerr << std::endl;
      
      std::cerr << "  Gave up at:" << std::endl;
      std::cerr << std::endl;
    
      for (TUtf32Chars::size_type i = 0; i < nextStrProcessed.size(); i++) {
        if (SmFontCalc::isLinebreak(nextStrProcessed[i])) {
          std::cerr << std::endl;
          continue;
        }
        
//        std::cerr << thingy.getRevEntry(nextStrProcessed[i]) << " ";
//        std::cerr << (wctype_t)(nextStrProcessed[i]);
        std::cerr << (char)(nextStrProcessed[i]);
      }
      
      std::cerr << std::endl << std::endl;
      std::cerr << "-----------------------"
        << std::endl << std::endl;
//      if (nextStrProcessed.size() != 0)
//        std::cerr << std::endl; 
      continue;
    }
    
    // parse formatted UTF-32 string for characters, speed adjustments
    TUtf32Chars::size_type ppos = 0;
    while (ppos < nextStrProcessed.size()) {
      
      TChar32 next = nextStrProcessed[ppos];
      
      // handle linebreaks
      if (SmFontCalc::isLinebreak(next)) {
        // due to the way the game handles things internally, every linebreak
        // _must_ be followed by at least one printable character
        ++ppos;
        if (ppos >= nextStrProcessed.size()) {
          std::cerr << "  Error formatting row " << row << "!" << std::endl;
          std::cerr << "  No character after linebreak" << std::endl;
          result = -2;
          break;
        }
        
        next = nextStrProcessed[ppos];
        
        SmLinebreakToken* token = new SmLinebreakToken();
        token->charNum = thingy.getRevEntry(next);
        
        // if this is the last character in the box, use pause instead of speed
        if (ppos == nextStrProcessed.size() - 1) {
          token->delay = pause;
        }
        else {
          token->delay = speed;
        }
        
        tokens_.push_back(token);
        
        ++ppos;
      }
      // handle control codes
      else if (SmFontCalc::isControlCodeStart(next)) {
        ++ppos;
        char codeType = (char)(nextStrProcessed[ppos++]);
        
        // get numeric argument
        std::string numStr;
        while (!SmFontCalc::isControlCodeEnd(
                  next = nextStrProcessed[ppos++])) {
          numStr += next;
        }
        
        int num = TStringConversion::stringToInt(numStr);
        
//        std::cerr << num << std::endl;
        
        switch (codeType) {
        case 's':
          speed = num;
          break;
        case 'p':
          pause = num;
          break;
        default:
          std::cerr << "  Error formatting row " << row << "!" << std::endl;
          std::cerr << "  Unrecognized control code: " << codeType
            << std::endl;
          result = -3;
          break;
        }
      }
      // standard character print
      else {
        
        SmPrintCharToken* token = new SmPrintCharToken();
        token->charNum = thingy.getRevEntry(next);
        
        // if this is the last character in the box, use pause instead of speed
        if (ppos == nextStrProcessed.size() - 1) {
          token->delay = pause;
        }
        else {
          token->delay = speed;
        }
        
        tokens_.push_back(token);
        
        ++ppos;
      }
    }
    
    // put a clearBox op at the end of every box except the last in the cell
    if (tPos < translated.size()) {
      tokens_.push_back(new SmClearBoxToken());
    }
  }
  
  ++row;
  
  // peek at next row: if it's ScriptEnd, we don't need to put a
  // clearBox op at the end
  if (csv.cell(0, row).compare("ScriptEnd") != 0) {
    tokens_.push_back(new SmClearBoxToken());
  }
  
  return result;
}
  
void SmScript::getNextTextBlock(const BlackT::TUtf32Chars& src,
                             int* posP,
                             BlackT::TUtf32Chars* dstP) {
  int& pos = *posP;
  int startPos = pos;
  
  while (pos < src.size()) {
    if ((pos <= src.size() - 2)
        && (src[pos] == '\n')
        && (src[pos + 1] == '\n')) {
      dstP->resize(pos - startPos);
      for (TUtf32Chars::size_type i = 0;
           i < dstP->size();
           i++) {
        (*dstP)[i] = src[startPos + i];
      }
      
      pos += 2;
      return;
    }
    else {
      ++pos;
    }
  }
  
  dstP->resize(pos - startPos);
  for (TUtf32Chars::size_type i = 0;
       i < dstP->size();
       i++) {
    (*dstP)[i] = src[startPos + i];
  }
}

void SmScript::copy(const SmScript& src) {
  tokens_.resize(src.tokens_.size());
  
  for (TokenCollection::size_type i = 0; i < tokens_.size(); i++) {
//    tokens_[i].setPtr(SmScriptToken::clone(*src.tokens_[i]));
    tokens_[i] = (SmScriptToken::clone(*src.tokens_[i]));
  }
}
  
SmScript::TokenPtr SmScript::fetchNextToken(int* pos) const {
  if (*pos >= tokens_.size()) {
    throw TGenericException(T_SRCANDLINE,
                            "SmScript::fetchNextToken()",
                            std::string("Error: out-of-range token access: ")
                              + TStringConversion::intToString(*pos));
  }

  return tokens_[(*pos)++];
}

bool SmScript::nextTokenIsType(const int* pos,
                     SmScriptToken::TokenType type) const {
  if (*pos >= tokens_.size()) return false;
  
  return (tokens_[*pos]->type() == type);
}

bool SmScript::nextTokenIsTextTerminator(const int* pos) const {
  return (nextTokenIsType(pos, SmScriptToken::endOfScript)
          || nextTokenIsType(pos, SmScriptToken::setPortrait)
          || nextTokenIsType(pos, SmScriptToken::unknown1)
          || nextTokenIsType(pos, SmScriptToken::unknown2)
          || nextTokenIsType(pos, SmScriptToken::clearBox));
}
  
void SmScript::writeNextCsv(BlackT::TStream& ofs, int* pos,
                  int* curPortrait,
                  int* curPortraitDelay,
                  const BlackT::TThingyTable& thingy,
                  TextMode mode,
                  bool showControlCodes) const {
  TokenPtr nextPtr = tokens_[*pos];
  
  switch (nextPtr->type()) {
  case SmScriptToken::printChar:
  case SmScriptToken::linebreak:
  case SmScriptToken::clearBox:
    writeNextCsvText(ofs, pos, curPortrait, curPortraitDelay,
                     thingy, mode, showControlCodes);
    break;
  case SmScriptToken::endOfScript:
    // done
    ofs.write("ScriptEnd,,,----------------------------,,,\n");
    *pos = tokens_.size();
    return;
    break;
  case SmScriptToken::setPortrait:
    {
      SmSetPortraitToken& token = dynamic_cast<SmSetPortraitToken&>(
        *nextPtr);
      *curPortrait = token.portraitCode;
      *curPortraitDelay = token.delay;
/*      ofs.write("SetPortrait,");
      ofs.write(TStringConversion::intToString(*curPortrait)
                + " "
                + TStringConversion::intToString(token.delay)
                + ",");
      ofs.write(",,,,\n"); */
      
      fetchNextToken(pos);
    }
    break;
  case SmScriptToken::unknown1:
    {
      SmUnknown1Token& token = dynamic_cast<SmUnknown1Token&>(
        *nextPtr);
      
      ofs.write("Unknown1,");
      ofs.write(TStringConversion::intToString(token.delay)
                + ",");
      ofs.write(",,,,\n");
      
      fetchNextToken(pos);
    }
    break;
  case SmScriptToken::unknown2:
    {
      SmUnknown2Token& token = dynamic_cast<SmUnknown2Token&>(
        *nextPtr);
      
      ofs.write("Unknown2,");
      ofs.write(TStringConversion::intToString(token.unknown)
                + ",");
      ofs.write(",,,,\n");
      
      fetchNextToken(pos);
    }
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "SmScript::writeNextCsv()",
                            std::string("Unknown token type: ")
                              + TStringConversion::intToString(
                                  nextPtr->type()));
    break;
  }
}
  
void SmScript::writeNextCsvText(BlackT::TStream& ofs, int* pos,
                  int* curPortrait,
                  int* curPortraitDelay,
                  const BlackT::TThingyTable& thingy,
                  TextMode mode,
                  bool showControlCodes) const {
  int speed = -1;
  int pause = -1;
  int origPause = -1;
  int origSpeed = -1;
  std::string jp;
  
  while (true) {
    TokenPtr tokenPtr = fetchNextToken(pos);
    
    bool done = false;
    switch (tokenPtr->type()) {
    case SmScriptToken::endOfScript:
    case SmScriptToken::setPortrait:
    case SmScriptToken::unknown1:
    case SmScriptToken::unknown2:
      --(*pos);
      done = true;
      break;
    case SmScriptToken::printChar:
      {
        SmPrintCharToken& token = dynamic_cast<SmPrintCharToken&>(
          *tokenPtr);
        bool isLast = nextTokenIsTextTerminator(pos);
        
        // Update text speed, if applicable
        if (!isLast) {
          if (speed == -1) {
            speed = token.delay;
            origSpeed = token.delay;
          }
          else if ((token.delay != speed)) {
//            std::cerr << "Warning: dynamic text speed at 0x"
//              << std::hex << *pos - 1 << std::endl;

            if (mode == split) {
              --(*pos);
              done = true;
              break;
            }
            else {
              if (showControlCodes) {
                jp += "[s" + TStringConversion::intToString(token.delay)
                  + "]";
              }
              speed = token.delay;
            }
          }
        }
        else {
          if (pause == -1) {
            origPause = token.delay;
            pause = token.delay;
          }
          else if ((token.delay != pause)) {
//            std::cerr << "Warning: dynamic text pause at 0x"
//              << std::hex << *pos - 1 << std::endl;

            if (mode == split) {
              --(*pos);
              done = true;
              break;
            }
            else {
              if (showControlCodes) {
                jp += "[p" + TStringConversion::intToString(token.delay)
                  + "]";
              }
              pause = token.delay;
            }
          }
        }
        
        jp += thingy.getEntry(token.charNum);
      }
      break;
    case SmScriptToken::linebreak:
      {
        // this is, unfortunately, an exact model of how linebreaks are handled
        // by the game, complete with copy-and-paste code
        
        jp += "\n";
        
        SmLinebreakToken& token = dynamic_cast<SmLinebreakToken&>(
          *tokenPtr);
        bool isLast = nextTokenIsTextTerminator(pos);
        
        // Update end-of-box pause, if applicable
        if (!isLast) {
          if (speed == -1) {
            speed = token.delay;
            origSpeed = token.delay;
          }
          else if ((token.delay != speed)) {
//            std::cerr << "Warning: dynamic text linebreak speed at 0x"
//              << std::hex << *pos - 1 << std::endl;

            if (mode == split) {
              --(*pos);
              done = true;
              break;
            }
            else {
              if (showControlCodes) {
                jp += "[s" + TStringConversion::intToString(token.delay)
                  + "]";
              }
              speed = token.delay;
            }
          }
        }
        else {
          if (pause == -1) {
            pause = token.delay;
            origPause = token.delay;
          }
          else if ((token.delay != pause)) {
//            std::cerr << "Warning: dynamic text linebreak pause at 0x"
//              << std::hex << *pos - 1 << std::endl;

            if (mode == split) {
              --(*pos);
              done = true;
              break;
            }
            else {
              if (showControlCodes) {
                jp += "[p" + TStringConversion::intToString(token.delay)
                  + "]";
              }
              pause = token.delay;
            }
          }
        }
        
        jp += thingy.getEntry(token.charNum);
      }
      break;
    case SmScriptToken::clearBox:
      if (nextTokenIsTextTerminator(pos)) {
      
      }
      else {
        if (showControlCodes) {
          jp += "\n\n";
        }
        else {
          jp += "\n";
        }
      }
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "SmScript::writeNextCsvText()",
                              std::string("Unknown token type: ")
                                + TStringConversion::intToString(
                                    tokenPtr->type()));
      break;
    }
    
    if (done) break;
    
  }
  
  std::string charName;
  const CharacterCodeEntry* charPtr = getCharacter(*curPortrait);
  if (charPtr != NULL) charName = std::string(charPtr->name);
  else charName = "NONE";
  
  ofs.write("Text,");
  if (mode == standard) {
    ofs.write(TStringConversion::intToString(origSpeed)
              + " "
              + TStringConversion::intToString(origPause)
              + " "
              + TStringConversion::intToString(*curPortraitDelay)
              + ",");
  }
  else {
    ofs.write(TStringConversion::intToString(speed)
              + " "
              + TStringConversion::intToString(pause)
              + " "
              + TStringConversion::intToString(*curPortraitDelay)
              + ",");
  }
//  ofs.write(TStringConversion::intToString(*curPortrait,
//              TStringConversion::baseHex)
//            + ",");
  ofs.write(charName
            + ",");
  ofs.write("\""
            + jp
            + "\",");
  // en
  ofs.write(",");
  // en_edit
  ofs.write(",");
  // comments
  ofs.write("\n");
  
  
}


};
