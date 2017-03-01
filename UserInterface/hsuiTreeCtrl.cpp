#include "hsuiPrecompiled.h"
#include "hsuiTreeCtrl.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscWicBitmap.h>
#include <xscDWrite.h>

#undef max
#undef min

using namespace std;
using namespace xsc;

namespace hsui {

static const UINT WND_ID_HORZ = 1;
static const UINT WND_ID_VERT = 2;

static const int FIXED_ITEM_HEIGHT = 20;

static const int CHILD_INDENTATION = 20;
static const int GIZMO_AREA = 20;
static const int IMAGE_AREA = 20;

static const int MAX_ITEM_COUNT = 5000;

BEGIN_MESSAGE_MAP(TreeCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

TreeCtrl::TreeCtrl()
{
	mHandleLedger = 0;

	mRootNode = make_unique<ItemNode>();
	mItemToNode[0] = mRootNode.get();

	mHorzExtent = MAX_ITEM_COUNT;
	mVertExtent = 0;
	mRootNode->extentRecord = mHorzExtent.Insert(0, 0);

	mFocused = false;
	mTrackingMouseLeave = false;

	mHotItem = 0;
	mSelectedItem = 0;

	mExpandedGizmoNormal = nullptr;
	mExpandedGizmoBright = nullptr;
	mCollapsedGizmoNormal = nullptr;
	mCollapsedGizmoBright = nullptr;
	mDefaultIcon = nullptr;
	mBrushScrollCorner = nullptr;
	mBrushHot = nullptr;
	mBrushSelectedFocused = nullptr;
	mBrushSelectedUnfocused = nullptr;
	mTextBrushWhite = nullptr;
	mTextBrushNormal = nullptr;
	mTextBrushBlack = nullptr;

	mShowHorz = true;
	mShowVert = true;

	mListener = nullptr;
}

TreeCtrl::~TreeCtrl()
{
}

void TreeCtrl::SetListener(IListener* listener)
{
	mListener = listener;
}

void TreeCtrl::ResetListener()
{
	mListener = nullptr;
}

void TreeCtrl::Refresh()
{
	Invalidate(FALSE);
}

void TreeCtrl::CreateWnd(CWnd* parent, int x, int y, int w, int h)
{
	CRect rect;
	Create(nullptr, nullptr,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		{ x, y, x + w, y + h }, parent, 0);
}

void TreeCtrl::BuildImageList(HMODULE module, int numItems, const UINT* list)
{
	for (auto image : mImageList)
	{
		image->Release();
	}
	mImageList.clear();

	WicBitmap wicBitmap;
	ID2D1Bitmap* bitmap;
	for (int i = 0; i < numItems; ++i)
	{
		if (wicBitmap.LoadFromResource(module, list[i]))
		{
			mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &bitmap);
			mImageList.push_back(bitmap);
		}
		else
		{
			OutputDebugStringW(L"TreeCtrl::BuildImageList -> Image load failed.\n");
		}
	}
}

void TreeCtrl::MoveWnd(int x, int y)
{
	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void TreeCtrl::ResizeWnd(int w, int h)
{
	SetWindowPos(nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void TreeCtrl::MoveAndResizeWnd(int x, int y, int w, int h)
{
	SetWindowPos(nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

void TreeCtrl::Reorder(int parent, const vector<int>& children)
{
	unordered_map<int, ItemNode*>::iterator iter;
	iter = mItemToNode.find(parent);

	ItemNode* parentNode = iter->second;

	unordered_map<int, unique_ptr<ItemNode>> childrenSave;
	childrenSave.reserve(parentNode->children.size());
	for (auto& child : parentNode->children)
	{
		childrenSave[child->handle] = std::move(child);
	}
	parentNode->children.clear();

	for (auto child : children)
	{
		iter = mItemToNode.find(child);
		parentNode->children.push_back(std::move(childrenSave[child]));
	}
}

int TreeCtrl::GetSelectedItem() const
{
	return mSelectedItem;
}

void* TreeCtrl::GetItemData(int handle) const
{
	unordered_map<int, ItemNode*>::const_iterator iter;
	iter = mItemToNode.find(handle);
	if (iter == mItemToNode.end())
	{
		return nullptr;
	}
	return iter->second->item.data;
}

void TreeCtrl::SetSelectedItem(int handle)
{
	mSelectedItem = handle;
}

void TreeCtrl::SetItemText(int handle, const std::wstring& text)
{
	if (handle == 0)
	{
		return;
	}

	ItemToNode::iterator iter = mItemToNode.find(handle);
	if (iter == mItemToNode.end())
	{
		return;
	}

	Item& item = iter->second->item;
	if (item.text == text)
	{
		return;
	}
	item.text = text;

	IDWriteTextFormat* font = item.bold ? DWrite::segoe12B : DWrite::segoe12R;
	int horzExtent = iter->second->pivot + GIZMO_AREA + DWrite::MeasureTextExtent(font, text);
	mHorzExtent.Update(iter->second->extentRecord, horzExtent);

	int topItem;
	mHorzExtent.GetMaximum(topItem, horzExtent);
	mHorz.SetRange(0, horzExtent - mClientWidth + ScrollBar::FIXED_SIZE);
}

void TreeCtrl::SetItemData(int handle, void* data)
{
	if (handle == 0)
	{
		return;
	}

	ItemToNode::iterator iter = mItemToNode.find(handle);
	if (iter == mItemToNode.end())
	{
		return;
	}

	Item& item = iter->second->item;
	item.data = data;
}

void TreeCtrl::SetItemImage(int handle, int image)
{
	if (handle == 0)
	{
		return;
	}

	ItemToNode::iterator iter = mItemToNode.find(handle);
	if (iter == mItemToNode.end())
	{
		return;
	}

	iter->second->item.image = image;
}

void TreeCtrl::SetItemTextBoldness(int handle, bool bold)
{
	if (handle == 0)
	{
		return;
	}

	ItemToNode::iterator iter = mItemToNode.find(handle);
	if (iter == mItemToNode.end())
	{
		return;
	}

	Item& item = iter->second->item;
	if (item.bold == bold)
	{
		return;
	}
	item.bold = bold;

	IDWriteTextFormat* font = bold ? DWrite::segoe12B : DWrite::segoe12R;
	int horzExtent = iter->second->pivot + GIZMO_AREA + DWrite::MeasureTextExtent(font, item.text);
	mHorzExtent.Update(iter->second->extentRecord, horzExtent);

	int topItem;
	mHorzExtent.GetMaximum(topItem, horzExtent);
	mHorz.SetRange(0, horzExtent - mClientWidth + ScrollBar::FIXED_SIZE);
}

int TreeCtrl::InsertItem(const Item& item, int parent, int insertAfter)
{
	ItemToNode::iterator parentIter;
	parentIter = mItemToNode.find(parent);
	if (parentIter == mItemToNode.end())
	{
		return 0;
	}

	ItemNode* parentNode = parentIter->second;

	int handle = ++mHandleLedger;

	unique_ptr<ItemNode> newNodePtr(make_unique<ItemNode>(item, parentNode, handle));
	ItemNode* newNode = newNodePtr.get();
	if (insertAfter)
	{
		list<unique_ptr<ItemNode>>::iterator iter;
		for (iter = parentNode->children.begin(); iter != parentNode->children.end(); ++iter)
		{
			if ((*iter)->handle == insertAfter)
			{
				break;
			}
		}

		if (iter == parentNode->children.end())
		{
			parentNode->children.push_back(std::move(newNodePtr));
		}
		else
		{
			parentNode->children.insert(++iter, std::move(newNodePtr));
		}
	}
	else
	{
		parentNode->children.push_back(std::move(newNodePtr));
	}

	mItemToNode[handle] = newNode;

	int oldVertExtent = mVertExtent;
	if (CheckExposednessRecursively(parentNode))
	{
		mVertExtent += FIXED_ITEM_HEIGHT;
		mVert.SetRange(0, mVertExtent - mClientHeight + ScrollBar::FIXED_SIZE);
	}

	int longestItem;
	int longestExtent;
	mHorzExtent.GetMaximum(longestItem, longestExtent);

	IDWriteTextFormat* font = item.bold ? DWrite::segoe12B : DWrite::segoe12R;
	int horzExtent = newNode->pivot + GIZMO_AREA + DWrite::MeasureTextExtent(font, item.text);
	newNode->extentRecord = mHorzExtent.Insert(handle, horzExtent);
	if (horzExtent > longestExtent)
	{
		mHorz.SetRange(0, horzExtent - mClientWidth + ScrollBar::FIXED_SIZE);
	}

	return handle;
}

bool TreeCtrl::CheckExposednessRecursively(ItemNode* node)
{
	if (node->Expanded)
	{
		if (node->parent)
		{
			return CheckExposednessRecursively(node->parent);
		}
		return true;
	}
	return false;
}

void TreeCtrl::RemoveItem(int itemHandle)
{
	if (itemHandle == 0)
	{
		return;
	}

	unordered_map<int, ItemNode*>::iterator iter;
	iter = mItemToNode.find(itemHandle);
	if (iter == mItemToNode.end())
	{
		return;
	}

	ItemNode* parentNode = iter->second->parent;
	list<unique_ptr<ItemNode>>::iterator childIter;
	for (childIter = parentNode->children.begin();
		childIter != parentNode->children.end();
		++childIter)
	{
		if (childIter->get() == iter->second)
		{
			break;
		}
	}

	int oldHotItem = mHotItem;
	int oldSelectedItem = mSelectedItem;

	int dummyItem;
	int oldHorzExtent;
	mHorzExtent.GetMaximum(dummyItem, oldHorzExtent);

	int oldVertExtent = mVertExtent;
	bool calcVertDelta = CheckExposednessRecursively(parentNode);

	RemoveItemRecursively(iter->second, calcVertDelta);
	parentNode->children.erase(childIter);

	int horzExtent;
	mHorzExtent.GetMaximum(dummyItem, horzExtent);
	if (oldHorzExtent != horzExtent)
	{
		mHorz.SetRange(0, horzExtent - mClientWidth + ScrollBar::FIXED_SIZE);
	}
	if (oldVertExtent != mVertExtent)
	{
		mVert.SetRange(0, mVertExtent - mClientHeight + ScrollBar::FIXED_SIZE);
	}

	if (mSelectedItem != oldSelectedItem)
	{
		if (mListener) mListener->OnSelectionChanged(this);
	}
}

void TreeCtrl::RemoveAllItems()
{
	int oldSelectedItem = mSelectedItem;

	for (auto& itemNode : mRootNode->children)
	{
		RemoveItemRecursively(itemNode.get(), true);
	}
	mRootNode->children.clear();

	mHorz.SetRange(0, 0);
	mVert.SetRange(0, 0);

	mHotItem = 0;
	mSelectedItem = 0;

	if (0 != oldSelectedItem)
	{
		if (mListener) mListener->OnSelectionChanged(this);
	}
}

void TreeCtrl::RemoveItemRecursively(ItemNode* node, bool calcVertDelta)
{
	if (node->handle == mHotItem)		mHotItem = 0;
	if (node->handle == mSelectedItem)	mSelectedItem = 0;

	mHorzExtent.Update(node->extentRecord, numeric_limits<int>::max());

	int itemExtent;
	mHorzExtent.Remove(node->handle, itemExtent);

	if (calcVertDelta)
	{
		mVertExtent -= FIXED_ITEM_HEIGHT;
	}

	for (auto& child : node->children)
	{
		if (calcVertDelta)
		{
			RemoveItemRecursively(child.get(), node->Expanded);
		}
		else
		{
			RemoveItemRecursively(child.get(), false);
		}
	}

	mItemToNode.erase(node->handle);
}

void TreeCtrl::OnScrollPositionChanged(ScrollBar* scrollBar)
{
	Render();
}

void TreeCtrl::CollapseItem(int itemHandle)
{
	int oldVertExtent = mVertExtent;

	if (itemHandle == 0)
	{
		mVertExtent = (int)(FIXED_ITEM_HEIGHT * mRootNode->children.size());
		if (oldVertExtent != mVertExtent)
		{
			mVert.SetRange(0, mVertExtent - mClientHeight + ScrollBar::FIXED_SIZE);
			Invalidate(FALSE);
		}

		unordered_map<int, ItemNode*>::iterator nodeIter;
		for (nodeIter = mItemToNode.begin(); nodeIter != mItemToNode.end(); ++nodeIter)
		{
			if (nodeIter->first != 0/*the root*/)
			{
				nodeIter->second->Expanded = false;
			}
		}
		return;
	}

	unordered_map<int, ItemNode*>::const_iterator iter;
	iter = mItemToNode.find(itemHandle);
	if (iter == mItemToNode.end())
	{
		return;
	}

	ItemNode* itemNode = iter->second;
	if (itemNode->Expanded == false)
	{
		return;
	}
	itemNode->Expanded = false;
	Invalidate(FALSE);

	if (false == CheckExposednessRecursively(itemNode->parent))
	{
		return;
	}

	for (auto& child : itemNode->children)
	{
		ApplyCollapseQuantityRecursively(child.get());
	}

	if (oldVertExtent != mVertExtent)
	{
		mVert.SetRange(0, mVertExtent - mClientHeight + ScrollBar::FIXED_SIZE);
		Invalidate(FALSE);
	}
}

void TreeCtrl::ApplyCollapseQuantityRecursively(ItemNode* itemNode)
{
	mVertExtent -= FIXED_ITEM_HEIGHT;

	if (itemNode->Expanded)
	{
		for (auto& child : itemNode->children)
		{
			ApplyCollapseQuantityRecursively(child.get());
		}
	}
}

void TreeCtrl::ExpandItem(int itemHandle)
{
	int oldVertExtent = mVertExtent;

	if (itemHandle == 0)
	{
		mVertExtent = (int)(FIXED_ITEM_HEIGHT * (mItemToNode.size() - 1));
		if (oldVertExtent != mVertExtent)
		{
			mVert.SetRange(0, mVertExtent - mClientHeight + ScrollBar::FIXED_SIZE);
			Invalidate(FALSE);
		}

		unordered_map<int, ItemNode*>::iterator nodeIter;
		for (nodeIter = mItemToNode.begin(); nodeIter != mItemToNode.end(); ++nodeIter)
		{
			if (nodeIter->first != 0)
			{
				nodeIter->second->Expanded = true;
			}
		}
		return;
	}

	unordered_map<int, ItemNode*>::const_iterator iter;
	iter = mItemToNode.find(itemHandle);
	if (iter == mItemToNode.end())
	{
		return;
	}

	ItemNode* itemNode = iter->second;
	if (itemNode->Expanded == true)
	{
		return;
	}
	itemNode->Expanded = true;
	Invalidate(FALSE);

	if (false == CheckExposednessRecursively(itemNode->parent))
	{
		return;
	}

	for (auto& child : itemNode->children)
	{
		AccumulateExpandQuantityRecursively(child.get());
	}

	if (oldVertExtent != mVertExtent)
	{
		mVert.SetRange(0, mVertExtent - mClientHeight + ScrollBar::FIXED_SIZE);
		Invalidate(FALSE);
	}
}

bool TreeCtrl::IsLeaf(int handle) const
{
	auto iter = mItemToNode.find(handle);
	if (iter != mItemToNode.end())
	{
		return (0 == iter->second->children.size());
	}
	return false;
}

void TreeCtrl::ShowHorzScrollBar(bool show)
{
	if (mShowHorz != show)
	{
		mShowHorz = show;
		mHorz.ShowWindow(show ? SW_SHOW : SW_HIDE);

		ReconfigureScrolls();
	}
}

void TreeCtrl::ShowVertScrollBar(bool show)
{
	if (mShowVert != show)
	{
		mShowVert = show;
		mVert.ShowWindow(show ? SW_SHOW : SW_HIDE);

		ReconfigureScrolls();
	}
}

void TreeCtrl::AccumulateExpandQuantityRecursively(ItemNode* itemNode)
{
	mVertExtent += FIXED_ITEM_HEIGHT;

	if (itemNode->Expanded)
	{
		for (auto& child : itemNode->children)
		{
			AccumulateExpandQuantityRecursively(child.get());
		}
	}
}

void TreeCtrl::UpdateHotItem(CPoint point)
{
	int horzOffset = -mHorz.GetScrollPosition();
	int vertDisplacement = -mVert.GetScrollPosition();

	int oldHotItem = mHotItem;
	mHotItem = 0;
	mHotZone = HOT_ZONE_OTHER;

	for (auto& child : mRootNode->children)
	{
		CheckHotStateRecursively(child.get(),
			horzOffset, vertDisplacement,
			mClientWidth - ScrollBar::FIXED_SIZE,
			mClientHeight - ScrollBar::FIXED_SIZE,
			point);
	}

	if (mHotItem != oldHotItem)
	{
		Invalidate(FALSE);
	}
}

void TreeCtrl::CheckHotStateRecursively(
	ItemNode* itemNode, int horzOffset, int& vertDisplacement, int xViewExtent, int yViewExtent, CPoint point)
{
	int top = vertDisplacement;
	int bot = vertDisplacement + FIXED_ITEM_HEIGHT;
	vertDisplacement += FIXED_ITEM_HEIGHT;

	CRect rect(0, top, xViewExtent, bot);
	if (rect.PtInRect(point))
	{
		mHotItem = itemNode->handle;

		rect.left = horzOffset + itemNode->pivot - GIZMO_AREA - IMAGE_AREA;
		rect.right = rect.left + GIZMO_AREA;
		if (rect.PtInRect(point))
		{
			mHotZone = HOT_ZONE_GIZMO;
		}
		else
		{
			mHotZone = HOT_ZONE_OTHER;
		}
		return;
	}

	if (itemNode->Expanded)
	{
		for (auto& child : itemNode->children)
		{
			CheckHotStateRecursively(child.get(), horzOffset, vertDisplacement, xViewExtent, yViewExtent, point);
		}
	}
}

int TreeCtrl::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	mClientWidth = createStruct->cx;
	mClientHeight = createStruct->cy;

	mRenderer.Create(m_hWnd, mClientWidth, mClientHeight);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_SEMI_WHITE, &mBrushScrollCorner);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_ORANGE, &mBrushHot);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BLUE, &mBrushSelectedFocused);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BASE, &mBrushSelectedUnfocused);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_WHITE, &mTextBrushWhite);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mTextBrushNormal);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BLACK, &mTextBrushBlack);

	WicBitmap wicBitmap(hsui::Module::handle, HSUI_IDR_TREE_ITEM_ICON);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mDefaultIcon);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_TREE_ITEM_EXPANDED_N);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mExpandedGizmoNormal);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_TREE_ITEM_EXPANDED_H);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mExpandedGizmoBright);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_TREE_ITEM_COLLAPSED_N);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mCollapsedGizmoNormal);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_TREE_ITEM_COLLAPSED_H);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mCollapsedGizmoBright);

	mHorz.SetListener(static_cast<ScrollBar::IListener*>(this));
	mHorz.CreateWnd(this, 0, 0, 100);
	mHorz.SetStepSize(FIXED_ITEM_HEIGHT);

	mVert.SetListener(static_cast<ScrollBar::IListener*>(this));
	mVert.CreateWnd(this, 0, 0, 100);
	mVert.SetStepSize(FIXED_ITEM_HEIGHT);

	return 0;
}

