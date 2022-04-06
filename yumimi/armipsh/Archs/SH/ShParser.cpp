#include "stdafx.h"
#include "ShParser.h"
#include "Parser/Parser.h"
#include "Parser/ExpressionParser.h"
#include "Parser/DirectivesParser.h"
#include "Util/Util.h"
#include "Core/Common.h"
#include "Commands/CDirectiveFile.h"
#include <cstring>

#define CHECK(exp) if (!(exp)) return false;

// FIXME?
const ShRegisterDescriptor shRegisters[] = {
	{ L"r0", 0 },	{ L"r1", 1 },	{ L"r2", 2 },	{ L"r3", 3 },
	{ L"r4", 4 },	{ L"r5", 5 },	{ L"r6", 6 },	{ L"r7", 7 },
	{ L"r8", 8 },	{ L"r9", 9 },	{ L"r10", 10 },	{ L"r11", 11 },
	{ L"r12", 12 },	{ L"r13", 13 },	{ L"sp", 13 },	{ L"r14", 14 },
//	{ L"lr", 14 },
	{ L"r15", 15 },
//	{ L"pc", 15 },
};

/*std::unique_ptr<CAssemblerCommand> parseDirectiveSh(Parser& parser, int flags)
{
	Sh.SetThumbMode(false);
	return make_unique<ShStateCommand>(true);
}*/

std::unique_ptr<CAssemblerCommand> parseDirectivePoolSh(Parser& parser, int flags)
{
	auto seq = make_unique<CommandSequence>();
	seq->addCommand(make_unique<CDirectiveAlignFill>(2,CDirectiveAlignFill::AlignVirtual));
	seq->addCommand(make_unique<ShPoolCommand>());
	// ensure word alignment for instructions following pool
	seq->addCommand(make_unique<CDirectiveAlignFill>(2,CDirectiveAlignFill::AlignVirtual));

	return std::move(seq);
}

const wchar_t* msgTemplateSh =
	L"mov    r12,r12\n"
	L"b      %after%\n"
	L".byte  0x64,0x64,0x00,0x00\n"
	L".ascii %text%\n"
	L".align %alignment%\n"
	L"%after%:"
;

std::unique_ptr<CAssemblerCommand> parseDirectiveMsgSh(Parser& parser, int flags)
{
	Expression text = parser.parseExpression();
	if (text.isLoaded() == false)
		return nullptr;

	return parser.parseTemplate(msgTemplateSh, {
		{ L"%after%", Global.symbolTable.getUniqueLabelName() },
		{ L"%text%", text.toString() },
		{ L"%alignment%", Sh.GetThumbMode() == true ? L"2" : L"4" }
	});
}

const DirectiveMap shDirectives = {
//	{ L".thumb",	{ &parseDirectiveThumb,	0 } },
//	{ L".sh",		{ &parseDirectiveSh,	0 } },
	{ L".pool",		{ &parseDirectivePoolSh,	0 } },
	{ L".msg",		{ &parseDirectiveMsgSh,	0 } },
};

std::unique_ptr<CAssemblerCommand> ShParser::parseDirective(Parser& parser)
{
	return parser.parseDirective(shDirectives);
}

bool ShParser::parseImmediate(Parser& parser, Expression& dest)
{
	TokenizerPosition pos = parser.getTokenizer()->getPosition();

	// check if it really is an immediate
	ShOpcodeVariables tempVars;
//	if (parsePsrTransfer(parser,tempVars,false))
//		return false;

	parser.getTokenizer()->setPosition(pos);
	if (parseRegister(parser,tempVars.rn))
		return false;
	
//	parser.getTokenizer()->setPosition(pos);
//	if (parseCopNumber(parser,tempVars.rd))
//		return false;
	
//	parser.getTokenizer()->setPosition(pos);
//	if (parseCopRegister(parser,tempVars.rd))
//		return false;
	
	parser.getTokenizer()->setPosition(pos);
	dest = parser.parseExpression();
	return dest.isLoaded();
}

