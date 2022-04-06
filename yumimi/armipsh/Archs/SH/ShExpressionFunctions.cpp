#include "stdafx.h"
#include "Core/ExpressionFunctions.h"
#include "Core/Common.h"
#include "ShExpressionFunctions.h"
#include "Sh.h"

#define GET_PARAM(params,index,dest) \
	if (getExpFuncParameter(params,index,dest,funcName,false) == false) \
		return ExpressionValue();

// FIXME
ExpressionValue expFuncIsSh_FIXME(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	bool isSh = Arch == &Sh && Sh.GetThumbMode() == false;
	return ExpressionValue(isSh ? INT64_C(1) : INT64_C(0));
}

// FIXME
ExpressionValue expFuncIsThumb_FIXME(const std::wstring& funcName, const std::vector<ExpressionValue>& parameters)
{
	bool isThumb = Sh.GetThumbMode() == true;
	return ExpressionValue(isThumb ? INT64_C(1) : INT64_C(0));
}

// FIXME
const ExpressionFunctionMap shExpressionFunctions = {
	{ L"issh",			{ &expFuncIsSh_FIXME,			0,	0,	ExpFuncSafety::Safe } },
	{ L"isthumb",		{ &expFuncIsThumb_FIXME,			0,	0,	ExpFuncSafety::Safe } },
};
