#include "hsuiPrecompiled.h"
#include "hsuiListCtrl.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscWicBitmap.h>
#include <xscDWrite.h>

using namespace std;
using namespace xsc;

namespace hsui {

static const int DEFAULT_COLUMN_WIDTH = 100;
static const int FIXED_ITEM_HEIGHT = 21;
static const int FIXED_HEADER_HEIGHT = 20;
static const int FIXED_ICON_IMAGE_SIZE = 20;

const int ListCtrl::ITEM_IMAGE_NONE = -2;
const int ListCtrl::ITEM_IMAGE_DEFAULT = -1;

BEGIN_MESSAGE_MAP(ListCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

ListCtrl::ListCtrl()
{
	mListener = nullptr;

	mHeaderFill = nullptr;
	mDefaultIcon = nullptr;

	mHotItem = -1;
	mSelectedItem = -1;
	mShowActive = true;

	mFocused = false;
	mTrackingMouseLeave = false;

	mBorderBrush = nullptr;
	mItemBrushGeneric = nullptr;
	mItemBrushHot = nullptr;
	mItemBrushSelectedFocused = nullptr;
	mItemBrushSelectedUnfocused = nullptr;
	mTextBrushHeader = nullptr;
	mTextBrushActive = nullptr;
	mTextBrushNormal = nullptr;
	
	mResizingColumnIndex = -1;

	mShowScrollBar[ScrollBar::TYPE_HORZ] = true;
	mShowScrollBar[ScrollBar::TYPE_VERT] = true;
	mScrollBarSize[ScrollBar::TYPE_HORZ] = ScrollBar::FIXED_SIZE;
	mScrollBarSize[ScrollBar::TYPE_VERT] = ScrollBar::FIXED_SIZE;
}

ListCtrl::~ListCtrl()
{
}

void ListCtrl::SetListener(ListCtrl::IListener* listener)
{
	mListener = listener;
}

void ListCtrl::ResetListener()
{
	mListener = nullptr;
}

void ListCtrl::OnScrollPositionChanged(ScrollBar* scrollBar)
{
	Render();
}

void ListCtrl::CreateWnd(CWnd* parent, const vector<Column>& columns, int x, int y, int w, int h)
{
	assert(GetSafeHwnd() == nullptr);

	mColumns = columns;

	CRect rect(x, y, x + w, y + h);
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	Create(nullptr, nullptr, style, rect, parent, 0);
}

void ListCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

void ListCtrl::ResizeWnd(int w, int h)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void ListCtrl::MoveAndResizeWnd(int x, int y, int w, int h)
{
	assert(GetSafeHwnd());
	SetWindowPos(nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

void ListCtrl::Refresh()
{
	if (GetSafeHwnd())
	{
		Invalidate(FALSE);
	}
}

void ListCtrl::SetShowSelected(bool show)
{
	if (mShowActive != show)
	{
		mShowActive = show;
	}

	Refresh();
}

void ListCtrl::ShowHorzScrollBar(bool show)
{
	assert(GetSafeHwnd());

	if (mShowScrollBar[ScrollBar::TYPE_HORZ] != show)
	{
		mShowScrollBar[ScrollBar::TYPE_HORZ] = show;
		if (show)
		{
			mScrollBarSize[ScrollBar::TYPE_HORZ] = ScrollBar::FIXED_SIZE;
		}
		else
		{
			mScrollBarSize[ScrollBar::TYPE_HORZ] = 0;
		}

		mHorz.SetScrollPosition(0);
		if (show)
		{
			mHorz.ShowWindow(SW_SHOW);
		}
		else
		{
			mHorz.ShowWindow(SW_HIDE);
		}
		
		AdjustScrollBarGeometryByVisibilities();
		UpdateScrollParameters();
	}
}

void ListCtrl::ShowVertScrollBar(bool show)
{
	assert(GetSafeHwnd());

	if (mShowScrollBar[ScrollBar::TYPE_VERT] != show)
	{
		mShowScrollBar[ScrollBar::TYPE_VERT] = show;
		if (show)
		{
			mScrollBarSize[ScrollBar::TYPE_VERT] = ScrollBar::FIXED_SIZE;
		}
		else
		{
			mScrollBarSize[ScrollBar::TYPE_VERT] = 0;
		}

		mVert.SetScrollPosition(0);
		if (show)
		{
			mVert.ShowWindow(SW_SHOW);
		}
		else
		{
			mVert.ShowWindow(SW_HIDE);
		}
		
		AdjustScrollBarGeometryByVisibilities();
		UpdateScrollParameters();
	}
}

void ListCtrl::AdjustScrollBarGeometryByVisibilities()
{
	CRect rect;
	GetClientRect(&rect);
	int cx = rect.Width();
	int cy = rect.Height();

	if (mShowScrollBar[ScrollBar::TYPE_HORZ])
	{
		if (mShowScrollBar[ScrollBar::TYPE_VERT])
		{
			mHorz.MoveAndResizeWnd(0, cy - ScrollBar::FIXED_SIZE, cx - ScrollBar::FIXED_SIZE);
			mVert.MoveAndResizeWnd(
				cx - ScrollBar::FIXED_SIZE, FIXED_HEADER_HEIGHT,
				cy - FIXED_HEADER_HEIGHT - ScrollBar::FIXED_SIZE);
		}
		else
		{
			mHorz.MoveAndResizeWnd(0, cy - ScrollBar::FIXED_SIZE, cx);
		}
	}
	else
	{
		if (mShowScrollBar[ScrollBar::TYPE_VERT])
		{
			mVert.MoveAndResizeWnd(cx - ScrollBar::FIXED_SIZE, FIXED_HEADER_HEIGHT, cy - FIXED_HEADER_HEIGHT);
		}
	}
}

void ListCtrl::SetColumnWidth(int column, int width)
{
	assert(GetSafeHwnd());
	assert(column >= 0);
	assert(column < ((int)mColumns.size()));

	if (width < 10)
	{
		mColumns[column].width = 10;
	}
	else
	{
		mColumns[column].width = width;
	}

	UpdateScrollParameters();
}

void ListCtrl::SetColumnText(int column, const std::wstring& text)
{
	assert(GetSafeHwnd());
	assert(column >= 0);
	assert(column < ((int)mColumns.size()));

	mColumns[column].text = text;
}

void ListCtrl::SetItemText(int item, int subitem, const std::wstring& text)
{
	assert(GetSafeHwnd());
	assert(subitem >= 0);
	assert(subitem < ((int)mColumns.size()));
	assert(item < ((int)mItems.size()));

	int itemIndex = 0;

	list<Item>::iterator iter;
	for (iter = mItems.begin(); iter != mItems.end(); ++iter)
	{
		if (itemIndex == item)
		{
			iter->subitems[subitem].text = text;
			break;
		}
		++itemIndex;
	}
}

void ListCtrl::SetItemImage(int item, int image)
{
	assert(GetSafeHwnd());
	assert(item < ((int)mItems.size()));

	int itemIndex = 0;
	list<Item>::iterator iter;
	for (iter = mItems.begin(); iter != mItems.end(); ++iter)
	{
		if (itemIndex == item)
		{
			iter->image = image;
			break;
		}

		++itemIndex;
	}
}

void* ListCtrl::GetSelectedItemData() const
{
	int itemIndex = 0;
	for (const auto& item : mItems)
	{
		if (itemIndex == mSelectedItem)
		{
			return item.data;
		}

		itemIndex++;
	}
	return nullptr;
}

int ListCtrl::AddItem(const ListCtrl::Item& item)
{
	assert(GetSafeHwnd());
	assert(item.subitems.size() == mColumns.size());

	int itemIndex = (int)mItems.size();
	mItems.push_back(item);
	
	UpdateScrollParameters();
	
	return itemIndex;
}

void ListCtrl::RemoveItem(int item)
{
	assert(GetSafeHwnd());
	assert(0 <= item && item < ((int)mItems.size()));

	int itemIndex = 0;
	list<Item>::const_iterator iter;
	for (iter = mItems.begin(); iter != mItems.end(); ++iter)
	{
		if (itemIndex == item)
		{
			mItems.erase(iter);
			break;
		}

		++itemIndex;
	}

	UpdateScrollParameters();
}

void ListCtrl::RemoveAllItems()
{
	assert(GetSafeHwnd());

	mItems.clear();
	
	mHotItem = -1;
	mSelectedItem = -1;

	UpdateScrollParameters();
}

int ListCtrl::GetNumItems() const
{
	return static_cast<int>(mItems.size());
}

void ListCtrl::UpdateScrollParameters()
{
	int viewWidth = mClientWidth - mScrollBarSize[ScrollBar::TYPE_VERT];
	int viewHeight = mClientHeight - mScrollBarSize[ScrollBar::TYPE_HORZ] - FIXED_HEADER_HEIGHT;

	int worldWidth = 0;
	for (auto& column : mColumns)
	{
		worldWidth += column.width;
	}
	int worldHeight = (int)(mItems.size() * FIXED_ITEM_HEIGHT);

	mHorz.SetRange(0, worldWidth - viewWidth);
	mVert.SetRange(0, worldHeight - viewHeight - 1);

	mHorz.SetPageSize(viewWidth);
	mVert.SetPageSize(viewHeight);
}

int ListCtrl::SelectHitItem(CPoint point)
{
	int xWorldPoint = point.x + mHorz.GetScrollPosition();
	int yWorldPoint = point.y + mVert.GetScrollPosition() - FIXED_HEADER_HEIGHT;
	CPoint worldPoint(xWorldPoint, yWorldPoint);

	int itemWidth = 0;
	for (auto& column : mColumns)
	{
		itemWidth += column.width;
	}

	CRect itemRect;
	itemRect.left = 0;
	itemRect.right = itemWidth;
	int itemIndex = 0;
	list<Item>::const_iterator iter;
	for (iter = mItems.begin(); iter != mItems.end(); ++iter)
	{
		itemRect.top = itemIndex * FIXED_ITEM_HEIGHT;
		itemRect.bottom = itemRect.top + FIXED_ITEM_HEIGHT;
		if (itemRect.PtInRect(worldPoint))
		{
			return itemIndex;
			break;
		}

		++itemIndex;
	}

	return -1;
}

bool ListCtrl::UpdateColumnResizingParameters(CPoint point)
{
	int xWorldPoint = point.x + mHorz.GetScrollPosition();
	CPoint worldPoint(xWorldPoint, point.y);

	CRect resizeRect;
	resizeRect.top = 0;
	resizeRect.bottom = FIXED_HEADER_HEIGHT;

	int columnOffset = 0;
	int columnIndex = 0;
	for (auto& column : mColumns)
	{
		resizeRect.left = columnOffset + column.width - 2;
		resizeRect.right = resizeRect.left + 4;
		if (resizeRect.PtInRect(worldPoint))
		{
			mResizingColumnIndex = columnIndex;
			mResizingPivotWidth = column.width;
			mResizingPivotPosition = point.x;
			return true;
		}

		columnOffset += column.width;
		++columnIndex;
	}

	return false;
}

int ListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mResizingCursor = LoadCursor(hsui::Module::handle, MAKEINTRESOURCE(HSUI_IDR_RESIZE_CURSOR_H));

	mHorz.SetListener(static_cast<ScrollBar::IListener*>(this));
	mHorz.CreateWnd(this, 0, 0, 100);
	mHorz.SetStepSize(FIXED_ITEM_HEIGHT);

	mVert.SetListener(static_cast<ScrollBar::IListener*>(this));
	mVert.CreateWnd(this, 0, 0, 100);
	mVert.SetStepSize(FIXED_ITEM_HEIGHT);

	mClientWidth = lpCreateStruct->cx;
	mClientHeight = lpCreateStruct->cy;

	mRenderer.Create(m_hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL3, &mBorderBrush);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mTextBrushHeader);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_WHITE, &mTextBrushActive);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mTextBrushNormal);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_ORANGE, &mItemBrushHot);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BLUE, &mItemBrushSelectedFocused);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BASE, &mItemBrushSelectedUnfocused);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_BASE, &mItemBrushGeneric);

	WicBitmap wicBitmap;
	
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_LIST_HEADER_FILL);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mHeaderFill);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_LIST_ITEM_ICON);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mDefaultIcon);

	return 0;
}

