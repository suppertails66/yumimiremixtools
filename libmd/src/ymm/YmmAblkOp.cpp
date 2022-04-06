#include "ymm/YmmAblkOp.h"
#include "ymm/YmmAblkOps.h"
#include "ymm/YmmAblkOpNotFoundExp.h"
#include "exception/TGenericException.h"
#include "util/TStringConversion.h"
#include <cstring>
#include <iostream>
#include <list>

using namespace BlackT;

namespace Md {


YmmAblkOp::YmmAblkOp(int id__, const char* name__,
                     const char* paramString__)
  : id(id__),
    name(name__),
    paramString(paramString__) { }
  
void YmmAblkOp::readParams(BlackT::TStream& ifs) {
  int pos = 0;
//  int loopPos = 0;
  std::list<int> loopPosStack;
  bool terminatorRead = false;
  
  while (paramString[pos] != 0) {
    char next = paramString[pos++];
    
    switch (next) {
    case 'b':
    {
      int value = ifs.readu8be();
      YmmAblkOpParam param
        = YmmAblkOpParam(YmmAblkOpParamTypes::typeByte, value);
      params_.push_back(param);
      terminatorRead = false;
    }
      break;
    case 'w':
    {
      int value = ifs.readu16be();
      YmmAblkOpParam param
        = YmmAblkOpParam(YmmAblkOpParamTypes::typeWord, value);
      params_.push_back(param);
      terminatorRead = false;
    }
      break;
    case 'l':
    {
      int value = ifs.readu32be();
      YmmAblkOpParam param
        = YmmAblkOpParam(YmmAblkOpParamTypes::typeLong, value);
      params_.push_back(param);
      terminatorRead = false;
    }
      break;
    case '*':
    {
      int value = ifs.readu16be();
      
      YmmAblkOpParam param
        = YmmAblkOpParam(YmmAblkOpParamTypes::typeWord, value);
      // if top byte is 0x80, array access; otherwise, literal
      if ((value & 0xFF00) == 0x8000) {
        param
          = YmmAblkOpParam(YmmAblkOpParamTypes::typeParamWord, value & 0x00FF);
      }
      
      params_.push_back(param);
      terminatorRead = false;
    }
      break;
    case '^':
    {
      int value = ifs.readu8be();
      if (value != 0x80) {
        throw TGenericException(T_SRCANDLINE,
                              "YmmAblkOp::readParams()",
                              std::string("Missing mandatory terminator at ")
                                + TStringConversion::intToString(ifs.tell() - 1)
                                );
      }
      terminatorRead = true;
    }
      break;
    case '[':
    {
      YmmAblkOpParam param
        = YmmAblkOpParam(YmmAblkOpParamTypes::typeOpenBracket, 0);
      params_.push_back(param);
      
      // skip optional sequence in param string if it's not present
      if ((unsigned char)ifs.peek() == 0x80) {
        ifs.get();
        while (paramString[pos++] != ']');
        
        YmmAblkOpParam param
          = YmmAblkOpParam(YmmAblkOpParamTypes::typeCloseBracket, 0);
        params_.push_back(param);
        
        terminatorRead = true;
      }
    }
      break;
    case ']':
    {
        YmmAblkOpParam param
          = YmmAblkOpParam(YmmAblkOpParamTypes::typeCloseBracket, 0);
        params_.push_back(param);
    }
      break;
    case '<':
    {
//      loopPos = pos;
      loopPosStack.push_back(pos);
      terminatorRead = false;
      
      YmmAblkOpParam param
        = YmmAblkOpParam(YmmAblkOpParamTypes::typeOpenAngleBracket, 0);
      params_.push_back(param);
      
      // skip optional sequence in param string if it's not present
      if ((unsigned char)ifs.peek() == 0x80) {
        ifs.get();
        while (paramString[pos++] != '>');
        
        YmmAblkOpParam param
          = YmmAblkOpParam(YmmAblkOpParamTypes::typeCloseAngleBracket, 0);
        params_.push_back(param);
        
        terminatorRead = true;
      }
    }
      break;
    case '>':
      if ((unsigned char)ifs.peek() != 0x80) {
        pos = loopPosStack.back();
      }
      else {
        ifs.get();
        loopPosStack.pop_back();
        
        YmmAblkOpParam param
          = YmmAblkOpParam(YmmAblkOpParamTypes::typeCloseAngleBracket, 0);
        params_.push_back(param);
        
        terminatorRead = true;
      }
      break;
    case '!':
    {
      terminatorRead = true;
        
      if (id == 0x02) {
        int subid = params_[0].value;
        try {
          YmmAblkOp subop = makeYmmAblkOp02Subop(subid);
          subop.readParams(ifs);
          for (int i = 0; i < subop.params_.size(); i++) {
            params_.push_back(subop.params_[i]);
          }
        }
        catch (YmmAblkOpNotFoundExp& e) {
          throw TGenericException(T_SRCANDLINE,
                                  "YmmAblkOp::readParams()",
                                  std::string("Unknown 02 subop ")
                                    + TStringConversion::intToString(
                                        subid,
                                        TStringConversion::baseHex)
                                    + " at "
                                    + TStringConversion::intToString(
                                        ifs.tell() - 2,
                                        TStringConversion::baseHex));
        }
      }
//      else if (id == 0x13) {
//        // ignore; only matters when writing
//      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "YmmAblkOp::readParams()",
                                std::string("Unknown special op type: ")
                                  + TStringConversion::intToString(id,
                                      TStringConversion::baseHex));
      }
    }
      break;
    // NEW FOR TRANSLATION
    case '&':
    {
      // this should never happen -- labels were added for the translation
//      throw TGenericException(T_SRCANDLINE,
//                              "YmmAblkOp::readParams(TStream&)",
//                              std::string("Somehow tried to read a label"));
      int value = ifs.reads16be();
      YmmAblkOpParam param
        = YmmAblkOpParam(YmmAblkOpParamTypes::typeWord, value);
      params_.push_back(param);
      terminatorRead = false;
    }
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "YmmAblkOp::readParams(TStream&)",
                              std::string("Invalid param symbol: ")
                                + next);
      break;
    }
  }
  
  if (!terminatorRead) ifs.get();
}

