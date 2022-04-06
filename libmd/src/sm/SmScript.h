#ifndef SMSCRIPT_H
#define SMSCRIPT_H


#include "util/TSptr.h"
#include "util/TStream.h"
#include "util/TThingyTable.h"
#include "util/TCsv.h"
#include "util/TCharFmt.h"
#include "sm/SmFont.h"
#include <vector>

namespace Md {


class SmScriptToken {
public:
  // 4 tiles per character
  const static int bytesPerFontChar = 0x80;

  enum TokenType {
    printChar,
    endOfScript,
    linebreak,
    clearBox,
    setPortrait,
    unknown1,
    unknown2
  };
  
  virtual void read(int opcode, BlackT::TStream& ifs) =0;
  virtual void write(BlackT::TStream& ofs) const =0;
  virtual SmScriptToken::TokenType type() const =0;
  
  virtual ~SmScriptToken();
  
  static SmScriptToken* instantiate(TokenType type);
  static SmScriptToken* clone(const SmScriptToken& token);
  static SmScriptToken* fetchToken(BlackT::TStream& ifs);
protected:
  const static int endOfScriptOp = 0xFFFE;
  const static int linebreakOp = 0xFFFC;
  const static int clearBoxOp = 0xFFFA;
  const static int setPortraitOp = 0xFFF8;
  const static int unknown1Op = 0xFFF6;
  const static int unknown2Op = 0xFFF4;
  
  SmScriptToken();
};

class SmPrintCharToken : public SmScriptToken {
public:
  virtual SmScriptToken::TokenType type() const;
  virtual void read(int opcode, BlackT::TStream& ifs);
  virtual void write(BlackT::TStream& ofs) const;
  
  int charNum;
  int delay;
protected:
  
};

class SmEndOfScriptToken : public SmScriptToken {
public:
  virtual SmScriptToken::TokenType type() const;
  virtual void read(int opcode, BlackT::TStream& ifs);
  virtual void write(BlackT::TStream& ofs) const;
protected:
  
};

class SmLinebreakToken : public SmScriptToken {
public:
  virtual SmScriptToken::TokenType type() const;
  virtual void read(int opcode, BlackT::TStream& ifs);
  virtual void write(BlackT::TStream& ofs) const;
  
  int charNum;
  int delay;
protected:
  
};

class SmClearBoxToken : public SmScriptToken {
public:
  virtual SmScriptToken::TokenType type() const;
  virtual void read(int opcode, BlackT::TStream& ifs);
  virtual void write(BlackT::TStream& ofs) const;
protected:
  
};

class SmSetPortraitToken : public SmScriptToken {
public:
  virtual SmScriptToken::TokenType type() const;
  virtual void read(int opcode, BlackT::TStream& ifs);
  virtual void write(BlackT::TStream& ofs) const;
  
  int portraitCode;
  int delay;
protected:
  
};

class SmUnknown1Token : public SmScriptToken {
public:
  virtual SmScriptToken::TokenType type() const;
  virtual void read(int opcode, BlackT::TStream& ifs);
  virtual void write(BlackT::TStream& ofs) const;
  
  int delay;
protected:
  
};

class SmUnknown2Token : public SmScriptToken {
public:
  virtual SmScriptToken::TokenType type() const;
  virtual void read(int opcode, BlackT::TStream& ifs);
  virtual void write(BlackT::TStream& ofs) const;
  
  int unknown;
protected:
  
};



class SmScript {
public:
  enum TextMode {
    standard,
    split
  };

  SmScript();
  ~SmScript();
  
  SmScript(const SmScript& src);
  void operator=(const SmScript& src);
  
  void read(BlackT::TStream& ifs);
  void write(BlackT::TStream& ofs) const;
  
  void writeCsv(BlackT::TStream& ofs,
                const BlackT::TThingyTable& thingy,
                TextMode mode = standard,
                bool showControlCodes = true,
                const std::vector<int> pointers
                  = std::vector<int>()) const;
  
  int readCsv(const BlackT::TCsv& csv,
               int* rowP,
               const SmFont& font,
               const BlackT::TThingyTable& thingy);

  int offset_;
  int origSize_;
  int baseVramAddr_;
  int baseSpritePos_;
  int spriteTypeOffset_;
protected:
//  typedef BlackT::TSptr<SmScriptToken> TokenPtr;
  typedef SmScriptToken* TokenPtr;
  typedef std::vector<TokenPtr> TokenCollection;
//  typedef std::vector<

  TokenCollection tokens_;
  
  void copy(const SmScript& src);
  
  TokenPtr fetchNextToken(int* pos) const;
  bool nextTokenIsType(const int* pos,
                       SmScriptToken::TokenType type) const;
  bool nextTokenIsTextTerminator(const int* pos) const;
  
  void writeNextCsv(BlackT::TStream& ofs, int* pos,
                    int* curPortrait,
                    int* curPortraitDelay,
                    const BlackT::TThingyTable& thingy,
                    TextMode mode = standard,
                    bool showControlCodes = true) const;
  
  void writeNextCsvText(BlackT::TStream& ofs, int* pos,
                    int* curPortrait,
                    int* curPortraitDelay,
                    const BlackT::TThingyTable& thingy,
                    TextMode mode = standard,
                    bool showControlCodes = true) const;
  
  int readCsvRow(const BlackT::TCsv& csv,
                  int* rowP,
                  const SmFont& font,
                  const BlackT::TThingyTable& thingy,
                  int* speedP,
                  int* pauseP,
                  int* curPortraitP,
                  int width);
  
  static void getNextTextBlock(const BlackT::TUtf32Chars& src,
                               int* posP,
                               BlackT::TUtf32Chars* dstP);
                  
};


}


#endif