void ListCtrl::OnDestroy()
{
	SafeRelease(mDefaultIcon);
	SafeRelease(mHeaderFill);
	SafeRelease(mBorderBrush);
	SafeRelease(mItemBrushGeneric);
	SafeRelease(mTextBrushHeader);
	SafeRelease(mTextBrushActive);
	SafeRelease(mTextBrushNormal);
	SafeRelease(mItemBrushHot);
	SafeRelease(mItemBrushSelectedUnfocused);
	SafeRelease(mItemBrushSelectedFocused);
	for (auto image : mImageList)
	{
		image->Release();
	}
	mImageList.clear();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void ListCtrl::OnSize(UINT nType, int cx, int cy)
{
	mClientWidth = cx;
	mClientHeight = cy;
	mRenderer.Resize(cx, cy);

	AdjustScrollBarGeometryByVisibilities();
	UpdateScrollParameters();
}

BOOL ListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (mResizingColumnIndex >= 0 || GetCapture() == this)
	{
		SetCursor(mResizingCursor);
		return TRUE;
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

BOOL ListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	mVert.SetScrollPosition(mVert.GetScrollPosition() - int(zDelta * 0.1f));

	GetCursorPos(&pt);
	ScreenToClient(&pt);
	mHotItem = SelectHitItem(pt);
	
	RedrawWindow();

	return TRUE;
}

void ListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	mFocused = true;
	if (mSelectedItem != -1)
	{
		Invalidate(FALSE);
	}
}

void ListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	mFocused = false;
	if (mSelectedItem != -1)
	{
		Invalidate(FALSE);
	}
}

void ListCtrl::OnMouseMove(UINT nFlags, CPoint point)
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

	if (GetCapture() == this)
	{
		int columnIndex = 0;
		for (auto& column : mColumns)
		{
			if (columnIndex == mResizingColumnIndex)
			{
				int delta = point.x - mResizingPivotPosition;
				column.width = mResizingPivotWidth + delta;
				if (column.width < 12)
				{
					column.width = 12;
				}

				UpdateScrollParameters();
				Invalidate(FALSE);
				break;
			}

			++columnIndex;
		}
	}
	else
	{
		mResizingColumnIndex = -1;

		if (point.y <= FIXED_HEADER_HEIGHT)
		{
			if (mHotItem != -1)
			{
				mHotItem = -1;
				Invalidate(FALSE);
			}

			if (UpdateColumnResizingParameters(point))
			{
				SetCursor(mResizingCursor);
			}
		}
		else
		{
			int hotItem = SelectHitItem(point);
			if (hotItem != mHotItem)
			{
				mHotItem = hotItem;
				Invalidate(FALSE);
			}
		}
	}
}

void ListCtrl::OnMouseLeave()
{
	mTrackingMouseLeave = false;
	mResizingColumnIndex = -1;
	
	mHotItem = -1;
	Invalidate(FALSE);
}

void ListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	if (point.y <= FIXED_HEADER_HEIGHT)
	{
		if (UpdateColumnResizingParameters(point))
		{
			SetCursor(mResizingCursor);
			SetCapture();
		}
		return;
	}

	mResizingColumnIndex = -1;

	int hotItem = SelectHitItem(point);
	if (hotItem != -1)
	{
		mHotItem = hotItem;
		mSelectedItem = hotItem;
		Invalidate(FALSE);

		if (mListener) mListener->OnSelectionChanged(this);
	}
}

void ListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		ReleaseCapture();
	}
	
	mResizingColumnIndex = -1;
	if (UpdateColumnResizingParameters(point))
	{
		SetCursor(mResizingCursor);
	}

	mTrackingMouseLeave = false;
}

void ListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int hitItem = SelectHitItem(point);
	if (hitItem != mSelectedItem)
	{
		mSelectedItem = hitItem;
		Invalidate(FALSE);
	}

	if (mListener) mListener->OnItemDoubleClicked(this);
}

void ListCtrl::BuildImageList(HMODULE module, int numItems, const UINT* list)
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

BOOL ListCtrl::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void ListCtrl::OnPaint()
{
	CPaintDC dc(this);

	Render();
}

void ListCtrl::Render()
{
	D2D1_RECT_F rect;

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_SEMI_WHITE);

	int xOffset = mHorz.GetScrollPosition();
	int yOffset = mVert.GetScrollPosition();
	
	int lastColumn = static_cast<int>(mColumns.size()) - 1;
	int lastItem = static_cast<int>(mItems.size()) - 1;
	int columnOffset = 0;
	int columnIndex = 0;

	int visibleRangeStart = yOffset;
	int visibleRangeEnd = yOffset + mClientHeight -
		FIXED_HEADER_HEIGHT - mScrollBarSize[ScrollBar::TYPE_HORZ];
	int itemViewPos, itemWorldStart, itemWorldEnd;
	int itemIndex = 0;
	list<Item>::const_iterator iter;
	IDWriteTextFormat* itemTextFormat = DWrite::segoe12R;
	for (iter = mItems.begin(); iter != mItems.end(); ++iter)
	{
		itemWorldStart = itemIndex * FIXED_ITEM_HEIGHT;
		itemWorldEnd = itemWorldStart + FIXED_ITEM_HEIGHT;

		bool draw = false;

		if (itemWorldStart <= visibleRangeStart)
		{
			if (itemWorldEnd >= visibleRangeStart)
			{
				draw = true;
			}
		}
		else if (itemWorldStart <= visibleRangeEnd) 
		{
			draw = true;
		}

		if (false == draw)
		{
			++itemIndex;
			continue;
		}

		itemViewPos = FIXED_HEADER_HEIGHT + itemWorldStart - yOffset;

		rect.top = static_cast<FLOAT>(itemViewPos);
		rect.bottom = rect.top + FIXED_ITEM_HEIGHT;

		columnOffset = 0;
		columnIndex = 0;
		for (auto& column : mColumns)
		{
			rect.left = static_cast<FLOAT>(columnOffset - xOffset);
			rect.right = rect.left + column.width + ((lastColumn == columnIndex) ? 1 : 0);
			columnOffset += column.width;

			if (iter->subitems[columnIndex].useBackgroundColor)
			{
				D2D1_RECT_F coloredBackgroundRect;
				coloredBackgroundRect = rect;
				mItemBrushGeneric->SetColor(iter->subitems[columnIndex].backgroundColor);
				mRenderer.FillRectangle(coloredBackgroundRect, mItemBrushGeneric);
			}

			if (mShowActive)
			{
				if (itemIndex == mSelectedItem)
				{
					if (mFocused)
					{
						mRenderer.FillRectangle(rect, mItemBrushSelectedFocused);
					}
					else
					{
						mRenderer.FillRectangle(rect, mItemBrushSelectedUnfocused);
					}
				}
				else if (itemIndex == mHotItem)
				{
				}
			}
			else
			{
				if (itemIndex == mHotItem)
				{
					mRenderer.FillRectangle(rect, mItemBrushHot);
				}
			}

			mRenderer.DrawRectangle(rect, mBorderBrush);

			D2D1_RECT_F textRect = rect;
			
			if (columnIndex == 0)
			{
				D2D1_RECT_F iconRect = rect;
				iconRect.right = iconRect.left + FIXED_ICON_IMAGE_SIZE;
				iconRect.bottom = iconRect.top + FIXED_ICON_IMAGE_SIZE;

				if (iter->image >= 0)
				{
					int numImages = static_cast<int>(mImageList.size());
					if (iter->image < numImages)
					{
						mRenderer.DrawBitmap(mImageList[iter->image], &iconRect);
						textRect.left = iconRect.right + 4;
					}
					else
					{
						textRect.left += 4;
					}
				}
				else
				{
					if (iter->image == ITEM_IMAGE_DEFAULT)
					{
						mRenderer.DrawBitmap(mDefaultIcon, &iconRect);
						textRect.left = iconRect.right + 4;
					}
					else
					{
						textRect.left += 4;
					}
				}
			}
			else
			{
				textRect.left += 4;
			}
			textRect.right -= 4;
			textRect.top -= 1;
			textRect.bottom -= 1;

			if (itemIndex == mSelectedItem)
			{
				if (mFocused)
				{
					mRenderer.DrawText(iter->subitems[columnIndex].text.c_str(),
						static_cast<UINT32>(iter->subitems[columnIndex].text.length()),
						itemTextFormat, textRect, mTextBrushActive, D2D1_DRAW_TEXT_OPTIONS_CLIP);
				}
				else
				{
					mRenderer.DrawText(iter->subitems[columnIndex].text.c_str(),
						static_cast<UINT32>(iter->subitems[columnIndex].text.length()),
						itemTextFormat, textRect, mTextBrushNormal, D2D1_DRAW_TEXT_OPTIONS_CLIP);
				}
			}
			else if (itemIndex == mHotItem)
			{
				mRenderer.DrawText(iter->subitems[columnIndex].text.c_str(),
					static_cast<UINT32>(iter->subitems[columnIndex].text.length()),
					itemTextFormat, textRect, mTextBrushNormal, D2D1_DRAW_TEXT_OPTIONS_CLIP);
			}
			else
			{
				if (iter->subitems[columnIndex].useForegroundColor)
				{
					mTextBrushNormal->SetColor(iter->subitems[columnIndex].foregroundColor);
				}
				mRenderer.DrawText(iter->subitems[columnIndex].text.c_str(),
					static_cast<UINT32>(iter->subitems[columnIndex].text.length()),
					itemTextFormat, textRect, mTextBrushNormal, D2D1_DRAW_TEXT_OPTIONS_CLIP);
				if (iter->subitems[columnIndex].useForegroundColor)
				{
					mTextBrushNormal->SetColor(VisualManager::CLR_LEVEL3);
				}
			}

			++columnIndex;
		}

		++itemIndex;
	}

	IDWriteTextFormat* headerTextFormat = DWrite::segoe12R;
	headerTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

	rect.top = 0.0f;
	rect.bottom = static_cast<float>(FIXED_HEADER_HEIGHT);
	rect.left = 0.0f;
	rect.right = static_cast<float>(mClientWidth);
	mRenderer.DrawBitmap(mHeaderFill, &rect);

	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	
	auto borderRect = rect;
	borderRect.right += 1;
	mRenderer.DrawRectangle(borderRect, mBorderBrush);

	columnOffset = 0;
	columnIndex = 0;
	
	for (auto& column : mColumns)
	{
		rect.left = static_cast<FLOAT>(columnOffset - xOffset);
		rect.right = rect.left + column.width + ((lastColumn == columnIndex) ? 1 : 0);
		columnOffset += column.width;
		
		D2D1_RECT_F textRect = rect;
		textRect.top -= 1.0f;
		textRect.bottom -= 1.0f;
		mRenderer.DrawText(column.text.c_str(), static_cast<UINT32>(column.text.length()),
			headerTextFormat, textRect, mTextBrushHeader, D2D1_DRAW_TEXT_OPTIONS_CLIP);

		mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		mRenderer.DrawRectangle(rect, mBorderBrush);

		++columnIndex;
	}

	headerTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	if (mShowScrollBar[ScrollBar::TYPE_HORZ] &&
		mShowScrollBar[ScrollBar::TYPE_VERT])
	{
		rect.left = static_cast<FLOAT>(mClientWidth - ScrollBar::FIXED_SIZE);
		rect.right = static_cast<FLOAT>(mClientWidth);
		rect.top = static_cast<FLOAT>(mClientHeight - ScrollBar::FIXED_SIZE);
		rect.bottom = static_cast<FLOAT>(mClientHeight);
		mRenderer.FillRectangle(rect, mItemBrushGeneric);
	}

	mRenderer.EndDraw();

	mRenderer.Display();
}