void YmmAblkOp::readParams(YmmAblkLexStream& src,
                           YmmAblkEnv& env) {
  int pos = 0;
//  int loopPos = 0;
  std::list<int> loopPosStack;
  
//  std::cerr << "xxxxxx" << std::endl;
  while (paramString[pos] != 0) {
    
    char next = paramString[pos++];
//    std::cerr << next << std::endl;
    
    switch (next) {
    case 'b':
    {
      YmmAblkOpParam param = readParam(src, env);
      param.type = YmmAblkOpParamTypes::typeByte;
      params_.push_back(param);
    }
      break;
    case 'w':
    {
      YmmAblkOpParam param = readParam(src, env);
      param.type = YmmAblkOpParamTypes::typeWord;
      params_.push_back(param);
    }
      break;
    case 'l':
    {
      YmmAblkOpParam param = readParam(src, env);
      param.type = YmmAblkOpParamTypes::typeLong;
      params_.push_back(param);
    }
      break;
    case '*':
    {
      bool isScriptVar = false;
      if (src.peek().type.compare("*") == 0) {
        isScriptVar = true;
        src.get();
      }
      
      YmmAblkOpParam param = readParam(src, env);
      if (isScriptVar) {
        param.type = YmmAblkOpParamTypes::typeParamWord;
      }
      else {
        param.type = YmmAblkOpParamTypes::typeWord;
      }
      
      params_.push_back(param);
    }
      break;
    case '^':
    {
      // write terminator
      YmmAblkOpParam param;
      param.type = YmmAblkOpParamTypes::typeTerminator;
      params_.push_back(param);
    }
      break;
    case '[':
    {
      // read optional sequence if present
/*      if (src.peek().type.compare("[") == 0) {
        src.get();
      }
      // otherwise, skip sequence in param string
      else {
        while (paramString[pos++] != ']');
      } */
      
      src.get();
      if (src.peek().type.compare("]") == 0) {
        while (paramString[pos++] != ']');
        
        // write terminator
        YmmAblkOpParam param;
        param.type = YmmAblkOpParamTypes::typeTerminator;
        params_.push_back(param);
        
        src.get();
      }
    }
      break;
    case ']':
    {
      // write terminator
      YmmAblkOpParam param;
      param.type = YmmAblkOpParamTypes::typeTerminator;
      params_.push_back(param);
      
      // skip trailing ']'
      src.get();
    }
      break;
    case '<':
//      loopPos = pos;
      loopPosStack.push_back(pos);
      
      // unlike one-shot optional parameters, list parameters are mandatory
      // even if the list is empty
      src.get();
      if (src.peek().type.compare(">") == 0) {
        while (paramString[pos++] != '>');
        
        // write terminator
        YmmAblkOpParam param;
        param.type = YmmAblkOpParamTypes::typeTerminator;
        params_.push_back(param);
        
        src.get();
      }
      
      break;
    case '>':
      if (src.peek().type.compare(">") != 0) {
//        pos = loopPos;
        pos = loopPosStack.back();
      }
      else {
        loopPosStack.pop_back();
        
        // write terminator
        YmmAblkOpParam param;
        param.type = YmmAblkOpParamTypes::typeTerminator;
        params_.push_back(param);
        
        src.get();
      }
      break;
    case '!':
    {
      if (id == 0x02) {
        YmmAblkLexeme lexeme = src.peek();
        int subid = params_[0].value;
        
//        YmmAblkOpParam param;
//        param.type = YmmAblkOpParamTypes::typeByte;
//        param.value = subid;
//        params_.push_back(param);
        
        try {
//          std::cerr << "subid: " << subid << std::endl;
          YmmAblkOp subop = makeYmmAblkOp02Subop(subid);
          subop.readParams(src, env);
          for (int i = 0; i < subop.params_.size(); i++) {
            params_.push_back(subop.params_[i]);
          }
        }
        catch (YmmAblkOpNotFoundExp& e) {
          throw TGenericException(T_SRCANDLINE,
                                  "YmmAblkOp::readParams(YmmAblkLexStream&)",
                                  std::string("Unknown 02 subop ")
                                    + TStringConversion::intToString(
                                        subid,
                                        TStringConversion::baseHex)
                                    + " at line "
                                    + TStringConversion::intToString(
                                        lexeme.lineNum));
        }
      }
//      else if (id == 0x13) {
//        // write 0x80 terminator??
//      }
      else {
        throw TGenericException(T_SRCANDLINE,
                                "YmmAblkOp::readParams(YmmAblkLexStream&)",
                                std::string("Unknown special op type: ")
                                  + TStringConversion::intToString(id,
                                      TStringConversion::baseHex));
      }
    }
      break;
    // NEW FOR TRANSLATION
    case '&':
    {
      YmmAblkOpParam param = readParam(src, env);
      param.type = YmmAblkOpParamTypes::typeLabel;
      params_.push_back(param);
    }
      break;
    default:
      throw TGenericException(T_SRCANDLINE,
                              "YmmAblkOp::readParams(YmmAblkLexStream&)",
                              std::string("Invalid param symbol: ")
                                + next);
      break;
    }
  }
  
  // write terminator if not already written
  if ((params_.size() == 0)
      || ((params_.size() > 0) 
          && (params_.back().type != YmmAblkOpParamTypes::typeTerminator))) {
    YmmAblkOpParam param;
    param.type = YmmAblkOpParamTypes::typeTerminator;
    params_.push_back(param);
  }
}

