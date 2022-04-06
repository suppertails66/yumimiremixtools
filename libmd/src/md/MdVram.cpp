#include "md/MdVram.h"
#include "util/TStringConversion.h"
#include "exception/TGenericException.h"
#include <string>

using namespace BlackT;

namespace Md {


MdVram::MdVram() { }

const MdPattern& MdVram::getPattern(int index) const {
  if (index >= numPatterns) {
    throw TGenericException(T_SRCANDLINE,
                            "MdVram::getPattern(int)",
                            (std::string("Out-of-range index: ")
                              + TStringConversion::intToString(index))
                              .c_str());
  }
  
  return patterns[index];
}

MdPattern MdVram::setPattern(int index, const MdPattern& pattern) {
  if (index >= numPatterns) {
    throw TGenericException(T_SRCANDLINE,
                            "MdVram::setPattern(int)",
                            (std::string("Out-of-range index: ")
                              + TStringConversion::intToString(index))
                              .c_str());
  }
  
  return patterns[index] = pattern;
}
  
void MdVram::read(const char* data, int numPatterns,
                  int startPattern) {
  for (int i = 0; i < numPatterns; i++) {
    patterns[startPattern + i].read(data + (i * MdPattern::uncompressedSize));
  }
}


}
