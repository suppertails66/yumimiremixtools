#include "stdafx.h"
#include "Sh.h"
#include "Core/Common.h"
// FIXME
//#include "ShElfRelocator.h"
#include "ShParser.h"
#include "ShExpressionFunctions.h"

CShArchitecture Sh;

CShArchitecture::CShArchitecture()
{
	clear();
}

CShArchitecture::~CShArchitecture()
{
	clear();
}

std::unique_ptr<CAssemblerCommand> CShArchitecture::parseDirective(Parser& parser)
{
	ShParser shParser;

	return shParser.parseDirective(parser);
}

std::unique_ptr<CAssemblerCommand> CShArchitecture::parseOpcode(Parser& parser)
{
	ShParser shParser;

	return shParser.parseShOpcode(parser);
}

const ExpressionFunctionMap& CShArchitecture::getExpressionFunctions()
{
	return shExpressionFunctions;
}

void CShArchitecture::clear()
{
	currentPoolContent.clear();
	thumb = false;
}

void CShArchitecture::Pass2()
{
	currentPoolContent.clear();
}

void CShArchitecture::Revalidate()
{
	for (ShPoolEntry& entry: currentPoolContent)
	{
		entry.command->applyFileInfo();
		Logger::queueError(Logger::Error,L"Unable to find literal pool");
	}

	currentPoolContent.clear();
}

void CShArchitecture::NextSection()
{

}

std::unique_ptr<IElfRelocator> CShArchitecture::getElfRelocator()
{
  // FIXME
//	return make_unique<ShElfRelocator>(version != AARCH_GBA);
  return nullptr;
}

void CShArchitecture::addPoolValue(ShOpcodeCommand* command, int32_t value,
                                   int byteWidth)
{
	ShPoolEntry entry;
	entry.command = command;
	entry.value = value;
	entry.byteWidth = byteWidth;

	currentPoolContent.push_back(entry);
}
