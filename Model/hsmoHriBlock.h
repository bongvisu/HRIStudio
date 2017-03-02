#pragma once

#include <xscBlock.h>
#include <hsmoModule.h>

namespace hsmo {

enum ComparisonOperator
{
	COMPARISON_OPERATOR_EQUAL_TO = 0,
	COMPARISON_OPERATOR_NOT_EQUAL_TO,
	COMPARISON_OPERATOR_GREATER_THAN,
	COMPARISON_OPERATOR_GREATER_THAN_OR_EQUAL_TO,
	COMPARISON_OPERATOR_LESS_THAN,
	COMPARISON_OPERATOR_LESS_THAN_OR_EQUAL_TO,
	NUMBER_OF_COMPARISON_OPERATORS
};

enum OperandType
{
	OPERAND_TYPE_VARIABLE = 0,
	OPERAND_TYPE_VALUE,
	OPERAND_TYPE_OTHER
};

enum HriEvent
{
	// Social
	// Touch Activity
	HEAD_TOUCHED = 0,
	L_HAND_TOUCHED,
	R_HAND_TOUCHED,

	// Voice Activity
	START_SPEECH,
	END_SPEECH,
	SILENCY_DETECTED,

	// Speech Recognition
	SPEECH_RECOGNIZED,

	// Sound Localization
	LOUD_SOUND_DETECTED,

	// Face Detection
	FACE_DETECTED,
	FACE_DISAPPEARED,

	// Face Recognition
	FACE_RECOGNIZED,

	// Person Identification
	PERSON_IDENTIFIED,

	// Engagement Detection
	PERSON_ENGAGED,
	PERSON_DISENGAGED,

	// Lip Reading
	LIP_TALKING_STARTED,
	LIP_TALKING_FINISHED,

	// Intention Recognition
	INTENTION_AFFIRMATION,
	INTENTION_NEGATION,
	INTENTION_ATTENTION,

	// People Detection
	NUMBER_OF_GUESTS_CHANGED,

	// Pointing Gesture Recognition
	POITING_GESTURE_RECOGNIZED,

	// Presence Detection
	PERSON_APPROACHED,
	PERSON_LEFT,

	// Proxemics Detection
	PERSON_ENTERED_ZONE,

	// Saliency Detection
	SALIENCY_CHANGED,

	// Environment Detection
	ENVIRONMENT_DETECTION,

	// System
	// Battery
	BATTERY_LOW_DETECTED,

	// Motion Changed
	TOP_UP,
	FRONT_UP,
	REAR_UP,
	RIGHT_UP,
	LEFT_UP,
	BOTTOM_UP,

	NUMBER_OF_HRI_EVENTS
};

extern HSMO_API const wchar_t* COMPARISON_OPERATOR_CODES[NUMBER_OF_COMPARISON_OPERATORS];
extern HSMO_API const wchar_t* HRI_EVENT_CODES[NUMBER_OF_HRI_EVENTS];

class HriBlock;
typedef xsc::Pointer<HriBlock> HriBlockPtr;
typedef std::list<HriBlock*> HriBlockList;
typedef std::vector<HriBlock*> HriBlockVec;
typedef std::unordered_set<HriBlock*> HriBlockSet;

class HSMO_API HriBlock : public xsc::Block
{
	XSC_DECLARE_DYNAMIC(HriBlock)

public:
	virtual ~HriBlock();

	virtual std::wstring GetEntryCode() const;
	virtual bool GenerateCode(std::wstring& code, int level);

protected:
	HriBlock();
};

} // namespace hsmo