ListCtrl::Column::Column()
{
	width = DEFAULT_COLUMN_WIDTH;
	image = -1;
}

ListCtrl::Column::Column(const wstring& argText, int argWidth, int argImage)
{
	text = argText;
	width = argWidth;
	image = argImage;
}

ListCtrl::Column::Column(const ListCtrl::Column& column)
{
	text = column.text;
	width = column.width;
	image = column.image;
}

ListCtrl::Column& ListCtrl::Column::operator=(const ListCtrl::Column& column)
{
	text = column.text;
	width = column.width;
	image = column.image;
	return *this;
}

ListCtrl::Column::Column(ListCtrl::Column&& column)
{
	text = std::move(column.text);
	width = column.width;
	image = column.image;
}

ListCtrl::Column& ListCtrl::Column::operator=(ListCtrl::Column&& column)
{
	text = std::move(column.text);
	width = column.width;
	image = column.image;
	return *this;
}

ListCtrl::Subitem::Subitem()
{
	text = L"";

	useBackgroundColor = false;
	backgroundColor = VisualManager::CLR_BASE;
	
	useForegroundColor = false;
	foregroundColor = VisualManager::CLR_LEVEL3;
}

ListCtrl::Subitem::Subitem(const ListCtrl::Subitem& subitem)
{
	text = subitem.text;

	useBackgroundColor = subitem.useBackgroundColor;
	backgroundColor = subitem.backgroundColor;
	
	useForegroundColor = subitem.useForegroundColor;
	foregroundColor = subitem.foregroundColor;
}