bool ShParser::parseRegister(Parser& parser, ShRegisterValue& dest, int max)
{
	const Token& token = parser.peekToken();
	if (token.type != TokenType::Identifier)
		return false;

	const std::wstring stringValue = token.getStringValue();
	
	for (size_t i = 0; i < ARRAY_SIZE(shRegisters); i++)
	{
		if (stringValue == shRegisters[i].name)
		{
			dest.name = stringValue;
			dest.num = shRegisters[i].num;
			parser.eatToken();
			return true;
		}
	}

	return false;
}

int ShParser::getAddressingMode(const char* str)
{
	for (int z = 0; ShAmodes[z].name != nullptr; z++)
	{
	  if (std::strcmp(str, ShAmodes[z].name) == 0) return ShAmodes[z].type;
	}
	
	return SH_AMODE_NONE;
}

bool ShParser::matchSymbol(Parser& parser, wchar_t symbol, bool optional)
{
	switch (symbol)
	{
	case '[':
		return parser.matchToken(TokenType::LBrack,optional);
	case ']':
		return parser.matchToken(TokenType::RBrack,optional);
	case ',':
		return parser.matchToken(TokenType::Comma,optional);
	case '!':
		return parser.matchToken(TokenType::Exclamation,optional);
	case '{':
		return parser.matchToken(TokenType::LBrace,optional);
	case '}':
		return parser.matchToken(TokenType::RBrace,optional);
	case '(':
		return parser.matchToken(TokenType::LParen,optional);
	case ')':
		return parser.matchToken(TokenType::RParen,optional);
	case '#':
		return parser.matchToken(TokenType::Hash,optional);
	case '=':
		return parser.matchToken(TokenType::Assign,optional);
	case '+':
		return parser.matchToken(TokenType::Plus,optional);
	case '-':
		return parser.matchToken(TokenType::Minus,optional);
	case '$':
		return parser.matchToken(TokenType::Dollar,optional);
	}

	return false;
}

bool ShParser::matchIdentifier(Parser& parser, std::string str) {
  if (parser.peekToken().type != TokenType::Identifier) return false;
  
  std::wstring tokStr = parser.peekToken().getStringValue();
  if (tokStr.size() != str.size()) return false;
  
  for (size_t i = 0; i < tokStr.size(); i++)
  {
    if (tokStr[i] != str[i]) return false;
  }
  
  parser.eatToken();
  return true;
}

bool ShParser::decodeShOpcode(const std::wstring& name, const tShOpcode& opcode, ShOpcodeVariables& vars)
{
  // FIXME?
//	vars.Opcode.c = vars.Opcode.a = 0;
//	vars.Opcode.s = false;

	const char* encoding = opcode.name;
	size_t pos = 0;

	while (*encoding != 0)
	{
		switch (*encoding++)
		{
		default:
			CHECK(pos < name.size());
			CHECK(*(encoding-1) == name[pos++]);
			break;
		}
	}

	return pos >= name.size();
}

