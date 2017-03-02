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
	L"e:head_touched",
	L"e:l_hand_touched",
	L"e:r_hand_touched",
	L"e:start_speech",
	L"e:end_speech",
	L"e:silency_detected",
	L"e:speed_recognized",
	L"e:loud_sound_detected",
	L"e:face_detected",
	L"e:face_disappeared",
	L"e:face_recognized",
	L"e:person_identified",
	L"e:person_engaged",
	L"e:person_disengaged",
	L"e:lip_talking_started",
	L"e:lip_talking_finished",
	L"e:intention_affirmation",
	L"e:intention_negation",
	L"e:intention_attention",
	L"e:number_of_guests_changed",
	L"e:pointing_gesture_recognized",
	L"e:person_approached",
	L"e:person_left",
	L"e:person_entered_zone",
	L"e:saliency_changed",
	L"e:environment_detection",
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