void TreeCtrl::OnDestroy()
{
	SafeRelease(mDefaultIcon);
	SafeRelease(mExpandedGizmoNormal);
	SafeRelease(mCollapsedGizmoNormal);
	SafeRelease(mExpandedGizmoBright);
	SafeRelease(mCollapsedGizmoBright);
	SafeRelease(mBrushScrollCorner);
	SafeRelease(mBrushHot);
	SafeRelease(mBrushSelectedFocused);
	SafeRelease(mBrushSelectedUnfocused);
	SafeRelease(mTextBrushWhite);
	SafeRelease(mTextBrushNormal);
	SafeRelease(mTextBrushBlack);
	for (auto image : mImageList)
	{
		image->Release();
	}
	mImageList.clear();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void TreeCtrl::ReconfigureScrolls()
{
	int horzSize = mClientWidth - (mShowVert ? ScrollBar::FIXED_SIZE : 0);
	int vertSize = mClientHeight - (mShowHorz ? ScrollBar::FIXED_SIZE : 0);
	mHorz.MoveAndResizeWnd(0, mClientHeight - ScrollBar::FIXED_SIZE, horzSize);
	mVert.MoveAndResizeWnd(mClientWidth - ScrollBar::FIXED_SIZE, 0, vertSize);

	int vertRange = mVertExtent - mClientHeight + ScrollBar::FIXED_SIZE;
	int vertPage = mClientHeight - ScrollBar::FIXED_SIZE;
	mVert.SetRange(0, vertRange);
	mVert.SetPageSize(vertPage);

	int topItem;
	int horzExtent;
	mHorzExtent.GetMaximum(topItem, horzExtent);

	int horzRange = horzExtent - mClientWidth + ScrollBar::FIXED_SIZE;
	int horzPage = mClientWidth - ScrollBar::FIXED_SIZE;
	mHorz.SetRange(0, horzRange);
	mHorz.SetPageSize(horzPage);
}

void TreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	mClientWidth = cx;
	mClientHeight = cy;
	mRenderer.Resize(cx, cy);

	ReconfigureScrolls();
}

void TreeCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	mFocused = true;
	if (mSelectedItem)
	{
		Invalidate(FALSE);
	}
}

void TreeCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	mFocused = false;
	if (mSelectedItem)
	{
		Invalidate(FALSE);
	}
}

void TreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (false == mTrackingMouseLeave)
	{
		mTrackingMouseLeave = true;

		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = m_hWnd;
		TrackMouseEvent(&trackMouseEvent);
	}

	UpdateHotItem(point);
}

void TreeCtrl::OnMouseLeave()
{
	mTrackingMouseLeave = false;

	mHotZone = HOT_ZONE_OTHER;
	if (mHotItem != 0)
	{
		mHotItem = 0;
		Invalidate(FALSE);
	}
}

BOOL TreeCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	mVert.SetScrollPosition(mVert.GetScrollPosition() - int(zDelta * 0.5f));

	GetCursorPos(&pt);
	ScreenToClient(&pt);
	UpdateHotItem(pt);
	Render();

	return TRUE;
}

void TreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	UpdateHotItem(point);

	if (mHotItem != 0)
	{
		if (mSelectedItem != mHotItem)
		{
			mSelectedItem = mHotItem;
			Invalidate(FALSE);

			if (mListener) mListener->OnSelectionChanged(this);
		}
	}
}

void TreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	if (mHotItem != 0)
	{
		if (mHotZone == HOT_ZONE_GIZMO)
		{
			unordered_map<int, ItemNode*>::iterator iter = mItemToNode.find(mHotItem);
			if (iter->second->Expanded)
			{
				CollapseItem(mHotItem);
			}
			else
			{
				ExpandItem(mHotItem);
			}
		}

		if (mSelectedItem != mHotItem)
		{
			mSelectedItem = mHotItem;
			Invalidate(FALSE);

			if (mListener) mListener->OnSelectionChanged(this);
		}

		if (mListener) mListener->OnLButtonDownOnItem(this);
	}
}

void TreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (mHotItem != 0)
	{
		if (mListener) mListener->OnItemDoubleClicked(this);
	}
}

BOOL TreeCtrl::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void TreeCtrl::OnPaint()
{
	CPaintDC dc(this);

	Render();
}

void TreeCtrl::Render()
{
	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_SEMI_WHITE);

	int horzOffset = -mHorz.GetScrollPosition();
	int vertDisplacement = -mVert.GetScrollPosition();
	for (auto& itemNode : mRootNode->children)
	{
		RenderItemRecursively(itemNode.get(), horzOffset, vertDisplacement, mClientWidth, mClientHeight);
	}

	D2D1_RECT_F rect;
	rect.left = (FLOAT)mClientWidth - ScrollBar::FIXED_SIZE;
	rect.right = (FLOAT)mClientWidth;
	rect.top = (FLOAT)mClientHeight - ScrollBar::FIXED_SIZE;
	rect.bottom = (FLOAT)mClientHeight;
	mRenderer.FillRectangle(rect, mBrushScrollCorner);

	mRenderer.EndDraw();
	mRenderer.Display();
}

