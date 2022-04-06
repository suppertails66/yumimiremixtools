#ifndef YMM_ABLK_LEXEME_H
#define YMM_ABLK_LEXEME_H


#include <string>

namespace Md {


class YmmAblkLexeme {
public:
  YmmAblkLexeme();
  
  bool isType(const std::string& type__) const;
  
  std::string type;
  int lineNum;
  int intVal;
  double doubleVal;
  std::string stringVal;
protected:
  
};


}


#endif
