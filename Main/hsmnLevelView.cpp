#include "hsmnPrecompiled.h"
#include "hsmnLevelView.h"
#include "hsmnLevelPanel.h"
#include "hsmnApplication.h"
#include "hsmnMainFrame.h"

#include <hsmoModelManager.h>
#include <hsmoModuleBlock.h>
#include <hsmoAddBlockCommand.h>
#include <hsmoRemoveCommand.h>
#include <hsmoCutCommand.h>
#include <hsmoCopyCommand.h>
#include <hsmoPasteCommand.h>
#include <hsmoAlignBlockCommand.h>
#include <hsmoEqualizeBlockCommand.h>
#include <hsmoRotateBlockCommand.h>
#include <hsprLinkPresenter.h>
#include <hsprBlockPresenter.h>
#include <hsuiPropManager.h>
#include <hsuiParamManager.h>
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hspr;
using namespace hsui;

namespace hsmn {

static const int WORLD_CONTENT_MARGIN = 50;

BEGIN_MESSAGE_MAP(LevelView, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CAPTURECHANGED()

	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()

	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)

	ON_COMMAND(HSMN_IDC_FORMAT_ALIGN_LEFT, OnFormatAlignLeft)
	ON_COMMAND(HSMN_IDC_FORMAT_ALIGN_RIGHT, OnFormatAlignRight)
	ON_COMMAND(HSMN_IDC_FORMAT_ALIGN_TOP, OnFormatAlignTop)
	ON_COMMAND(HSMN_IDC_FORMAT_ALIGN_BOTTOM, OnFormatAlignBottom)
	ON_COMMAND(HSMN_IDC_FORMAT_MAKE_SAME_SIZE, OnFormatMakeSameSize)
	ON_COMMAND(HSMN_IDC_FORMAT_MAKE_SAME_WIDTH, OnFormatMakeSameWidth)
	ON_COMMAND(HSMN_IDC_FORMAT_MAKE_SAME_HEIGHT, OnFormatMakeSameHeight)
	ON_COMMAND(HSMN_IDC_FORMAT_ROTATE_BLOCK, OnFormatRotateBlock)

	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

LevelView::LevelView(Level* level, CWnd* parent)
	:
	mLevel(level)
{
	mDragHandlers[DRAG_HANDLER_TYPE_ADD_LINK]			= std::make_unique<DHAddLink>(this);
	mDragHandlers[DRAG_HANDLER_TYPE_RESIZE_BLOCK]		= std::make_unique<DHResizeBlock>(this);
	mDragHandlers[DRAG_HANDLER_TYPE_SELECT_GROUP]		= std::make_unique<DHSelectGroup>(this);
	mDragHandlers[DRAG_HANDLER_TYPE_MOVE_COMPONENTS]	= std::make_unique<DHMoveComponents>(this);

	mDragHandler = nullptr;
	mTrackingMouseLeave = false;
	mDeleteKeyDown = false;
	mActiveAddition = false;

	mHotPresenter = nullptr;
	mPrimaryBlock = nullptr;

	mPanel = dynamic_cast<LevelPanel*>(parent);
	mViewSize = { 8, 8 };

	CRect rect(0, 0, 8, 8);
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	Create(nullptr, nullptr, style, rect, parent, 0, nullptr);
}

LevelView::~LevelView()
{
}

void LevelView::OnChildAddition(const ComponentSet& components)
{
	if (mActiveAddition)
	{
		ClearSelection();
	}

	for (const auto component : components)
	{
		if (component->IsDerivedFrom(XSC_RTTI(Block)))
		{
			auto block = static_cast<Block*>(component);
			auto presenter = BlockPresenter::CreateDynamicInstance(block, static_cast<ILevelView*>(this));
			mPresenters[block] = presenter;

			if (mActiveAddition)
			{
				InsertSelection(presenter);
			}
		}
		else
		{
			assert(component->IsDerivedFrom(XSC_RTTI(Link)));
			auto link = static_cast<Link*>(component);
			auto presenter = new LinkPresenter(link, static_cast<ILevelView*>(this));
			mPresenters[link] = presenter;

			if (mActiveAddition)
			{
				mTempPresenterVec.clear();
				presenter->GetChildren(mTempPresenterVec);
				for (auto elementPresenter : mTempPresenterVec)
				{
					InsertSelection(elementPresenter);
				}
			}
		}
	}

	if (mActiveAddition)
	{
		ReportActiveSelections();
	}
}

void LevelView::OnChildRemoval(const ComponentSet& components)
{
	for (auto component : components)
	{
		assert(component->IsDerivedFrom(XSC_RTTI(Block)) || component->IsDerivedFrom(XSC_RTTI(Link)));

		ComponentToPresenter::const_iterator iter;
		iter = mPresenters.find(component);
		assert(iter != mPresenters.end());

		if (iter->second == mHotPresenter)
		{
			mHotPresenter = nullptr;
		}
		if (iter->second == mPrimaryBlock)
		{
			mPrimaryBlock = nullptr;
		}
		mSelectedPresenters.erase(iter->second);

		mTempPresenterVec.clear();
		iter->second->GetChildren(mTempPresenterVec);
		for (auto childPresenter : mTempPresenterVec)
		{
			if (childPresenter == mHotPresenter)
			{
				mHotPresenter = nullptr;
			}
			if (childPresenter == mPrimaryBlock)
			{
				mPrimaryBlock = nullptr;
			}
			mSelectedPresenters.erase(childPresenter);
		}

		mPresenters.erase(iter);
	}

	mDragHandler = nullptr;
}

void LevelView::InvalidateInteractionFor(Presenter* const presenter)
{
	if (presenter == mHotPresenter)
	{
		mHotPresenter = nullptr;
	}
	if (presenter == mPrimaryBlock)
	{
		mPrimaryBlock = nullptr;
	}
	mSelectedPresenters.erase(presenter);
	
	mDragHandler = nullptr;
}

void LevelView::UpdateFrameTitle()
{
	CFrameWnd* levelFrame = GetParentFrame();
	CString title = L" ";
	title += mLevel->GetName().c_str();
	title += L" ";
	levelFrame->SetWindowTextW(title);
}

void LevelView::OnPropertyChange(Entity::PropID prop)
{
	if (Component::PROP_NAME == prop)
	{
		UpdateFrameTitle();
	}
}

void LevelView::OnNodalGeometryChange()
{
	UpdateBound();
	UpdateScrollParameters();
}

void LevelView::UpdateBound()
{
	mBound.MakeInvalid();

	for (const auto& element : mPresenters)
	{
		mBound.Combine(element.second->GetBound());
	}
}

double LevelView::GetAppTime() const
{
	return theApp.GetAppTime();
}

void LevelView::OnBlockDrop(const wchar_t* rttiName, CPoint point)
{
	ScreenToClient(&point);
	MPVector worldPoint(point.x, point.y);
	worldPoint = ViewToWorld(worldPoint);

	auto command = new AddBlockCommand(mLevel, rttiName, worldPoint);
	auto model = static_cast<Model*>(mLevel->GetModel());

	mActiveAddition = true;
	model->ExecuteCommand(command);
	mActiveAddition = false;
}

BOOL LevelView::PreCreateWindow(CREATESTRUCT& cs) 
{
	CWnd::PreCreateWindow(cs);

	cs.style &= ~WS_BORDER;

	cs.lpszClass = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, 
		LoadCursorW(theApp.m_hInstance, MAKEINTRESOURCE(HSMN_IDR_LEVEL_VIEW_CURSOR)),
		reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

void LevelView::InitPresenters()
{
	const BlockList& blocks = mLevel->GetBlocks();
	for (const auto block : blocks)
	{
		auto blockPresenter = BlockPresenter::CreateDynamicInstance(block, static_cast<ILevelView*>(this));
		mPresenters[block] = blockPresenter;
	}

	const LinkList& links = mLevel->GetLinks();
	for (const auto& link : links)
	{
		auto linkPresenter = new LinkPresenter(link, static_cast<ILevelView*>(this));
		mPresenters[link] = linkPresenter;
	}

	UpdateBound();
}

int LevelView::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	UpdateFrameTitle();

	mRenderer.Create(m_hWnd, 8, 8);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	InitPresenters();

	theApp.RegisterLoopConsumer(static_cast<ILoopConsumer*>(this));
	mLevel->RegisterListener(static_cast<IComponentListener*>(this));

	return 0;
}

void LevelView::OnDestroy()
{
	mLevel->UnregisterListener(static_cast<IComponentListener*>(this));
	theApp.UnregisterLoopConsumer(static_cast<ILoopConsumer*>(this));

	mPresenters.clear();

	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void LevelView::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);