void TreeCtrl::RenderItemRecursively(const ItemNode* itemNode,
	int horzOffset, int& vertDisplacement, int xViewExtent, int yViewExtent)
{
	int top = vertDisplacement;
	int bot = vertDisplacement + FIXED_ITEM_HEIGHT;

	vertDisplacement += FIXED_ITEM_HEIGHT;

	if (bot >= 0 && top <= yViewExtent)
	{
		D2D1_RECT_F rect;

		rect.left = 0.0f;
		rect.right = (FLOAT)xViewExtent;
		rect.top = (FLOAT)top;
		rect.bottom = (FLOAT)bot;

		if (itemNode->handle == mSelectedItem)
		{
			if (mFocused)
			{
				mRenderer.FillRectangle(rect, mBrushSelectedFocused);
			}
			else
			{
				mRenderer.FillRectangle(rect, mBrushSelectedUnfocused);
			}
		}
		else if (itemNode->handle == mHotItem)
		{
		}

		if (itemNode->children.size() > 0)
		{
			rect.left = (FLOAT)itemNode->pivot + horzOffset - GIZMO_AREA - IMAGE_AREA;
			rect.right = rect.left + GIZMO_AREA;
			if (itemNode->Expanded)
			{
				if (mFocused)
				{
					if (itemNode->handle == mSelectedItem)
					{
						mRenderer.DrawBitmap(mExpandedGizmoBright, &rect);
					}
					else
					{
						mRenderer.DrawBitmap(mExpandedGizmoNormal, &rect);
					}
				}
				else
				{
					mRenderer.DrawBitmap(mExpandedGizmoNormal, &rect);
				}
			}
			else
			{
				if (mFocused)
				{
					if (itemNode->handle == mSelectedItem)
					{
						mRenderer.DrawBitmap(mCollapsedGizmoBright, &rect);
					}
					else
					{
						mRenderer.DrawBitmap(mCollapsedGizmoNormal, &rect);
					}
				}
				else
				{
					mRenderer.DrawBitmap(mCollapsedGizmoNormal, &rect);
				}
			}
		}

		rect.left = (FLOAT)itemNode->pivot + horzOffset - IMAGE_AREA;
		rect.right = rect.left + IMAGE_AREA;

		ID2D1Bitmap* iconBitmap = mDefaultIcon;
		if (itemNode->item.image >= 0)
		{
			int numImages = static_cast<int>(mImageList.size());
			if (itemNode->item.image < numImages)
			{
				iconBitmap = mImageList[itemNode->item.image];
			}
		}
		mRenderer.DrawBitmap(iconBitmap, &rect);

		rect.left = (FLOAT)itemNode->pivot + horzOffset + 1;
		rect.right = (FLOAT)xViewExtent;
		rect.top -= 1;
		rect.bottom -= 1;

		IDWriteTextFormat* font = itemNode->item.bold ? DWrite::segoe12B : DWrite::segoe12R;
		if (itemNode->handle == mSelectedItem)
		{
			if (mFocused)
			{
				mRenderer.DrawText(itemNode->item.text.c_str(),
					static_cast<UINT32>(itemNode->item.text.length()),
					font, rect, mTextBrushWhite,
					D2D1_DRAW_TEXT_OPTIONS_CLIP);
			}
			else
			{
				mRenderer.DrawText(itemNode->item.text.c_str(),
					static_cast<UINT32>(itemNode->item.text.length()),
					font, rect, mTextBrushNormal,
					D2D1_DRAW_TEXT_OPTIONS_CLIP);
			}
		}
		else if (itemNode->handle == mHotItem)
		{
			mRenderer.DrawText(itemNode->item.text.c_str(),
				static_cast<UINT32>(itemNode->item.text.length()),
				font, rect, mTextBrushNormal,
				D2D1_DRAW_TEXT_OPTIONS_CLIP);
		}
		else if (itemNode->item.bold)
		{
			mRenderer.DrawText(itemNode->item.text.c_str(),
				static_cast<UINT32>(itemNode->item.text.length()),
				font, rect, mTextBrushNormal,
				D2D1_DRAW_TEXT_OPTIONS_CLIP);
		}
		else
		{
			mRenderer.DrawText(itemNode->item.text.c_str(),
				static_cast<UINT32>(itemNode->item.text.length()),
				font, rect, mTextBrushNormal,
				D2D1_DRAW_TEXT_OPTIONS_CLIP);
		}
	}

	if (itemNode->Expanded)
	{
		for (auto& child : itemNode->children)
		{
			RenderItemRecursively(
				child.get(), horzOffset, vertDisplacement, xViewExtent, yViewExtent);
		}
	}
}

