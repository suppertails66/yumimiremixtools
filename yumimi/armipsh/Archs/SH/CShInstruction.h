#pragma once
#include "Commands/CAssemblerCommand.h"
#include "ShOpcodes.h"
#include "Sh.h"
#include "Core/Expression.h"

struct ShOpcodeVariables {
	struct {
		unsigned char c,a;
		bool s,x,y;
		unsigned int NewEncoding;
		char NewType;
		bool UseNewEncoding;
		bool UseNewType;
	} Opcode;

	struct {
		unsigned char Type;
		bool ShiftByRegister;
		bool UseShift;
		ShRegisterValue reg;
		Expression ShiftExpression;
		int ShiftAmount;
		unsigned char FinalType;
		int FinalShiftAmount;
		bool UseFinal;
	} Shift;

	struct {
		bool spsr;
		int field;
	} PsrData;

	struct {
		ShRegisterValue cd;	// cop register d
		ShRegisterValue cn;	// cop register n
		ShRegisterValue cm;	// cop register m
		ShRegisterValue pn;	// cop number
		Expression CpopExpression;	// cp opc number
		Expression CpinfExpression;	// cp information
		int Cpop;
		int Cpinf;
	} CopData;

	ShRegisterValue rs;
//	ShRegisterValue rm;
	ShRegisterValue rd;
//	ShRegisterValue rn;
	bool psr;
	bool writeback;
	bool SignPlus;
	bool negative;
//	Expression ImmediateExpression;
//	int Immediate;
//	int ImmediateBitLen;
//	int OriginalImmediate;
	int rlist;
	char RlistStr[64];
	
	// REMOVE everything above this line
	
	
	
	
	
	ShRegisterValue rn;
	ShRegisterValue rm;
	Expression ImmediateExpression;
	// Immediate is used for both d and i parameters
	// (they never occur simultaneously)
	int64_t Immediate;
	int64_t OriginalImmediate;
	int ImmediateBitLen;
	bool ImmediateSigned;
//	bool ImmediateIsPoolVal;
	int ImmediateScale;
	bool DisplacementTargetsAddr;
	int DisplacementPcMask;
	// size of operation in bytes,
	// i.e. .b/.w/.l represented numerically
//	int datSize;
	
};

class CShInstruction: public ShOpcodeCommand
{
public:
	CShInstruction(const tShOpcode& sourceOpcode, ShOpcodeVariables& vars);
//	~CShInstruction();
	bool Load(char* Name, char* Params);
	virtual bool Validate();
	virtual void Encode() const;
	virtual void writeTempData(TempData& tempData) const;
	virtual void setPoolAddress(int64_t address, int byteWidth);
private:
	void FormatOpcode(char* Dest, const char* Source) const;
	void FormatInstruction(const char* encoding, char* dest) const;
//	int getShiftedImmediate(unsigned int num, int& ShiftAmount);

	ShOpcodeVariables Vars;
	tShOpcode Opcode;
	int64_t RamPos;
	ShArchType arch;
};