	mViewSize.x = cx;
	mViewSize.y = cy;
	
	mRenderer.Resize(cx, cy);
	
	UpdateScrollParameters();
}

void LevelView::UpdateHotPresenter(const CPoint& viewPoint)
{
	if (mHotPresenter)
	{
		auto visual = mHotPresenter->GetVisualInterface();
		assert(visual);
		visual->SetHot(false);
		mHotPresenter = nullptr;
	}

	MPVector point(ViewToWorld({ viewPoint.x, viewPoint.y }));

	const BlockList& blocks = mLevel->GetBlocks();
	BlockList::const_reverse_iterator bit;
	for (bit = blocks.rbegin(); bit != blocks.rend(); ++bit)
	{
		mHotPresenter = mPresenters[*bit]->FindHit(point);
		if (mHotPresenter) 
		{
			auto visual = mHotPresenter->GetVisualInterface();
			assert(visual);
			visual->SetHot(true);
			return;
		}
	}

	const LinkList& links = mLevel->GetLinks();
	LinkList::const_reverse_iterator lit;
	for (lit = links.rbegin(); lit != links.rend(); ++lit)
	{
		mHotPresenter = mPresenters[*lit]->FindHit(point);
		if (mHotPresenter) 
		{
			auto visual = mHotPresenter->GetVisualInterface();
			assert(visual);
			visual->SetHot(true);
			return;
		}
	}
}

