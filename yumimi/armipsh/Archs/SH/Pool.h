#pragma once
#include "Commands/CAssemblerCommand.h"
#include "../Architecture.h"

class ShStateCommand: public CAssemblerCommand
{
public:
	ShStateCommand(bool state);
	virtual bool Validate();
	virtual void Encode() const { };
	virtual void writeTempData(TempData& tempData) const { };
	virtual void writeSymData(SymbolData& symData) const;
private:
	int64_t RamPos;
	bool shstate;
};

class ShOpcodeCommand;

struct ShPoolEntry
{
	ShOpcodeCommand* command;
	int32_t value;
	int byteWidth;
	int poolOffset;
};

class ShPoolCommand: public CAssemblerCommand
{
public:
	ShPoolCommand();
	virtual bool Validate();
	virtual void Encode() const;
	virtual void writeTempData(TempData& tempData) const;
	virtual void writeSymData(SymbolData& symData) const;
private:
	int64_t position;
	// we need to hold on to the byte size info,
	// so just the raw values aren't enough here
//	std::vector<int32_t> values;
	std::vector<ShPoolEntry> values;
};
