#include "util/TBufStream.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TStringConversion.h"
#include "util/TFileManip.h"
#include "util/TSort.h"
#include "discaster/ScriptLexer.h"
#include "discaster/StdInputStream.h"
#include "discaster/LexicalStream.h"
#include "discaster/ScriptParser.h"
#include "discaster/ScriptEvaluator.h"
#include "discaster/Object.h"
#include "exception/TException.h"
#include "exception/TGenericException.h"
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;
using namespace BlackT;
using namespace Discaster;

string as2bHex(int num) {
  string str = TStringConversion::intToString(num,
                  TStringConversion::baseHex).substr(2, string::npos);
  while (str.size() < 2) str = string("0") + str;
  
//  return "<$" + str + ">";
  return str;
}

string as2bHexPrefix(int num) {
  return "$" + as2bHex(num) + "";
}

int main(int argc, char* argv[]) {
/*  TFileManip::FileInfoCollection infoCollection;
  
  TFileManip::getDirectoryListing(".", infoCollection);
  std::sort(infoCollection.begin(),
            infoCollection.end(),
            TFileInfo::sortByIso9660Name);
  for (unsigned int i = 0; i < infoCollection.size(); i++) {
//    if (!infoCollection[i].isDirectory()) continue;
  
    std::cout << "file " << i << std::endl;
    std::cout << "  " << infoCollection[i].name() << std::endl;
    std::cout << "  " << infoCollection[i].isDirectory() << std::endl;
    std::cout << "  " << infoCollection[i].size() << std::endl;
    
//    std::cout << std::endl;
  }
  
  return 0; */

  try {
    Discaster::ScriptLexer lexer;
    Discaster::LexicalStream lexStream;
    std::ifstream ifs("test.ast", ios_base::binary);
    Discaster::StdInputStream inputStream(ifs);
    
    lexer.lexProgram(lexStream, inputStream);
  //  while (!lexStream.eof()) std::cout << lexStream.get();
    
    ScriptParser parser;
    parser.parseProgram(lexStream);
    parser.parseTree().prettyPrint(std::cout);

    ScriptEvaluator evaluator;
    evaluator.evaluateProgram(parser.parseTree());
  }
  catch (BlackT::TGenericException& e) {
    std::cout << "Exception: " << e.problem() << std::endl;
  }
  
/*  Object obj;
  Object test1;
  test1.setStringValue("test1");
  Object test2;
  test2.setStringValue("test2");
  obj.setMember("test1", test1);
  obj.setMember("test2", test2);
  IntObject test3(5);
  obj.setMember("test3", test3);
  
  std::cerr << obj.getMember("test1").stringValue() << std::endl;
  std::cerr << obj.getMember("test2").stringValue() << std::endl;
  std::cerr << obj.getMember("test3").intValue() << std::endl; */
  
  return 0;
}