TreeCtrl::Item::Item()
{
	image = -1;
	data = nullptr;
	bold = false;
}

TreeCtrl::Item::Item(const Item& item)
{
	*this = item;
}

TreeCtrl::Item& TreeCtrl::Item::operator=(const Item& item)
{
	image = item.image;
	text = item.text;
	bold = item.bold;
	data = item.data;
	return *this;
}

TreeCtrl::Item::Item(Item&& item)
{
	*this = item;
}

TreeCtrl::Item& TreeCtrl::Item::operator=(Item&& item)
{
	image = item.image;
	text = std::move(item.text);
	bold = item.bold;
	data = item.data;
	return *this;
}

TreeCtrl::ItemNode::ItemNode()
{
	handle = 0;
	parent = nullptr;
	Expanded = true;
	level = 0;
	pivot = 0;
}

TreeCtrl::ItemNode::ItemNode(const Item& argItem, ItemNode* argParent, int argHandle)
{
	item = argItem;
	handle = argHandle;
	parent = argParent;
	Expanded = true;

	level = parent->level + 1;
	if (level == 1)
	{
		pivot = GIZMO_AREA + IMAGE_AREA;
	}
	else
	{
		pivot = parent->pivot + CHILD_INDENTATION;
	}
}

TreeCtrl::ItemNode::ItemNode(Item&& argItem, ItemNode* argParent, int argHandle)
{
	item = std::move(argItem);
	handle = argHandle;
	parent = argParent;
	Expanded = true;

	level = parent->level + 1;
	if (level == 1)
	{
		pivot = GIZMO_AREA + IMAGE_AREA;
	}
	else
	{
		pivot = parent->pivot + CHILD_INDENTATION;
	}
}

} // namespace hsui
