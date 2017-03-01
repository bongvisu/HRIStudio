#include "hsmnPrecompiled.h"
#include "hsmnLibraryView.h"
#include "hsmnApplication.h"
#include "hsmnIBlockDropTarget.h"

#include <xscDWrite.h>
#include <xscWicBitmap.h>

#include <hsmoModuleBlock.h>
#include <hsmoModuleInBlock.h>
#include <hsmoModuleOutBlock.h>
#include <hsmoEventBlock.h>
#include <hsmoSayBlock.h>
#include <hsmoIfBlock.h>
#include <hsmoSwitchBlock.h>
#include <hsmoVariableBlock.h>
#include <hsmoDialogBlock.h>

#include <hsprBlockPresenter.h>
#include <hsprLinkPresenter.h>

#include <hsuiVisualManager.h>

#undef max
#undef min

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hspr;
using namespace hsui;

namespace hsmn {

static const int GROUP_HEIGHT = 22;
static const int ELEMENT_WIDTH = 50;
static const int ELEMENT_HEIGHT = 47;
static const int ELEMENT_ICON_SIZE = 47;
static const int VERTICAL_MARGIN = 10;

BEGIN_MESSAGE_MAP(LibraryView, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CAPTURECHANGED()

	ON_WM_PAINT()
END_MESSAGE_MAP()

LibraryView::LibraryView()
{
	mHotItem = nullptr;
	mSelectedItem = nullptr;
	mTrackingMouseLeave = false;
	mDragEntered = false;

	mWidth = 0;
	mHeight = 0;
	mNumColumns = 0;
	mIntercolumnGap = 0;
}

LibraryView::~LibraryView()
{
}

void LibraryView::PopulateItems()
{
	Group* group;
	Element* element;

	group = new Group(L"Structural");
	{
		element = new Element(XSC_RTTI(ModuleBlock).GetName());
		group->mElements.push_back(element);

		element = new Element(XSC_RTTI(ModuleInBlock).GetName());
		group->mElements.push_back(element);

		element = new Element(XSC_RTTI(ModuleOutBlock).GetName());
		group->mElements.push_back(element);
	}
	mGroups.push_back(group);

	group = new Group(L"Procedural");
	{
		element = new Element(XSC_RTTI(EventBlock).GetName());
		group->mElements.push_back(element);

		element = new Element(XSC_RTTI(IfBlock).GetName());
		group->mElements.push_back(element);

		element = new Element(XSC_RTTI(SwitchBlock).GetName());
		group->mElements.push_back(element);

		element = new Element(XSC_RTTI(SayBlock).GetName());
		group->mElements.push_back(element);

		element = new Element(XSC_RTTI(VariableBlock).GetName());
		group->mElements.push_back(element);

		element = new Element(XSC_RTTI(DialogBlock).GetName());
		group->mElements.push_back(element);
	}
	mGroups.push_back(group);
}

int LibraryView::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	DWrite::factory->CreateTextFormat(
		L"Cambria", nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		10.0f, L"", &mTextFormat);
	mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	mTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);

	DWRITE_TRIMMING trimming;
	trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
	trimming.delimiter = 0;
	trimming.delimiterCount = 0;
	mTextFormat->SetTrimming(&trimming, nullptr);

	mRenderer.Create(m_hWnd, 8, 8);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL3, &mBrush);
	
	WicBitmap wicBitmap;
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_LIBRARY_GROUP_EXPANED);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mIconExpaned);
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_LIBRARY_GROUP_COLLAPSED);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mIconCollapsed);

	mScrollBar.CreateWnd(this, 0, 0, 100);
	mScrollBar.SetListener(static_cast<ScrollBar::IListener*>(this));
	mScrollBar.SetStepSize(ELEMENT_HEIGHT + VERTICAL_MARGIN);

	CPngImage dragIcon;
	dragIcon.Load(HSMN_IDR_LIBRARY_BLOCK_DRAG_IMAGE, theApp.m_hInstance);
	mDragIcon.Create(40, 40, ILC_COLOR32, 1, 0);
	mDragIcon.Add(&dragIcon, RGB(0, 0, 0));

	PopulateItems();

	return 0;
}