void LevelView::OnMouseMove(UINT flags, CPoint point)
{
	CWnd::OnMouseMove(flags, point);
	mCursorPos = point;

	MPVector pointerPos = ViewToWorld({ point.x, point.y });
	mPanel->UpdatePointerInfo(pointerPos.x, pointerPos.y);

	if (mTrackingMouseLeave == false)
	{
		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = m_hWnd;
		TrackMouseEvent(&trackMouseEvent);

		mTrackingMouseLeave = true;
	}

	if (mDragHandler)
	{
		mDragHandler->OnMove(point);
	}
	else
	{
		UpdateHotPresenter(point);
	}
}

void LevelView::OnMouseLeave()
{
	mTrackingMouseLeave = false;

	if (mHotPresenter)
	{
		auto visual = mHotPresenter->GetVisualInterface();
		assert(visual);
		visual->SetHot(false);
		mHotPresenter = nullptr;
	}
}

void LevelView::OnRButtonDown(UINT flags, CPoint point)
{
	CWnd::OnRButtonDown(flags, point);
	SetFocus();

	mDragHandler = nullptr;
}

void LevelView::InsertSelection(Presenter* presenter)
{
	auto selectable = presenter->GetSelectableInterface();
	assert(selectable);
	selectable->SetSelected(true);
	mSelectedPresenters.insert(presenter);
}

void LevelView::RemoveSelection(Presenter* presenter)
{
	auto selectable = presenter->GetSelectableInterface();
	assert(selectable);
	selectable->SetSelected(false);
	mSelectedPresenters.erase(presenter);

	if (presenter == mPrimaryBlock)
	{
		auto alignable = mPrimaryBlock->GetAlignableInterface();
		alignable->SetPrimary(false);
		mPrimaryBlock = nullptr;
	}
}

void LevelView::ClearSelection()
{
	for (const auto element : mSelectedPresenters)
	{
		auto selectable = element->GetSelectableInterface();
		assert(selectable);
		selectable->SetSelected(false);
	}
	mSelectedPresenters.clear();

	if (mPrimaryBlock)
	{
		auto alignable = mPrimaryBlock->GetAlignableInterface();
		assert(alignable);
		alignable->SetPrimary(false);
		mPrimaryBlock = nullptr;
	}
}

void LevelView::ResetPrimary(Presenter* newPrimary)
{
	if (newPrimary != mPrimaryBlock)
	{
		if (mPrimaryBlock)
		{
			auto alignable = mPrimaryBlock->GetAlignableInterface();
			assert(alignable);
			alignable->SetPrimary(false);
		}

		mPrimaryBlock = newPrimary;
		
		if (mPrimaryBlock)
		{
			auto alignable = mPrimaryBlock->GetAlignableInterface();
			assert(alignable);
			alignable->SetPrimary(true);
		}
	}
}