bool ShParser::parseShParameters(Parser& parser, const tShOpcode& opcode, ShOpcodeVariables& vars)
{
  const char* encoding = opcode.argstr;

//	vars.ImmediateIsPoolVal = false;
	vars.DisplacementTargetsAddr = false;
	vars.DisplacementPcMask = 0x0;
	
  // FIXME??
	bool optional = false;
	while (*encoding != 0)
	{
	  if (*encoding == ',')
	  {
	    // match commas
      matchSymbol(parser, ',', optional);
      ++encoding;
      continue;
	  }
	  
    std::string paramIdStr;
    // get name of next param to match
    while ((*encoding != 0) && (*encoding != ','))
    {
      paramIdStr += *(encoding++);
    }
    
    int addrMode = getAddressingMode(paramIdStr.c_str());
    
    // if this corresponds to a known addressing mode, call appropriate handler;
    // otherwise, match literal content as an identifier
    switch (addrMode) {
    case SH_AMODE_NONE:
      // unrecognized param string:
      // match this sequence as an identifier
      // (handles ops with "hardcoded" parameters like macl or gbr)
      CHECK(matchIdentifier(parser, paramIdStr));
      break;
      
    case SH_AMODE_RN: // Rn
    case SH_AMODE_RM:
			CHECK(parseRegister(parser,
			  (addrMode == SH_AMODE_RN) ? vars.rn : vars.rm));
      break;
      
    case SH_AMODE_ATRN: // @Rn
    case SH_AMODE_ATRM:
			CHECK(matchSymbol(parser,'$',optional));
			CHECK(parseRegister(parser,
			  (addrMode == SH_AMODE_ATRN) ? vars.rn : vars.rm));
      break;
      
    case SH_AMODE_ATRNPL: // @Rn+
    case SH_AMODE_ATRMPL:
			CHECK(matchSymbol(parser,'$',optional));
			CHECK(parseRegister(parser,
			  (addrMode == SH_AMODE_ATRNPL) ? vars.rn : vars.rm));
			CHECK(matchSymbol(parser,'+',optional));
      break;
      
    case SH_AMODE_ATMIRN: // @-Rn
    case SH_AMODE_ATMIRM:
			CHECK(matchSymbol(parser,'$',optional));
			CHECK(matchSymbol(parser,'-',optional));
			CHECK(parseRegister(parser,
			  (addrMode == SH_AMODE_ATMIRN) ? vars.rn : vars.rm));
      break;
      
    case SH_AMODE_ATD4RN: // @(disp:4,Rn)
    case SH_AMODE_ATD4RM:
			CHECK(matchSymbol(parser,'$',optional));
			CHECK(matchSymbol(parser,'(',optional));
			
      // displacement
			CHECK(parseImmediate(parser,vars.ImmediateExpression));
			vars.ImmediateBitLen = 4;
			vars.ImmediateSigned = false;
	    vars.ImmediateScale = opcode.datsize;
			
			CHECK(matchSymbol(parser,',',optional));
			
			// register
			CHECK(parseRegister(parser,
			  (addrMode == SH_AMODE_ATD4RN) ? vars.rn : vars.rm));
			
			CHECK(matchSymbol(parser,')',optional));
      break;
      
    case SH_AMODE_ATR0RN: // @(r0,Rn)
    case SH_AMODE_ATR0RM:
			CHECK(matchSymbol(parser,'$',optional));
			CHECK(matchSymbol(parser,'(',optional));
			
      CHECK(matchIdentifier(parser, "r0"));
      
			CHECK(matchSymbol(parser,',',optional));
			
			// register
			CHECK(parseRegister(parser,
			  (addrMode == SH_AMODE_ATR0RN) ? vars.rn : vars.rm));
			  
			CHECK(matchSymbol(parser,')',optional));
      break;
      
    case SH_AMODE_ATD8GBR: // @(d8,gbr)
			CHECK(matchSymbol(parser,'$',optional));
			CHECK(matchSymbol(parser,'(',optional));
			
      // displacement
			CHECK(parseImmediate(parser,vars.ImmediateExpression));
			vars.ImmediateBitLen = 8;
			vars.ImmediateSigned = false;
	    vars.ImmediateScale = opcode.datsize;
			
			CHECK(matchSymbol(parser,',',optional));
      CHECK(matchIdentifier(parser, "gbr"));
			CHECK(matchSymbol(parser,')',optional));
      
      break;
      
    case SH_AMODE_ATR0GBR: // @(r0,gbr)
			CHECK(matchSymbol(parser,'$',optional));
			CHECK(matchSymbol(parser,'(',optional));
      CHECK(matchIdentifier(parser, "r0"));
			CHECK(matchSymbol(parser,',',optional));
      CHECK(matchIdentifier(parser, "gbr"));
			CHECK(matchSymbol(parser,')',optional));
      break;
      
    case SH_AMODE_ATD8PC: // @(d8,pc)
			CHECK(matchSymbol(parser,'$',optional));
			CHECK(matchSymbol(parser,'(',optional));
			
	    vars.DisplacementTargetsAddr = true;
	    
			// check for optional '#' indicating a literal
			// that should be added to the current pool instead of an address
			if (parser.peekToken().type == TokenType::Hash) {
			  // flag so we know this is a pool literal
//	      vars.ImmediateIsPoolVal = true;
	      vars.DisplacementTargetsAddr = false;
			  parser.eatToken();
			}
			
      // displacement (or literal)
			CHECK(parseImmediate(parser,vars.ImmediateExpression));
			vars.ImmediateBitLen = 8;
			vars.ImmediateSigned = false;
	    vars.ImmediateScale = opcode.datsize;
	    // low 2 bits of pc are masked in this case (for longword ops)
	    vars.DisplacementPcMask = 0x3;
			
			CHECK(matchSymbol(parser,',',optional));
      CHECK(matchIdentifier(parser, "pc"));
			CHECK(matchSymbol(parser,')',optional));
      break;
      
    case SH_AMODE_D8: // @(d8,pc)
    case SH_AMODE_D12: // @(d12,pc)
      // displacement
			CHECK(parseImmediate(parser,vars.ImmediateExpression));
			vars.ImmediateBitLen = (addrMode == SH_AMODE_D8) ? 8 : 12;
			vars.ImmediateSigned = true;
	    vars.ImmediateScale = 2;
	    vars.DisplacementTargetsAddr = true;
      break;
      
    case SH_AMODE_IZ: // #i8 (zero-extended)
    case SH_AMODE_IS: // #i8 (sign-extended)
    case SH_AMODE_IZQ: // #i8 (zero-extended + quadrupled)
      // immediate
			CHECK(parseImmediate(parser,vars.ImmediateExpression));
			vars.ImmediateBitLen = 8;
			vars.ImmediateSigned = (addrMode == SH_AMODE_IS) ? true : false;
	    vars.ImmediateScale = (addrMode == SH_AMODE_IZQ) ? 4 : 0;
      break;
      
    default:
      // FIXME?
//      printf("bad\n");
      break;
    }
    
	}

	// the next token has to be a separator, else the parameters aren't
	// completely parsed
	return parser.nextToken().type == TokenType::Separator;
}

