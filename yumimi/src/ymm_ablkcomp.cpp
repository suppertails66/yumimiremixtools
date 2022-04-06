#include "ymm/YmmAblkLexer.h"
#include "ymm/YmmAblkEvaluator.h"
#include "util/TIfstream.h"
#include "util/TOfstream.h"
#include "util/TBufStream.h"
#include <iostream>

using namespace std;
using namespace BlackT;
using namespace Md;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Yumimi Mix script compiler" << endl;
    cout << "Usage: " << argv[0] << " <infile> <outfile>" << endl;
    return 0;
  }
  
  TIfstream ifs(argv[1]);
  YmmAblkLexStream lexStream;
  
  YmmAblkLexer lexer(ifs, lexStream);
  lexer.lex();
  
/*  while (!lexStream.eof()) {
    YmmAblkLexeme lexeme = lexStream.get();
    std::cout << lexeme.lineNum << " " << lexeme.type << " "
      << lexeme.stringVal << " " << lexeme.intVal << std::endl;
  } */
  
  TBufStream ofs(0x400000);
  YmmAblkEvaluator evaluator(lexStream, ofs);
  evaluator.evaluate();
  ofs.save(argv[2]);
  
  return 0;
}