void LevelView::ReportActiveSelections()
{
	ComponentSet targets;
	targets.reserve(mSelectedPresenters.size());
	for (auto presenter : mSelectedPresenters)
	{
		targets.insert(presenter->GetComponent());
	}
	thePropManager->SetTargets(std::move(targets));
}

void LevelView::OnLButtonDown(UINT flags, CPoint point)
{
	CWnd::OnLButtonDown(flags, point);

	SetFocus();
	SetCapture();

	UpdateHotPresenter(point);

	if (nullptr == mHotPresenter)
	{
		ClearSelection();

		ReportActiveSelections();
		mDragHandler = mDragHandlers[DRAG_HANDLER_TYPE_SELECT_GROUP].get();
		mDragHandler->Enter(point);
		return;
	}

	SHORT shifted = GetKeyState(VK_SHIFT) & 0x8000;
	SHORT controlled = GetKeyState(VK_CONTROL) & 0x8000;

	if (mHotPresenter->GetPresenterType() == PRESENTER_TYPE_BLOCK)
	{
		auto selectable = mHotPresenter->GetSelectableInterface();
		assert(selectable);
		if (controlled)
		{
			if (selectable->GetSelected())
			{
				RemoveSelection(mHotPresenter);
			}
			else
			{
				InsertSelection(mHotPresenter);
				ResetPrimary(mHotPresenter);
			}

			ReportActiveSelections();
			mDragHandler = nullptr;
		}
		else
		{
			if (false == selectable->GetSelected())
			{
				ClearSelection();
				InsertSelection(mHotPresenter);
			}
			ResetPrimary(mHotPresenter);

			ReportActiveSelections();
			mDragHandler = mDragHandlers[DRAG_HANDLER_TYPE_MOVE_COMPONENTS].get();
			mDragHandler->Enter(point);
		}
	}
	else if (mHotPresenter->GetPresenterType() == PRESENTER_TYPE_PORT)
	{
		Presenter* block = mHotPresenter->GetParent();
		PortPresenter* port = static_cast<PortPresenter*>(mHotPresenter);

		if (port->IsUnoccupied())
		{
			ClearSelection();
			InsertSelection(block);
			ResetPrimary(block);

			ReportActiveSelections();
			mDragHandler = mDragHandlers[DRAG_HANDLER_TYPE_ADD_LINK].get();
			mDragHandler->Enter(point);
		}
		else
		{
			auto selectable = block->GetSelectableInterface();
			assert(selectable);
			if (false == selectable->GetSelected())
			{
				ClearSelection();
				InsertSelection(block);
			}
			ResetPrimary(block);

			ReportActiveSelections();
			mDragHandler = mDragHandlers[DRAG_HANDLER_TYPE_MOVE_COMPONENTS].get();
			mDragHandler->Enter(point);
		}
	}
	else if (mHotPresenter->GetPresenterType() == PRESENTER_TYPE_CORNER)
	{
		Presenter* block = mHotPresenter->GetParent();

		ClearSelection();
		InsertSelection(block);
		ResetPrimary(block);
		
		ReportActiveSelections();
		mDragHandler = mDragHandlers[DRAG_HANDLER_TYPE_RESIZE_BLOCK].get();
		mDragHandler->Enter(point);
	}
	else if (mHotPresenter->GetPresenterType() == PRESENTER_TYPE_JOINT)
	{
		auto selectable = mHotPresenter->GetSelectableInterface();
		assert(selectable);
		if (controlled)
		{
			if (selectable->GetSelected())
			{
				RemoveSelection(mHotPresenter);
			}
			else
			{
				InsertSelection(mHotPresenter);
			}

			ReportActiveSelections();
			mDragHandler = nullptr;
		}
		else
		{
			if (false == selectable->GetSelected())
			{
				ClearSelection();
				InsertSelection(mHotPresenter);
			}

			ReportActiveSelections();
			mDragHandler = mDragHandlers[DRAG_HANDLER_TYPE_MOVE_COMPONENTS].get();
			mDragHandler->Enter(point);
		}
	}
	else if (mHotPresenter->GetPresenterType() == PRESENTER_TYPE_SEGMENT)
	{
		auto selectable = mHotPresenter->GetSelectableInterface();
		assert(selectable);
		if (controlled)
		{
			if (selectable->GetSelected())
			{
				RemoveSelection(mHotPresenter);
			}
			else
			{
				InsertSelection(mHotPresenter);
			}

			ReportActiveSelections();
			mDragHandler = nullptr;
		}
		else
		{
			if (false == selectable->GetSelected())
			{
				ClearSelection();
				InsertSelection(mHotPresenter);
			}

			ReportActiveSelections();
			mDragHandler = mDragHandlers[DRAG_HANDLER_TYPE_MOVE_COMPONENTS].get();
			mDragHandler->Enter(point);
		}
	}
	else
	{
		assert(false);
	}
}

