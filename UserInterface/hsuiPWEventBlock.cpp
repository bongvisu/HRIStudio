#include "hsuiPrecompiled.h"
#include "hsuiPWEventBlock.h"
#include "hsuiVisualManager.h"
#include "hsuiEditCtrl.h"
#include "hsuiResource.h"

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static ParamWindow* CreatePWEventBlock(Component* component)
{
	return new PWEventBlock(component);
}

void PWEventBlock::InitPWEventBlock()
{
	msFactory[XSC_RTTI(EventBlock).GetName()] = CreatePWEventBlock;
}

void PWEventBlock::TermPWEventBlock()
{
}
// ====================================================================================================================

PWEventBlock::PWEventBlock(Component* component)
	:
	ParamWindow(component)
{
}

PWEventBlock::~PWEventBlock()
{
}

void PWEventBlock::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"EVENT - %s", mComponent->GetName().c_str());
}

void PWEventBlock::GetDimension(int& w, int& h) const
{
	w = 400;
	h = 600;
}

void PWEventBlock::OnInitWnd()
{
	auto block = static_cast<EventBlock*>(mComponent);

	int x = CONTENT_MARGIN + 1;
	int y = TOP_MARGIN + 1;

	mEvents.CreateWnd(this, x, y, 384, 561);
	UINT resIDs[] = { HSUI_IDR_EVENT_CATEGORY1_ICON, HSUI_IDR_EVENT_CATEGORY2_ICON, HSUI_IDR_EVENT_ITEM_ICON };
	mEvents.BuildImageList(Module::handle, 3, resIDs);
	mEvents.ShowHorzScrollBar(false);
	mEvents.SetFocus();

	TreeCtrl::Item item;
	int categoryHandle1, categoryHandle2;

	item.text = L"Social";
	item.image = 0;
	categoryHandle1 = mEvents.InsertItem(item);
		
		item.image = 1;
		item.text = L"Touch Activity";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);
			
			item.image = 2;
			
			item.text = L"Head Touched";
			item.data = reinterpret_cast<void*>(HEAD_TOUCHED);
			mEventToItem[HEAD_TOUCHED] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Left Hand Touched";
			item.data = reinterpret_cast<void*>(L_HAND_TOUCHED);
			mEventToItem[L_HAND_TOUCHED] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Right Hand Touched";
			item.data = reinterpret_cast<void*>(R_HAND_TOUCHED);
			mEventToItem[R_HAND_TOUCHED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Voice Activity";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Start Speech";
			item.data = reinterpret_cast<void*>(START_SPEECH);
			mEventToItem[START_SPEECH] = mEvents.InsertItem(item, categoryHandle2);
			
			item.text = L"End Speech";
			item.data = reinterpret_cast<void*>(END_SPEECH);
			mEventToItem[END_SPEECH] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Silency Detected";
			item.data = reinterpret_cast<void*>(SILENCY_DETECTED);
			mEventToItem[SILENCY_DETECTED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Speech Recognition";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Speech Recognized";
			item.data = reinterpret_cast<void*>(SPEECH_RECOGNIZED);
			mEventToItem[SPEECH_RECOGNIZED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Sound Localization";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Loud Sound Detected";
			item.data = reinterpret_cast<void*>(LOUD_SOUND_DETECTED);
			mEventToItem[LOUD_SOUND_DETECTED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Face Detection";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Face Detected";
			item.data = reinterpret_cast<void*>(FACE_DETECTED);
			mEventToItem[FACE_DETECTED] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Face Disappeared";
			item.data = reinterpret_cast<void*>(FACE_DISAPPEARED);
			mEventToItem[FACE_DISAPPEARED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Face Recognition";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Face Recognized";
			item.data = reinterpret_cast<void*>(FACE_RECOGNIZED);
			mEventToItem[FACE_RECOGNIZED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Person Identification";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Person Identified";
			item.data = reinterpret_cast<void*>(PERSON_IDENTIFIED);
			mEventToItem[PERSON_IDENTIFIED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Engagement Detection";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Person Engaged";
			item.data = reinterpret_cast<void*>(PERSON_ENGAGED);
			mEventToItem[PERSON_ENGAGED] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Person Disengaged";
			item.data = reinterpret_cast<void*>(PERSON_DISENGAGED);
			mEventToItem[PERSON_DISENGAGED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Lip Reading";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Lip Talking Started";
			item.data = reinterpret_cast<void*>(LIP_TALKING_STARTED);
			mEventToItem[LIP_TALKING_STARTED] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Lip Talking Finished";
			item.data = reinterpret_cast<void*>(LIP_TALKING_FINISHED);
			mEventToItem[LIP_TALKING_FINISHED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Intention Recognition";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Intention Affirmation";
			item.data = reinterpret_cast<void*>(INTENTION_AFFIRMATION);
			mEventToItem[INTENTION_AFFIRMATION] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Intention Negation";
			item.data = reinterpret_cast<void*>(INTENTION_NEGATION);
			mEventToItem[INTENTION_NEGATION] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Intention Attention";
			item.data = reinterpret_cast<void*>(INTENTION_ATTENTION);
			mEventToItem[INTENTION_ATTENTION] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"People Detection";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Number of Guests Changed";
			item.data = reinterpret_cast<void*>(NUMBER_OF_GUESTS_CHANGED);
			mEventToItem[NUMBER_OF_GUESTS_CHANGED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Pointing Gesture Recognition";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Pointing Gesture Recognized";
			item.data = reinterpret_cast<void*>(POITING_GESTURE_RECOGNIZED);
			mEventToItem[POITING_GESTURE_RECOGNIZED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Presence Detection";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Person Approached";
			item.data = reinterpret_cast<void*>(PERSON_APPROACHED);
			mEventToItem[PERSON_APPROACHED] = mEvents.InsertItem(item, categoryHandle2);

			item.text = L"Person Left";
			item.data = reinterpret_cast<void*>(PERSON_LEFT);
			mEventToItem[PERSON_LEFT] = mEvents.InsertItem(item, categoryHandle2);
			
		item.image = 1;
		item.text = L"Proxemics Detection";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Person Entered Zone";
			item.data = reinterpret_cast<void*>(PERSON_ENTERED_ZONE);
			mEventToItem[PERSON_ENTERED_ZONE] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Saliency Detection";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Saliency Changed";
			item.data = reinterpret_cast<void*>(SALIENCY_CHANGED);
			mEventToItem[SALIENCY_CHANGED] = mEvents.InsertItem(item, categoryHandle2);

		item.image = 1;
		item.text = L"Environment Detection";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Environment Detection";
			item.data = reinterpret_cast<void*>(ENVIRONMENT_DETECTION);
			mEventToItem[ENVIRONMENT_DETECTION] = mEvents.InsertItem(item, categoryHandle2);

	item.image = 0;
	item.text = L"System";
	categoryHandle1 = mEvents.InsertItem(item);

		item.image = 1;
		item.text = L"Battery";
		categoryHandle2 = mEvents.InsertItem(item, categoryHandle1);

			item.image = 2;

			item.text = L"Battery Loow Detected";
			item.data = reinterpret_cast<void*>(BATTERY_LOW_DETECTED);
			mEventToItem[BATTERY_LOW_DETECTED] = mEvents.InsertItem(item, categoryHandle2);

	mEvents.SetSelectedItem(mEventToItem[block->GetHriEvent()]);
}

void PWEventBlock::OnApply()
{
	auto block = static_cast<EventBlock*>(mComponent);

	auto selectedIem = mEvents.GetSelectedItem();
	if (mEvents.IsLeaf(selectedIem))
	{
		auto itemData = mEvents.GetItemData(selectedIem);
		block->SetHriEvent(static_cast<HriEvent>(reinterpret_cast<int64_t>(itemData)));
	}
}

void PWEventBlock::OnPaintWnd()
{
	CRect wndRect;
	mEvents.GetWindowRect(&wndRect);
	ScreenToClient(&wndRect);

	D2D1_RECT_F rect;
	rect.left = static_cast<FLOAT>(wndRect.left);
	rect.top = static_cast<FLOAT>(wndRect.top);
	rect.right = wndRect.right + 1.0f;
	rect.bottom = wndRect.bottom + 1.0f;

	mBrush->SetColor(VisualManager::CLR_LEVEL3);
	mRenderer.DrawRectangle(rect, mBrush);
}

} // namespace hsui
