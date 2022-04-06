#ifndef YMM_ABLK_OP_NOT_FOUND_EXP_H
#define YMM_ABLK_OP_NOT_FOUND_EXP_H


#include "exception/TException.h"

namespace Md {


class YmmAblkOpNotFoundExp : public BlackT::TException {
public:
  YmmAblkOpNotFoundExp(const char* nameOfSourceFile__,
                 int lineNum__,
                 const std::string& source__);
protected:
  
};


}


#endif
