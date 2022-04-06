#include "ymm/YmmAblkLexer.h"
#include "util/TStringConversion.h"
#include "util/TParse.h"
#include "exception/TGenericException.h"
#include <cstring>
#include <cctype>
#include <iostream>

using namespace BlackT;

namespace Md {


YmmAblkLexer::YmmAblkLexer(BlackT::TStream& src__,
             YmmAblkLexStream& dstStream__)
  : src_(&src__),
    stream_(&dstStream__),
    lineNum_(1) { }
             
void YmmAblkLexer::lex() {
  while (!src_->eof()) {
    skipSpace();
    if (src_->eof()) break;
    
    YmmAblkLexeme lexeme = nextLexeme();
    lexeme.lineNum = lineNum_;
//    std::cout << lineNum_ << " " << lexeme.type << " "
//      << lexeme.stringVal << " " << lexeme.intVal << std::endl;
    stream_->append(lexeme);
    
//    stream_->append(nextLexeme());
  }
}
  
YmmAblkLexeme YmmAblkLexer::nextLexeme() {
  char next = src_->peek();
  if (isdigit(next)) return nextNumberLexeme();
  else if (isalpha(next)) return nextNameLexeme();
  else if (next == '"') return nextStringLexeme();
  
  src_->get();
  
  YmmAblkLexeme lexeme;
  switch (next) {
  case '.':
  case '*':
  case '(':
  case ')':
  case '[':
  case ']':
  case '<':
  case '>':
  case ':':
  case '=':
    lexeme.type += next;
    lexeme.stringVal += next;
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexer::nextLexeme()",
                            "Lex error:\n  Line "
                              + TStringConversion::intToString(lineNum_)
                              + ": Unknown character '"
                              + next
                              + "'");
    break;
  }
  
  return lexeme;
}

YmmAblkLexeme YmmAblkLexer::nextNameLexeme() {
  YmmAblkLexeme lexeme;
  lexeme.type = "NAME";
  lexeme.lineNum = lineNum_;
  
  while (isalnum(src_->peek())
         || (src_->peek() == '_')
         || (src_->peek() == '?')) {
    lexeme.stringVal += src_->get();
  }
  
  return lexeme;
}

YmmAblkLexeme YmmAblkLexer::nextIntegerLexeme() {
  YmmAblkLexeme lexeme;
  lexeme.type = "INTEGER";
  lexeme.lineNum = lineNum_;
  
  std::string numStr;
  
  // check for unary prefixes
  if (src_->peek() == '-') numStr += src_->get();
  else if (src_->peek() == '+') src_->get();
  
  // check for hexadecimal prefix "0x"
  if (src_->peek() == '0') {
    src_->get();
    if (tolower(src_->peek()) == 'x') {
      src_->get();
      numStr += "0x";
    }
    else {
      src_->unget();
    }
  }
  
  while (isdigit(src_->peek())
         || (tolower(src_->peek()) == 'a')
         || (tolower(src_->peek()) == 'b')
         || (tolower(src_->peek()) == 'c')
         || (tolower(src_->peek()) == 'd')
         || (tolower(src_->peek()) == 'e')
         || (tolower(src_->peek()) == 'f')) numStr += src_->get();
  
  lexeme.intVal = TStringConversion::stringToInt(numStr);
  
  return lexeme;
}

YmmAblkLexeme YmmAblkLexer::nextNumberLexeme() {
  int start = src_->tell();
  std::string numstr;
  
  bool isDouble = false;
  while (!src_->eof()
         && (isdigit(src_->peek())
           || (tolower(src_->peek()) == 'a')
           || (tolower(src_->peek()) == 'b')
           || (tolower(src_->peek()) == 'c')
           || (tolower(src_->peek()) == 'd')
           || (tolower(src_->peek()) == 'e')
           || (tolower(src_->peek()) == 'f')
           || (src_->peek() == '+')
           || (src_->peek() == '-')
           || (src_->peek() == 'x')
           || (src_->peek() == '.'))) {
    if (src_->peek() == '.') isDouble = true;
    numstr += src_->get();
  }
  
  src_->seek(start);
  
  if (isDouble) {
    return nextDoubleLexeme();
  }
  else {
    return nextIntegerLexeme();
  }
}

YmmAblkLexeme YmmAblkLexer::nextDoubleLexeme() {
  YmmAblkLexeme lexeme;
  lexeme.type = "DOUBLE";
  lexeme.lineNum = lineNum_;
  
  lexeme.doubleVal = TParse::matchDouble(*src_);
  
  return lexeme;
}

YmmAblkLexeme YmmAblkLexer::nextStringLexeme() {
  YmmAblkLexeme lexeme;
  lexeme.type = "STRING";
  lexeme.lineNum = lineNum_;
  
  lexeme.stringVal = TParse::matchString(*src_);
  
  return lexeme;
}

void YmmAblkLexer::skipSpace() {
  while (!src_->eof()) {
    if (src_->peek() == '\n') {
      ++lineNum_;
      src_->get();
    }
    else if (isspace(src_->peek())) src_->get();
    else if (src_->peek() == '/') {
      src_->get();
      if (src_->peek() == '/') {
        src_->get();
        skipLine();
      }
      else if (src_->peek() == '*') {
        src_->get();
        skipBlockComment();
      }
      else {
        src_->unget();
        break;
      }
    }
    else break;
  }
}

void YmmAblkLexer::skipLine() {
  while (!src_->eof()) {
    if ((src_->get() == '\n')) {
      ++lineNum_;
      break;
    }
  }
}

void YmmAblkLexer::skipBlockComment() {
  while (!src_->eof()) {
    if (src_->peek() == '\n') ++lineNum_;
    
    if ((src_->get() == '*')
        && !src_->eof()
        && (src_->peek() == '/')) {
      src_->get();
      break;
    }
  }
}


}