void YmmAblkOp::writeParams(BlackT::TStream& ofs,
                 YmmAblkEnv& env) const {
  for (int i = 0; i < params_.size(); i++) {
    params_[i].write(ofs, env);
  }
}

void YmmAblkOp::write(BlackT::TStream& ofs,
                 YmmAblkEnv& env) const {
  ofs.writeu8be(id);
  writeParams(ofs, env);
}
  
YmmAblkOpParam YmmAblkOp::readParam(YmmAblkLexStream& src,
                          YmmAblkEnv& env) {
  YmmAblkOpParam param;
  if (src.peek().type == "NAME") {
    param.labelRep = src.get().stringVal;
  }
  else if (src.peek().type == "=") {
    if (env.translationData == NULL) {
      throw TGenericException(T_SRCANDLINE,
                    "YmmAblkOp::readParam()",
                    std::string("variable with no translation data"));
    }
    
    src.get();
    std::string varName = src.get().stringVal;
    
//    std::cerr << env.translationData->variables.size() << std::endl;
    
    YmmTranslationData::NameToVarMap::const_iterator findIt
      = env.translationData->variables.find(varName);
    if (findIt == env.translationData->variables.cend()) {
      throw TGenericException(T_SRCANDLINE,
                              "YmmAblkOp::readParam()",
                              std::string("Unknown variable: ")
                                + varName);
    }
    
    param.value = TStringConversion::stringToInt(findIt->second);
  }
  else {
    param.value = src.get().intVal;
  }
  return param;
}

void YmmAblkOp::printParams(std::ostream& ofs) const {
  for (int i = 0; i < params_.size(); i++) {
    if (i != 0) ofs << " ";
    params_[i].print(ofs);
  }
}

int YmmAblkOp::numParams() const {
  return params_.size();
}

YmmAblkOpParam& YmmAblkOp::getParam(int index) {
  return params_.at(index);
}

bool YmmAblkOp::isRenderOp() const {
  switch (id) {
  case 0x06:
  case 0x0A:
  case 0x18:
    return true;
    break;
  default:
    break;
  }
  
  return false;
}

bool YmmAblkOp::isDrawOp() const {
  switch (id) {
  // not sure about all 02 subops
  case 0x03:
  // 07, 08, 09?
  case 0x0C:
  case 0x0D:
  case 0x0E:
    return true;
    break;
  case 0x16:
    if (params_.size() > 3) return true;
    return false;
    break;
  // scrollBG/FG commands.
  // these redraw only if the extra parameters are present.
  case 0x11:
  case 0x12:
  case 0x13:
    if (params_.size() > 3) return true;
    return false;
    break;
  default:
    break;
  }
  
  return false;
}

bool YmmAblkOp::isPatternTransferOp() const {
  switch (id) {
  case 0x10:
    return true;
    break;
  default:
    break;
  }
  
  return false;
}

bool YmmAblkOp::isScrollOp() const {
  switch (id) {
  case 0x0C:
  case 0x0D:
  case 0x0E:
  case 0x11:
  case 0x12:
  case 0x13:
    return true;
    break;
  default:
    break;
  }
  
  return false;
}


} 
