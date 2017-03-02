#include "hsmoPrecompiled.h"
#include "hsmoHriBlock.h"

using namespace std;
using namespace xsc;

namespace hsmo {

const wchar_t* COMPARISON_OPERATOR_CODES[NUMBER_OF_COMPARISON_OPERATORS] =
{
	L"==",
	L"!=",
	L">",
	L">=",
	L"<",
	L"<="
};

const wchar_t* HRI_EVENT_CODES[NUMBER_OF_HRI_EVENTS] =
{
	L"e:head-touched",
	L"e:l-hand-touched",
	L"e:r-hand-touched",
	L"e:start-speech",
	L"e:end-speech",
	L"e:silency-detected",
	L"e:speed-recognized",
	L"e:loud-sound-detected",
	L"e:face-detected",
	L"e:face-disappeared",
	L"e:face-recognized",
	L"e:person-identified",
	L"e:person-engaged",
	L"e:person-disengaged",
	L"e:lip-talking-started",
	L"e:lip-talking-finished",
	L"e:intention-affirmation",
	L"e:intention-negation",
	L"e:intention-attention",
	L"e:number-of-guests-changed",
	L"e:pointing-gesture-recognized",
	L"e:person-approached",
	L"e:person-left",
	L"e:person-entered-zone",
	L"e:saliency-changed",
	L"e:environment-detection",
	L"e:top-up",
	L"e:front-up",
	L"e:rear-up",
	L"e:right-up",
	L"e:left-up",
	L"e:bottom-up"
};

XSC_IMPLEMENT_DYNAMIC(hsmo, HriBlock, Block)

HriBlock::HriBlock()
{
}

HriBlock::~HriBlock()
{
}

bool HriBlock::GenerateCode(wstring& code, int level)
{
	// to be overriden
	return true;
}

wstring HriBlock::GetEntryCode() const
{
	// to be overriden
	return L"";
}

} // namespace hsmo