void LevelView::OnLButtonUp(UINT flags, CPoint point)
{
	CWnd::OnLButtonUp(flags, point);

	if (GetCapture() == this)
	{
		if (mDragHandler)
		{
			mDragHandler->Leave(point);
		}
		ReleaseCapture();
	}
	else
	{
		mDragHandler = nullptr;
	}
}

void LevelView::OnCaptureChanged(CWnd* capturingWnd)
{
	mTrackingMouseLeave = false;

	if (capturingWnd != this)
	{
		mDragHandler = nullptr;
	}
}

void LevelView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UpdateHotPresenter(point);

	if (mHotPresenter)
	{
		auto presenterType = mHotPresenter->GetPresenterType();

		if (presenterType == PRESENTER_TYPE_BLOCK)
		{
			auto component = mHotPresenter->GetComponent();
			if (component->IsDerivedFrom(XSC_RTTI(ModuleBlock)))
			{
				ModuleBlock* moduleBlock = static_cast<ModuleBlock*>(component);
				ModuleLevel* moduleLevel = moduleBlock->GetModuleLevel();
				MainFrame* mainFrame = static_cast<MainFrame*>(theApp.m_pMainWnd);
				mainFrame->OpenLevelView(moduleLevel);
			}
			else
			{
				theParamManager->OpenWindow(component, theApp.m_pMainWnd);
			}
		}
		else if (presenterType == PRESENTER_TYPE_JOINT
			  || presenterType == PRESENTER_TYPE_SEGMENT)
		{
			auto component = mHotPresenter->GetComponent();
			assert(component->GetParent()->IsDerivedFrom(XSC_RTTI(Link)));
			theParamManager->OpenWindow(component->GetParent(), theApp.m_pMainWnd);
		}
	}
}

