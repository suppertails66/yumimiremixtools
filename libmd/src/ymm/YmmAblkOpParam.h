#ifndef YMM_ABLK_OP_PARAM_H
#define YMM_ABLK_OP_PARAM_H


#include "util/TStream.h"
#include <iostream>
#include <string>

namespace Md {


class YmmAblkEnv;

namespace YmmAblkOpParamTypes {
  enum YmmAblkOpParamType {
    typeByte,
    typeWord,
    typeLong,
    typeParamWord,
    // only here for convenience
    typeTerminator,
    typeOpenBracket,
    typeCloseBracket,
    typeOpenAngleBracket,
    typeCloseAngleBracket,
    // NEW FOR TRANSLATION
    typeLabel,
    typeVariable
  };
}

class YmmAblkOpParam {
public:
  YmmAblkOpParam();
  YmmAblkOpParam(YmmAblkOpParamTypes::YmmAblkOpParamType type__,
                 int value__);
  
  void print(std::ostream& ofs) const;
  void write(BlackT::TStream& ofs,
             YmmAblkEnv& env) const;
  
  YmmAblkOpParamTypes::YmmAblkOpParamType type;
  int value;
  std::string labelRep;
protected:
  
};


}


#endif
