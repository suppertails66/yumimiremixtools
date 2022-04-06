#pragma once
#include "Pool.h"

#define SH_SHIFT_LSL		0x00
#define SH_SHIFT_LSR		0x01
#define SH_SHIFT_ASR		0x02
#define SH_SHIFT_ROR		0x03
#define SH_SHIFT_RRX		0x04

enum ShArchType { SARCH_SH1 = 0, SARCH_SH2, SARCH_INVALID };

typedef struct {
	const char* name;
	short num;
	short len;
} tShRegister;

typedef struct {
	char Name[4];
	int Number;
} tShRegisterInfo;

struct ShRegisterValue
{
	std::wstring name;
	int num;
};

extern const tShRegister ShRegister[];

class CShArchitecture: public CArchitecture
{
public:
	CShArchitecture();
	~CShArchitecture();
	void clear();

	virtual std::unique_ptr<CAssemblerCommand> parseDirective(Parser& parser);
	virtual std::unique_ptr<CAssemblerCommand> parseOpcode(Parser& parser);
	virtual const ExpressionFunctionMap& getExpressionFunctions();
	virtual void NextSection();
	virtual void Pass2();
	virtual void Revalidate();
	virtual std::unique_ptr<IElfRelocator> getElfRelocator();
	virtual Endianness getEndianness() { return Endianness::Big; };
	void SetThumbMode(bool b) { thumb = b; };
	bool GetThumbMode() { return thumb; };
	void setVersion(ShArchType type) { version = type; }
	ShArchType getVersion() { return version; }

	std::vector<ShPoolEntry> getPoolContent() { return currentPoolContent; }
	void clearPoolContent() { currentPoolContent.clear(); }
	void addPoolValue(ShOpcodeCommand* command, int32_t value, int byteWidth);
private:
	bool thumb;
	ShArchType version;

	std::vector<ShPoolEntry> currentPoolContent;
};

class ShOpcodeCommand: public CAssemblerCommand
{
public:
	virtual void setPoolAddress(int64_t address, int byteWidth) = 0;
};

extern CShArchitecture Sh;