void LevelView::OnKeyDown(UINT key, UINT repCnt, UINT flags)
{
	CWnd::OnKeyDown(key, repCnt, flags);

	if (key == VK_DELETE)
	{
		if (mDeleteKeyDown)
		{
			return;
		}
		mDeleteKeyDown = true;

		ComponentSet components;
		components.reserve(mSelectedPresenters.size());
		for (auto presenter : mSelectedPresenters)
		{
			components.insert(presenter->GetComponent());
		}

		if (components.size())
		{
			auto command = new RemoveCommand(mLevel, std::move(components));
			auto model = static_cast<Model*>(mLevel->GetModel());
			model->ExecuteCommand(command);
		}
	}
	else if (key == VK_UP)
	{
		if (mMoveByKeyCommand)
		{
			if (mMoveDirection == MPVector::POSITIVE_Y)
			{
				mTempComponentSet.clear();
				for (auto presenter : mSelectedPresenters)
				{
					mTempComponentSet.insert(presenter->GetComponent());
				}
				mLevel->MoveComponents(mTempComponentSet, { 0, -1 });
			}
		}
		else
		{
			mMoveByKeyCommand = new CustomCommand(mLevel);
			mMoveDirection = MPVector::POSITIVE_Y;

			mTempComponentSet.clear();
			for (auto presenter : mSelectedPresenters)
			{
				mTempComponentSet.insert(presenter->GetComponent());
			}
			mLevel->MoveComponents(mTempComponentSet, { 0, -1 });
		}
	}
	else if (key == VK_DOWN)
	{
		if (mMoveByKeyCommand)
		{
			if (mMoveDirection == MPVector::NEGATIVE_Y)
			{
				mTempComponentSet.clear();
				for (auto presenter : mSelectedPresenters)
				{
					mTempComponentSet.insert(presenter->GetComponent());
				}
				mLevel->MoveComponents(mTempComponentSet, { 0, 1 });
			}
		}
		else
		{
			mMoveByKeyCommand = new CustomCommand(mLevel);
			mMoveDirection = MPVector::NEGATIVE_Y;

			mTempComponentSet.clear();
			for (auto presenter : mSelectedPresenters)
			{
				mTempComponentSet.insert(presenter->GetComponent());
			}
			mLevel->MoveComponents(mTempComponentSet, { 0, 1 });
		}
	}
	else if (key == VK_LEFT)
	{
		if (mMoveByKeyCommand)
		{
			if (mMoveDirection == MPVector::NEGATIVE_X)
			{
				mTempComponentSet.clear();
				for (auto presenter : mSelectedPresenters)
				{
					mTempComponentSet.insert(presenter->GetComponent());
				}
				mLevel->MoveComponents(mTempComponentSet, { -1, 0 });
			}
		}
		else
		{
			mMoveByKeyCommand = new CustomCommand(mLevel);
			mMoveDirection = MPVector::NEGATIVE_X;

			mTempComponentSet.clear();
			for (auto presenter : mSelectedPresenters)
			{
				mTempComponentSet.insert(presenter->GetComponent());
			}
			mLevel->MoveComponents(mTempComponentSet, { -1, 0 });
		}
	}
	else if (key == VK_RIGHT)
	{
		if (mMoveByKeyCommand)
		{
			if (mMoveDirection == MPVector::POSITIVE_X)
			{
				mTempComponentSet.clear();
				for (auto presenter : mSelectedPresenters)
				{
					mTempComponentSet.insert(presenter->GetComponent());
				}
				mLevel->MoveComponents(mTempComponentSet, { 1, 0 });
			}
		}
		else
		{
			mMoveByKeyCommand = new CustomCommand(mLevel);
			mMoveDirection = MPVector::POSITIVE_X;

			mTempComponentSet.clear();
			for (auto presenter : mSelectedPresenters)
			{
				mTempComponentSet.insert(presenter->GetComponent());
			}
			mLevel->MoveComponents(mTempComponentSet, { 1, 0 });
		}
	}
}

void LevelView::OnKeyUp(UINT key, UINT repCnt, UINT flags)
{
	CWnd::OnKeyUp(key, repCnt, flags);

	if (key == VK_DELETE)
	{
		mDeleteKeyDown = false;
	}

	if (mMoveByKeyCommand)
	{
		mMoveByKeyCommand->Finish();
		mMoveByKeyCommand = nullptr;
	}
}

void LevelView::OnKillFocus(CWnd* newFocus)
{
	CWnd::OnKillFocus(newFocus);

	if (mMoveByKeyCommand)
	{
		mMoveByKeyCommand->Finish();
		mMoveByKeyCommand = nullptr;
	}
}

void LevelView::OnEditSelectAll()
{
	mSelectedPresenters.clear();
	for (auto& item : mPresenters)
	{
		if (item.second->GetPresenterType() == PRESENTER_TYPE_LINK)
		{
			item.second->GetChildren(mSelectedPresenters);
		}
		else
		{
			assert(item.second->GetPresenterType() == PRESENTER_TYPE_BLOCK);
			mSelectedPresenters.insert(item.second.Get());
		}
	}

	for (auto presenter : mSelectedPresenters)
	{
		auto selectable = presenter->GetSelectableInterface();
		selectable->SetSelected(true);

		if (mPrimaryBlock == nullptr && presenter->GetPresenterType() == PRESENTER_TYPE_BLOCK)
		{
			ResetPrimary(presenter);
		}
	}

	ReportActiveSelections();
}

void LevelView::OnEditCut()
{
	if (mSelectedPresenters.size())
	{
		ComponentSet components;
		components.reserve(mSelectedPresenters.size());
		for (auto presenter : mSelectedPresenters)
		{
			components.insert(presenter->GetComponent());
		}

		auto command = new CutCommand(mLevel, std::move(components));
		auto model = static_cast<Model*>(mLevel->GetModel());
		model->ExecuteCommand(command);
	}
}