void LibraryView::OnDestroy()
{
	mScrollBar.ResetListener();

	mIconExpaned->Release();
	mIconCollapsed->Release();
	mBrush->Release();
	mRenderer.Destroy();

	mTextFormat->Release();

	CWnd::OnDestroy();
}

void LibraryView::OnScrollPositionChanged(ScrollBar* scrollBar)
{
	Invalidate(FALSE);
}

void LibraryView::UpdateGeometricParameters(bool reconfigure)
{
	int offset = 0;
	for (auto& group : mGroups)
	{
		if (reconfigure) group->Reconfigure(offset, this);
		else			 group->UpdateOffset(offset);
		offset += group->GetExtent();
	}

	mScrollBar.SetRange(0, offset - mHeight);
	mScrollBar.SetPageSize(mHeight);
}

void LibraryView::OnSize(UINT type, int cx, int cy)
{
	mRenderer.Resize(cx, cy);

	mWidth = cx;
	mHeight = cy;
	int contentSize = mWidth - ScrollBar::FIXED_SIZE;

	mScrollBar.MoveAndResizeWnd(contentSize, 0, cy);

	int numColumns = contentSize / ELEMENT_WIDTH;
	mNumColumns = std::max(1, numColumns);
	
	int extentElements = mNumColumns * ELEMENT_WIDTH;
	double remainingSize = contentSize - extentElements;
	mIntercolumnGap = remainingSize / (mNumColumns + 1);
	
	UpdateGeometricParameters(true);
}

void LibraryView::UpdateHotItem(CPoint point)
{
	Item* oldHot = mHotItem;

	point.y += mScrollBar.GetScrollPosition();

	for (auto& group : mGroups)
	{
		mHotItem = group->FindHit(point, this);
		if (mHotItem) break;
	}

	if (oldHot != mHotItem)
	{
		if (oldHot) oldHot->mHot = false;
		if (mHotItem) mHotItem->mHot = true;
		Invalidate(FALSE);
	}
}

void LibraryView::OnMouseLeave()
{
	mTrackingMouseLeave = false;

	if (mHotItem)
	{
		mHotItem->mHot = false;
		mHotItem = nullptr;
		Invalidate(FALSE);
	}
}

BOOL LibraryView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int scrollPos = mScrollBar.GetScrollPosition();

	int scrollUnit = mScrollBar.GetStepSize();
	int quotient = zDelta / scrollUnit;
	int delta = quotient * scrollUnit;

	scrollPos -= delta;
	mScrollBar.SetScrollPosition(scrollPos);

	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	UpdateHotItem(point);
	Invalidate(FALSE);

	return TRUE;
}

void LibraryView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (mTrackingMouseLeave == false)
	{
		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = m_hWnd;
		TrackMouseEvent(&trackMouseEvent);

		mTrackingMouseLeave = true;
	}

	if (GetCapture() == this)
	{
		if (mDragEntered)
		{
			ClientToScreen(&point);
			CImageList::DragMove(point);
		}
		else
		{
			mDragEntered = true;

			ClientToScreen(&point);
			mDragIcon.BeginDrag(0, CPoint(20, 20));
			CImageList::DragEnter(0, point);
		}
	}
	else
	{
		UpdateHotItem(point);
	}
}

void LibraryView::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();
}

void LibraryView::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	UpdateHotItem(point);

	if (mHotItem)
	{
		point.y += mScrollBar.GetScrollPosition();

		if (mHotItem->GetItemType() == ITEM_TYPE_GROUP)
		{
			Group* group = static_cast<Group*>(mHotItem);
			if (group->ConsumeLButtonDown(point, this))
			{
				UpdateGeometricParameters(false);
				Invalidate(FALSE);
			}
		}
		else
		{
			if (mHotItem != mSelectedItem)
			{
				if (mSelectedItem)
				{
					mSelectedItem->mSelected = false;
				}
				mSelectedItem = mHotItem;
				mSelectedItem->mHot = false;
				mSelectedItem->mSelected = true;
				RedrawWindow(nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW);
			}

			SetCapture();
			mDragEntered = false;
		}
	}
	else if (mSelectedItem)
	{
		mSelectedItem->mSelected = false;
		mSelectedItem = nullptr;
		Invalidate(FALSE);
	}
}