std::unique_ptr<CShInstruction> ShParser::parseShOpcode(Parser& parser)
{
	if (parser.peekToken().type != TokenType::Identifier)
		return nullptr;

	const Token &token = parser.nextToken();

	ShOpcodeVariables vars;
	bool paramFail = false;

//	const std::wstring stringValue = token.getStringValue();
	std::wstring stringValue = token.getStringValue();
	
	// HACK: under standard SH mnemonics, some instructions contain
	// a slash character, e.g. "bf/s" and "cmp/eq".
	// however, armips does not tokenize slashes into identifiers,
	// so in these cases, we can't get the whole thing at once
	// and must read the part after the slash separately.
	if ((parser.peekToken(0).type == TokenType::Div)
	    && (parser.peekToken(1).type == TokenType::Identifier)
	    && ((stringValue.compare(L"cmp") == 0)
	        || (stringValue.compare(L"bf") == 0)
	        || (stringValue.compare(L"bt") == 0))) {
    parser.eatToken();
    stringValue += L"/";
    stringValue += parser.nextToken().getStringValue();
  }
	
	for (int z = 0; ShOpcodes[z].name != nullptr; z++)
	{
	  // filter sh2 ops if in sh1 mode
		if ((ShOpcodes[z].flags & SH_SH2ONLY) && Sh.getVersion() == SARCH_SH1)
			continue;

    if (decodeShOpcode(stringValue,ShOpcodes[z],vars) == true)
    {
		  TokenizerPosition tokenPos = parser.getTokenizer()->getPosition();

		  if (parseShParameters(parser,ShOpcodes[z],vars) == true)
		  {
			  // success, return opcode
			  return make_unique<CShInstruction>(ShOpcodes[z],vars);
		  }

		  parser.getTokenizer()->setPosition(tokenPos);
		  paramFail = true;
		}
	}

  // FIXME: will not display correct op name for ops with "/"
	if (paramFail == true)
		parser.printError(token,L"SH parameter failure");
	else
		parser.printError(token,L"Invalid SH opcode");

	return nullptr;
}
