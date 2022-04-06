#include "stdafx.h"
#include "Pool.h"
#include "Sh.h"
#include "Core/Common.h"
#include "Core/FileManager.h"

ShStateCommand::ShStateCommand(bool state)
{
	shstate = state;
}

bool ShStateCommand::Validate()
{
	RamPos = g_fileManager->getVirtualAddress();
	return false;
}

void ShStateCommand::writeSymData(SymbolData& symData) const
{
	// TODO: find a less ugly way to check for undefined memory positions
	if (RamPos == -1)
		return;

/*	if (shstate == true)
	{
		symData.addLabel(RamPos,L".sh");
	} else {
		symData.addLabel(RamPos,L".thumb");
	}*/
}


ShPoolCommand::ShPoolCommand()
{

}

bool ShPoolCommand::Validate()
{
	position = g_fileManager->getVirtualAddress();

	size_t oldSize = values.size();
	values.clear();

  int poolOffset = 0;
  // add pool content in the order bytes -> words -> longs
  for (int byteWPass = 1; byteWPass <= 4; byteWPass *= 2) {
	  for (ShPoolEntry& entry: Sh.getPoolContent())
	  {
	    if (entry.byteWidth != byteWPass) continue;
	    
		  size_t index = values.size();
		
		  // try to filter redundant values, but only if
		  // we aren't in an unordinarily long validation loop
		  if (Global.validationPasses < 10)
		  {
			  for (size_t i = 0; i < values.size(); i++)
			  {
				  if ((values[i].value == entry.value)
				      && (values[i].byteWidth == entry.byteWidth))
				  {
					  index = i;
					  break;
				  }
			  }
		  }

		  if (index == values.size())
		  {
		    // pad with dummy byte entries to needed boundary
		    // for this width
			  while ((position + poolOffset) % byteWPass != 0)
			  {
			    ShPoolEntry dummy = { nullptr, 0, 1, poolOffset };
			    values.push_back(dummy);
			    
			    ++poolOffset;
			    ++index;
			  }
			
		    entry.poolOffset = poolOffset;
			  values.push_back(entry);
			  poolOffset += byteWPass;
			}

		  entry.command->applyFileInfo();
		  entry.command->setPoolAddress(position + values[index].poolOffset,
		                                byteWPass);
	  }
	}

	Sh.clearPoolContent();
	g_fileManager->advanceMemory(poolOffset);

	return oldSize != values.size();
}

void ShPoolCommand::Encode() const
{
	for (size_t i = 0; i < values.size(); i++)
	{
		int32_t value = values[i].value;
		switch (values[i].byteWidth)
		{
		case 1:
		  g_fileManager->writeU8(value);
		  break;
		case 2:
		  g_fileManager->writeU16(value);
		  break;
		case 4:
		  g_fileManager->writeU32(value);
		  break;
		default:
		  break;
		}
	}
}

void ShPoolCommand::writeTempData(TempData& tempData) const
{
	for (size_t i = 0; i < values.size(); i++)
	{
		int32_t value = values[i].value;
//		tempData.writeLine(position+i*4,formatString(L".word 0x%08X",value));
    
    std::wstring fmtStr;
    switch (values[i].byteWidth)
    {
    case 1:
      fmtStr = L".byte 0x%02X";
      break;
    case 2:
      fmtStr = L".half 0x%04X";
      break;
    case 4:
      fmtStr = L".word 0x%08X";
      break;
    default:
      break;
    }
		tempData.writeLine(position + values[i].poolOffset,
		                   formatString(fmtStr.c_str(),value));
	}
}

void ShPoolCommand::writeSymData(SymbolData& symData) const
{
	if (values.size() != 0)
	{
		symData.addLabel(position,L".pool");
//		symData.addData(position,values.size()*4,SymbolData::Data32);
    const ShPoolEntry& lastEntry = values.back();
		symData.addData(position, lastEntry.poolOffset + lastEntry.byteWidth,
		                SymbolData::Data32);
	}
}
