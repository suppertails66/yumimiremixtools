#pragma once
#include <unordered_map>
#include "Parser/DirectivesParser.h"
#include "Parser/Tokenizer.h"
#include "CShInstruction.h"

struct ShRegisterDescriptor {
	const wchar_t* name;
	int num;
};

class ShParser
{
public:
	std::unique_ptr<CAssemblerCommand> parseDirective(Parser& parser);
	std::unique_ptr<CShInstruction> parseShOpcode(Parser& parser);
private:
/*	bool parseRegisterTable(Parser& parser, ShRegisterValue& dest, const ShRegisterDescriptor* table, size_t count);
	bool parseCopRegister(Parser& parser, ShRegisterValue& dest);
	bool parseCopNumber(Parser& parser, ShRegisterValue& dest);
	bool parseRegisterList(Parser& parser, int& dest, int validMask);
	bool parseShift(Parser& parser, ShOpcodeVariables& vars, bool immediateOnly);
	bool parsePseudoShift(Parser& parser, ShOpcodeVariables& vars, int type);
	void parseWriteback(Parser& parser, bool& dest);
	void parsePsr(Parser& parser, bool& dest);
	void parseSign(Parser& parser, bool& dest);
	bool parsePsrTransfer(Parser& parser, ShOpcodeVariables& vars, bool shortVersion);
	
	int decodeCondition(const std::wstring& text, size_t& pos);
	bool decodeAddressingMode(const std::wstring& text, size_t& pos, unsigned char& dest);
	bool decodeXY(const std::wstring& text, size_t& pos, bool& dest);
	void decodeS(const std::wstring& text, size_t& pos, bool& dest); */
	
	bool parseImmediate(Parser& parser, Expression& dest);
	bool parseRegister(Parser& parser, ShRegisterValue& dest, int max = 15);
	
	int getAddressingMode(const char* str);
	
	bool matchSymbol(Parser& parser, wchar_t symbol, bool optional);
	bool matchIdentifier(Parser& parser, std::string str);
	
	bool decodeShOpcode(const std::wstring& name, const tShOpcode& opcode, ShOpcodeVariables& vars);
	
	bool parseShParameters(Parser& parser, const tShOpcode& opcode, ShOpcodeVariables& vars);
};