void LevelView::OnEditCopy()
{
	if (mSelectedPresenters.size())
	{
		ComponentSet components;
		components.reserve(mSelectedPresenters.size());
		for (auto presenter : mSelectedPresenters)
		{
			components.insert(presenter->GetComponent());
		}

		auto command = new CopyCommand(mLevel, std::move(components));
		auto model = static_cast<Model*>(mLevel->GetModel());
		model->ExecuteCommand(command);
	}
}

void LevelView::OnEditPaste()
{
	if (theModelManager->IsClipboardAvailable())
	{
		auto command = new PasteCommand(mLevel);
		auto model = static_cast<Model*>(mLevel->GetModel());

		mActiveAddition = true;
		model->ExecuteCommand(command);
		mActiveAddition = false;
	}
}

void LevelView::OnEditUndo()
{
	auto model = static_cast<Model*>(mLevel->GetModel());
	model->Undo();
}

void LevelView::OnEditRedo()
{
	auto model = static_cast<Model*>(mLevel->GetModel());
	model->Redo();
}

void LevelView::AlignBlocks(MPVector::Direction direction)
{
	if (mPrimaryBlock == nullptr)
	{
		LogItem logItem;
		logItem.senderName = L"System";
		logItem.message = L"No primary block. Select a primary block first.";
		theLogMediator->TransferLogItem(std::move(logItem));
		return;
	}

	auto pivot = static_cast<Block*>(mPrimaryBlock->GetComponent());

	BlockSet blocks;
	blocks.reserve(mSelectedPresenters.size());
	for (auto presenter : mSelectedPresenters)
	{
		auto component = presenter->GetComponent();
		if (component->IsDerivedFrom(XSC_RTTI(Block)))
		{
			if (component != pivot)
			{
				blocks.insert(static_cast<Block*>(component));
			}
		}
	}
	if (blocks.size() < 1)
	{
		LogItem logItem;
		logItem.senderName = L"System";
		logItem.message = L"Alignment requires multiple blocks. Ignoring...";
		theLogMediator->TransferLogItem(std::move(logItem));
		return;
	}

	auto command = new AlignBlockCommand(mLevel, std::move(blocks), pivot, direction);
	auto model = static_cast<Model*>(mLevel->GetModel());
	model->ExecuteCommand(command);
}

void LevelView::OnFormatAlignLeft()
{
	AlignBlocks(MPVector::NEGATIVE_X);
}

void LevelView::OnFormatAlignRight()
{
	AlignBlocks(MPVector::POSITIVE_X);
}

void LevelView::OnFormatAlignTop()
{
	AlignBlocks(MPVector::POSITIVE_Y);
}

void LevelView::OnFormatAlignBottom()
{
	AlignBlocks(MPVector::NEGATIVE_Y);
}

void LevelView::EqualizeBlocks(MPVector::Axis dim)
{
	if (mPrimaryBlock == nullptr)
	{
		LogItem logItem;
		logItem.senderName = L"System";
		logItem.message = L"No primary block. Select a primary block first.";
		theLogMediator->TransferLogItem(std::move(logItem));
		return;
	}

	auto* primary = static_cast<Block*>(mPrimaryBlock->GetComponent());

	BlockSet blocks;
	blocks.reserve(mSelectedPresenters.size());
	for (auto presenter : mSelectedPresenters)
	{
		auto component = presenter->GetComponent();
		if (component->IsDerivedFrom(XSC_RTTI(Block)))
		{
			if (component != primary)
			{
				blocks.insert(static_cast<Block*>(component));
			}
		}
	}
	if (blocks.size() < 1)
	{
		LogItem logItem;
		logItem.senderName = L"System";
		logItem.message = L"Size equalization requires multiple blocks. Ignoring...";
		theLogMediator->TransferLogItem(std::move(logItem));
		return;
	}

	auto command = new EqualizeBlockCommand(mLevel, std::move(blocks), primary, dim);
	static_cast<Model*>(mLevel->GetModel())->ExecuteCommand(command);
}

void LevelView::OnFormatMakeSameSize()
{
	EqualizeBlocks(MPVector::AXIS_OTHER);
}

