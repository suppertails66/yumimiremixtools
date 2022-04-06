#include "ymm/YmmAblkLexStream.h"
#include "exception/TGenericException.h"
#include "util/TStringConversion.h"

using namespace BlackT;

namespace Md {


YmmAblkLexStream::YmmAblkLexStream()
  : pos_(0) { }

YmmAblkLexeme YmmAblkLexStream::get() {
  if (pos_ < 0) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::get()",
                            "Accessed before start of stream");
  }
  else if (pos_ >= lexemes_.size()) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::get()",
                            "Accessed beyond end of stream");
  }
  
  return lexemes_[pos_++];
}

void YmmAblkLexStream::unget() {
  --pos_;
}

YmmAblkLexeme YmmAblkLexStream::peek() {
  if (pos_ < 0) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::peek()",
                            "Accessed before start of stream");
  }
  else if (pos_ >= lexemes_.size()) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::peek()",
                            "Accessed beyond end of stream");
  }
  
  return lexemes_[pos_];
}

bool YmmAblkLexStream::eof() const {
  return (pos_ >= lexemes_.size());
}

void YmmAblkLexStream::reset() {
  pos_ = 0;
}

void YmmAblkLexStream::append(YmmAblkLexeme lexeme) {
  lexemes_.push_back(lexeme);
}

YmmAblkLexeme YmmAblkLexStream::match(std::string type) {
  if (eof()) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::match()",
                            "Read beyond end of stream");
  }
  
  YmmAblkLexeme next = get();
  if (!next.isType(type)) {
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkLexStream::match()",
                            std::string("Line ")
                              + TStringConversion::intToString(next.lineNum)
                              + ":\n  Lexical mismatch: expected "
                              + type
                              + ", got "
                              + next.type);
  }
  
  return next;
}


}
