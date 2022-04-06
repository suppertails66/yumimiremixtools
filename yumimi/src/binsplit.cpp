#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TParse.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace BlackT;

string as3bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 3) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as1bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 1) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}






struct MsfNumber {
  int min;
  int sec;
  int frame;
  
  const static int frameRange = 75;
  const static int secRange = 60;
  
  MsfNumber operator+(const MsfNumber& other) {
    int newmin = min;
    int newsec = sec;
    int newframe = frame;
  
    newframe += other.frame;
    if (newframe >= frameRange) {
      newframe -= frameRange;
      ++newsec;
    }
    else if (newframe < 0) {
      newframe += frameRange;
      --newsec;
    }
    
    newsec += other.sec;
    if (newsec >= secRange) {
      newsec -= secRange;
      ++newmin;
    }
    else if (newsec < 0) {
      newsec += secRange;
      --newmin;
    }
    
    newmin += other.min;
    
    return MsfNumber { newmin, newsec, newframe };
  }
  
  MsfNumber operator-(const MsfNumber& other) {
    MsfNumber conv { -(other.min), -(other.sec), -(other.frame) };
    return (*this + conv);
  }
  
  int asSectorNum() const {
    return (frame) + (sec * frameRange) + (min * secRange * frameRange);
  }
  
  int asByteNum() const {
    return asSectorNum() * 0x930;
  }
};

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cout << "Usage: " << argv[0] << " <bin> <cue> <outbase>" << endl;
    return 0;
  }
  
  std::string binName(argv[1]);
  std::string cueName(argv[2]);
  std::string outBaseName(argv[3]);
  
  TBufStream cueIfs;
  cueIfs.open(cueName.c_str());
  
  while (true) {
    std::string command = TParse::getSpacedSymbol(cueIfs);
    if (command.compare("TRACK") == 0) {
      
    }
    else if (command.compare("INDEX") == 0) {
      
    }
    else {
      std::string junk;
      cueIfs.getLine(junk);
    }
  }
  
  return 0;
}
