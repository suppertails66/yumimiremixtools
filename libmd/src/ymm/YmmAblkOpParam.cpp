#include "ymm/YmmAblkOpParam.h"
#include "ymm/YmmAblkEnv.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <iomanip>

using namespace BlackT;

namespace Md {


YmmAblkOpParam::YmmAblkOpParam()
  : type(YmmAblkOpParamTypes::typeByte),
    value(0) { }
  
YmmAblkOpParam::YmmAblkOpParam(
               YmmAblkOpParamTypes::YmmAblkOpParamType type__,
               int value__)
  : type(type__),
    value(value__) { }
  
void YmmAblkOpParam::print(std::ostream& ofs) const {
  switch (type) {
//  case YmmAblkOpParamTypes::typeByte:
//  case YmmAblkOpParamTypes::typeWord:
//  case YmmAblkOpParamTypes::typeLong:
//    ofs << TStringConversion::intToString(value, TStringConversion::baseHex);
//    break;
  case YmmAblkOpParamTypes::typeByte:
    ofs << "0x" << std::hex << std::setw(2) << std::setfill('0')
      << value;
    break;
  case YmmAblkOpParamTypes::typeWord:
    ofs << "0x" << std::hex << std::setw(4) << std::setfill('0')
      << value;
    break;
  case YmmAblkOpParamTypes::typeLong:
    ofs << "0x" << std::hex << std::setw(8) << std::setfill('0')
      << value;
    break;
  case YmmAblkOpParamTypes::typeParamWord:
//    ofs << "["
//      << TStringConversion::intToString(value, TStringConversion::baseHex)
//      << "]";
    ofs << "*" << "0x" << std::hex << std::setw(2) << std::setfill('0')
      << value;
    break;
  case YmmAblkOpParamTypes::typeTerminator:
    ofs << "^";
    break;
  case YmmAblkOpParamTypes::typeOpenBracket:
    ofs << "[";
    break;
  case YmmAblkOpParamTypes::typeCloseBracket:
    ofs << "]";
    break;
  case YmmAblkOpParamTypes::typeOpenAngleBracket:
    ofs << "<";
    break;
  case YmmAblkOpParamTypes::typeCloseAngleBracket:
    ofs << "> ";
    break;
  case YmmAblkOpParamTypes::typeLabel:
    ofs << ":" << labelRep;
    break;
  case YmmAblkOpParamTypes::typeVariable:
    ofs << "=" << labelRep;
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkOpParam::print()",
                            "Unknown or illegal param print type: "
                              + TStringConversion::intToString(type));
    break;
  }
}

void YmmAblkOpParam::write(BlackT::TStream& ofs,
                           YmmAblkEnv& env) const {
  switch (type) {
  case YmmAblkOpParamTypes::typeByte:
    ofs.writeu8be(value);
    break;
  case YmmAblkOpParamTypes::typeWord:
    ofs.writeu16be(value);
    break;
  case YmmAblkOpParamTypes::typeLong:
    ofs.writeu32be(value);
    break;
  case YmmAblkOpParamTypes::typeParamWord:
    ofs.writeu8be(0x80);
    ofs.writeu8be(value);
    break;
  case YmmAblkOpParamTypes::typeTerminator:
    ofs.writeu8be(0x80);
    break;
  case YmmAblkOpParamTypes::typeLabel:
    // save target for second-pass resolution
    env.addLabelTarget(labelRep, ofs.tell(), 2);
    // write placeholder value
    ofs.writeu16be(0x0000);
    break;
  case YmmAblkOpParamTypes::typeVariable:
    // this should never happen; variable substitution is done
    // at read time, before we ever touch anything
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkOpParam::write()",
                            "Bad variable substitution");
    break;
  default:
    throw TGenericException(T_SRCANDLINE,
                            "YmmAblkOpParam::write()",
                            "Unknown or illegal param type write: "
                              + TStringConversion::intToString(type));
    break;
  }
}


}