void LibraryView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		if (mDragEntered)
		{
			ClientToScreen(&point);

			CWnd* wndUnderCursor = WindowFromPoint(point);
			if (wndUnderCursor && mSelectedItem)
			{
				IBlockDropTarget* dropTarget = dynamic_cast<IBlockDropTarget*>(wndUnderCursor);
				if (dropTarget && mSelectedItem->GetItemType() == ITEM_TYPE_ELEMENT)
				{
					Element* element = static_cast<Element*>(mSelectedItem);
					dropTarget->OnBlockDrop(element->mRttiName, point);
				}
			}
		}

		ReleaseCapture();
	}
}

void LibraryView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	UpdateHotItem(point);

	if (mHotItem)
	{
		point.y += mScrollBar.GetScrollPosition();

		if (mHotItem->GetItemType() == ITEM_TYPE_GROUP)
		{
			Group* group = static_cast<Group*>(mHotItem);
			group->ToggleExpasion();
			UpdateGeometricParameters(false);
			Invalidate(FALSE);
		}
	}
}

void LibraryView::OnCaptureChanged(CWnd* capturingWnd)
{
	if (capturingWnd != this)
	{
		if (mDragEntered)
		{
			CImageList::DragLeave(0);
			CImageList::EndDrag();
		}
	}

	mDragEntered = false;
	mTrackingMouseLeave = false;
}

void LibraryView::OnPaint()
{
	CPaintDC dc(this);

	Render();
}

void LibraryView::Render()
{
	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_SEMI_WHITE);
	auto context = mRenderer.GetContext();

	D2D1_MATRIX_3X2_F transform;
	context->GetTransform(&transform);

	D2D1_MATRIX_3X2_F translate = D2D1::Matrix3x2F::Identity();
	translate._31 = 0;
	translate._32 = -static_cast<FLOAT>(mScrollBar.GetScrollPosition());
	context->SetTransform(translate);

	for (auto& group : mGroups)
	{
		group->Draw(this);
	}

	context->SetTransform(transform);

	mRenderer.EndDraw();
	mRenderer.Display();
}

LibraryView::Item::Item()
	:
	mHot(false),
	mSelected(false)
{
}

LibraryView::Element::Element(const wchar_t* rttiName)
	:
	mRttiName(rttiName)
{
	mBitmap = BlockPresenter::GetCanonicalImage(rttiName);
}

LibraryView::ItemType LibraryView::Element::GetItemType() const
{
	return ITEM_TYPE_ELEMENT;
}

LibraryView::Item* LibraryView::Element::FindHit(int offset, CPoint point, LibraryView* view)
{
	CRect rect(mRect);
	rect.OffsetRect(0, offset);
	if (rect.PtInRect(point))
	{
		return this;
	}
	return nullptr;
}

void LibraryView::Element::Draw(int offset, LibraryView* view)
{
	if (mHot)
	{
		view->mBrush->SetColor(VisualManager::CLR_ORANGE);
	}
	else if (mSelected)
	{
		view->mBrush->SetColor(VisualManager::CLR_DARK_BLUE);
	}
	else
	{
		view->mBrush->SetColor(VisualManager::CLR_LEVEL1);
	}

	int width = mRect.right - mRect.left;
	int iconOffset = (width - ELEMENT_ICON_SIZE) / 2;

	D2D1_RECT_F rect;
	rect.left = static_cast<FLOAT>(mRect.left + iconOffset);
	rect.top = static_cast<FLOAT>(mRect.top + offset);
	rect.right = rect.left + ELEMENT_ICON_SIZE;
	rect.bottom = rect.top + ELEMENT_ICON_SIZE;
	view->mRenderer.DrawBitmap(mBitmap, rect);

	if (mHot || mSelected)
	{
		rect.left += 1;
		rect.top += 1;
		view->mRenderer.DrawRectangle(rect, view->mBrush);
	}
}

LibraryView::Group::Group(const wchar_t* displayName)
	:
	mDisplayName(displayName)
{
	mExpanded = true;
	mNumRows = 0;
	mContentExtent = 0;
	mOffset = 0;
}

LibraryView::ItemType LibraryView::Group::GetItemType() const
{
	return ITEM_TYPE_GROUP;
}

