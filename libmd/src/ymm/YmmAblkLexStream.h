#ifndef YMM_ABLK_LEX_STREAM_H
#define YMM_ABLK_LEX_STREAM_H


#include "ymm/YmmAblkLexeme.h"
#include "util/TStream.h"
#include <vector>

namespace Md {


class YmmAblkLexStream {
public:
  YmmAblkLexStream();
  
  YmmAblkLexeme get();
  void unget();
  YmmAblkLexeme peek();
  bool eof() const;
  void reset();
  
  void append(YmmAblkLexeme lexeme);
  
  YmmAblkLexeme match(std::string type);
  
protected:
  typedef std::vector<YmmAblkLexeme> YmmAblkLexemeCollection;
  
  YmmAblkLexemeCollection lexemes_;
  int pos_;
};


}


#endif
