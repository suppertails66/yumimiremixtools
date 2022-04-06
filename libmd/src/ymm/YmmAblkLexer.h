#ifndef YMM_ABLK_LEXER_H
#define YMM_ABLK_LEXER_H


#include "ymm/YmmAblkLexStream.h"
#include "util/TStream.h"

namespace Md {


class YmmAblkLexer {
public:
  YmmAblkLexer(BlackT::TStream& src__,
               YmmAblkLexStream& dstStream__);
  
  void lex();
  
protected:
  BlackT::TStream* src_;
  YmmAblkLexStream* stream_;
  int lineNum_;
  
  YmmAblkLexeme nextLexeme();
  YmmAblkLexeme nextNameLexeme();
  YmmAblkLexeme nextNumberLexeme();
  YmmAblkLexeme nextIntegerLexeme();
  YmmAblkLexeme nextDoubleLexeme();
  YmmAblkLexeme nextStringLexeme();
  
  void skipSpace();
  void skipLine();
  void skipBlockComment();
};

}


#endif