int LibraryView::Group::GetExtent() const
{
	int extent = GROUP_HEIGHT;
	if (mExpanded)
	{
		extent += mContentExtent;
	}
	return extent;
}

void LibraryView::Group::UpdateOffset(int offset)
{
	mOffset = offset;
}

void LibraryView::Group::Reconfigure(int offset, LibraryView* view)
{
	mOffset = offset;

	int numElements = (int)mElements.size();
	mNumRows = numElements / view->mNumColumns;
	mNumRows += (numElements % view->mNumColumns) ? 1 : 0;

	mContentExtent = (mNumRows * ELEMENT_HEIGHT) + ((mNumRows + 1) * VERTICAL_MARGIN);

	int elementIndex = 0;
	int elementY;
	for (int r = 0; r < mNumRows; ++r)
	{
		if (elementIndex >= numElements)
		{
			break;
		}

		elementY = GROUP_HEIGHT + VERTICAL_MARGIN + r * (VERTICAL_MARGIN + ELEMENT_HEIGHT);
		for (int c = 0; c < view->mNumColumns; ++c)
		{
			if (elementIndex >= numElements)
			{
				break;
			}

			double elementX = view->mIntercolumnGap + c * (view->mIntercolumnGap + ELEMENT_WIDTH);
			int left = static_cast<int>(elementX);
			mElements[elementIndex]->mRect.left = left;
			mElements[elementIndex]->mRect.right = left + ELEMENT_WIDTH;
			mElements[elementIndex]->mRect.top = elementY;
			mElements[elementIndex]->mRect.bottom = elementY + ELEMENT_HEIGHT;

			elementIndex++;
		}
	}
}

void LibraryView::Group::ToggleExpasion()
{
	mExpanded = !mExpanded;
}

bool LibraryView::Group::ConsumeLButtonDown(CPoint point, LibraryView* view)
{
	CRect rect;
	rect.left = 7;
	rect.top = mOffset + 6;
	rect.right = rect.left + 9;
	rect.bottom = rect.top + 9;
	if (rect.PtInRect(point))
	{
		mExpanded = !mExpanded;
		return true;
	}
	return false;
}

LibraryView::Item* LibraryView::Group::FindHit(CPoint point, LibraryView* view)
{
	CRect rect;
	rect.left = 0;
	rect.right = view->mWidth;
	rect.top = mOffset;
	rect.bottom = rect.top + GROUP_HEIGHT;
	if (rect.PtInRect(point)) return this;

	if (mExpanded)
	{
		for (auto& element : mElements)
		{
			Item* hit = element->FindHit(mOffset, point, view);
			if (hit) return hit; // hit found
		}
	}

	return nullptr;
}

void LibraryView::Group::Draw(LibraryView* view)
{
	D2D1_RECT_F rect;
	rect.left = 0;
	rect.top = static_cast<FLOAT>(mOffset);
	rect.right = static_cast<FLOAT>(view->mWidth - ScrollBar::FIXED_SIZE);
	rect.bottom = static_cast<FLOAT>(mOffset + GROUP_HEIGHT);

	view->mBrush->SetColor(VisualManager::CLR_BASE);
	view->mRenderer.FillRectangle(rect, view->mBrush);

	D2D1_RECT_F iconRect;
	iconRect.left = 7;
	iconRect.top = mOffset + 6.0f;
	iconRect.right = iconRect.left + 9;
	iconRect.bottom = iconRect.top + 9;
	if (mExpanded)
	{
		view->mRenderer.DrawBitmap(view->mIconExpaned, iconRect);
	}
	else
	{
		view->mRenderer.DrawBitmap(view->mIconCollapsed, iconRect);
	}

	rect.left += 25;
	rect.top += 1;
	rect.bottom -= 2;
	view->mBrush->SetColor(VisualManager::CLR_LEVEL1);
	view->mRenderer.DrawText(mDisplayName,
		static_cast<UINT32>(wcslen(mDisplayName)),
		DWrite::segoe12B, rect, view->mBrush);

	if (mExpanded)
	{
		for (auto& element : mElements)
		{
			element->Draw(mOffset, view);
		}
	}
}

} // namespace hsmn