void LevelView::OnFormatMakeSameWidth()
{
	EqualizeBlocks(MPVector::AXIS_X);
}

void LevelView::OnFormatMakeSameHeight()
{
	EqualizeBlocks(MPVector::AXIS_Y);
}

void LevelView::OnFormatRotateBlock()
{
	BlockSet blocks;
	blocks.reserve(mSelectedPresenters.size());
	for (auto presenter : mSelectedPresenters)
	{
		auto component = presenter->GetComponent();
		if (component->IsDerivedFrom(XSC_RTTI(Block)))
		{
			blocks.insert(static_cast<Block*>(component));
		}
	}
	if (blocks.size() == 0)
	{
		LogItem logItem;
		logItem.senderName = L"System";
		logItem.message = L"No blocks selected. Ignoring...";
		theLogMediator->TransferLogItem(std::move(logItem));
		return;
	}

	auto command = new RotateBlockCommand(mLevel, std::move(blocks));
	auto model = static_cast<Model*>(mLevel->GetModel());
	model->ExecuteCommand(command);
}

BOOL LevelView::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void LevelView::OnPaint() 
{
	CPaintDC dc(this);
	Render(theApp.GetAppTime());
}

void LevelView::UpdateScrollParameters()
{
	__int64 world, diff, start, end;

	world = mBound.l - WORLD_CONTENT_MARGIN;
	start = std::min<__int64>(0, world);
	world = mBound.r + WORLD_CONTENT_MARGIN;
	diff = world - mViewSize.x;
	end = std::max<__int64>(0, diff);
	mPanel->SetHorzRange(static_cast<int>(start), static_cast<int>(end));

	world = mBound.t - WORLD_CONTENT_MARGIN;
	start = std::min<__int64>(0, world);
	world = mBound.b + WORLD_CONTENT_MARGIN;
	diff = world - mViewSize.y;
	end = std::max<__int64>(0, diff);
	mPanel->SetVertRange(static_cast<int>(start), static_cast<int>(end));
}

MPVector LevelView::ViewToWorld(const MPVector& point) const
{
	int xOffset = mPanel->GetHorzOffset();
	int yOffset = mPanel->GetVertOffset();
	return MPVector(point.x + xOffset, point.y + yOffset);
}

MPVector LevelView::WorldToView(const MPVector& point) const
{
	int xOffset = mPanel->GetHorzOffset();
	int yOffset = mPanel->GetVertOffset();
	return MPVector(point.x - xOffset, point.y - yOffset);
}

MPRectangle LevelView::ViewToWorld(const MPRectangle& rect) const
{
	MPVector lt(rect.GetLeftTop());
	MPVector rb(rect.GetRightBottom());
	return MPRectangle(ViewToWorld(lt), ViewToWorld(rb));
}

MPRectangle LevelView::WorldToView(const MPRectangle& rect) const
{
	MPVector lt(rect.GetLeftTop());
	MPVector rb(rect.GetRightBottom());
	return MPRectangle(WorldToView(lt), WorldToView(rb));
}

void LevelView::DoLoop(double t)
{
	auto mainFrame = static_cast<MainFrame*>(theApp.m_pMainWnd);
	if (mainFrame->IsRenderable(this))
	{
		Render(t);
	}
}

void LevelView::Render(double t)
{
	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_WHITE);

	auto context = mRenderer.GetContext();

	D2D1_MATRIX_3X2_F transform;
	context->GetTransform(&transform);

	auto translate = D2D1::Matrix3x2F::Identity();
	translate._31 = -static_cast<FLOAT>(mPanel->GetHorzOffset());
	translate._32 = -static_cast<FLOAT>(mPanel->GetVertOffset());
	context->SetTransform(translate);

	if (mDragHandler)
	{
		mDragHandler->OnDrawBottom();
	}

	auto& links = mLevel->GetLinks();
	for (auto link : links)
	{
		mPresenters[link]->Render(context, t);
	}

	auto& blocks = mLevel->GetBlocks();
	for (auto block : blocks)
	{
		mPresenters[block]->Render(context, t);
	}

	if (mDragHandler)
	{
		mDragHandler->OnDrawTop();
	}

	context->SetTransform(transform);

	mRenderer.EndDraw();
	mRenderer.Display();
}

} // namespace hsmn