ListCtrl::Subitem& ListCtrl::Subitem::operator=(const ListCtrl::Subitem& subitem)
{
	text = subitem.text;

	useBackgroundColor = subitem.useBackgroundColor;
	backgroundColor = subitem.backgroundColor;

	useForegroundColor = subitem.useForegroundColor;
	foregroundColor = subitem.foregroundColor;

	return *this;
}

ListCtrl::Subitem::Subitem(ListCtrl::Subitem&& subitem)
{
	text = std::move(subitem.text);

	useBackgroundColor = subitem.useBackgroundColor;
	backgroundColor = subitem.backgroundColor;

	useForegroundColor = subitem.useForegroundColor;
	foregroundColor = subitem.foregroundColor;
}

ListCtrl::Subitem& ListCtrl::Subitem::operator=(ListCtrl::Subitem&& subitem)
{
	text = std::move(subitem.text);

	useBackgroundColor = subitem.useBackgroundColor;
	backgroundColor = subitem.backgroundColor;

	useForegroundColor = subitem.useForegroundColor;
	foregroundColor = subitem.foregroundColor;

	return *this;
}

ListCtrl::Item::Item()
{
	image = ListCtrl::ITEM_IMAGE_DEFAULT;

	data = nullptr;
}

ListCtrl::Item::Item(const std::vector<Subitem>& argSubitems,
	int argImage, void* argData)
{
	subitems = argSubitems;

	image = argImage;
	data = argData;
}

ListCtrl::Item::Item(const ListCtrl::Item& item)
{
	subitems = item.subitems;

	image = item.image;
	data = item.data;
}

ListCtrl::Item& ListCtrl::Item::operator=(const ListCtrl::Item& item)
{
	subitems = item.subitems;

	image = item.image;
	data = item.data;

	return *this;
}

ListCtrl::Item::Item(ListCtrl::Item&& item)
{
	subitems = std::move(item.subitems);

	image = item.image;
	data = item.data;
}

ListCtrl::Item& ListCtrl::Item::operator=(ListCtrl::Item&& item)
{
	subitems = std::move(item.subitems);

	image = item.image;
	data = item.data;
	
	return *this;
}

} // namespace hsui
