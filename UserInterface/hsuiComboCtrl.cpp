#include <hsuiPrecompiled.h>
#include <hsuiComboCtrl.h>
#include <hsuiVisualManager.h>
#include <hsuiResource.h>
#include <xscWicBitmap.h>
#include <xscDWrite.h>

using namespace std;
using namespace xsc;

namespace hsui {

static const int MAX_LIST_HEIGHT = 200;
static const int FIXED_ITEM_HEIGHT = 20;
static const int COMMON_GADGET_SIZE = 20;
static const int FIXED_SCROLL_THUMB_HEIGHT = 30;

class ItemListWnd : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	ItemListWnd();
	virtual ~ItemListWnd();

	void CreateWnd(ComboCtrl* owner);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	void DrawScrollBar(const CRect& clientRect);
	void DrawContent(const CRect& clientRect);

	CRect CalcScrollBarRect(const CRect& clientRect) const;
	CRect CalcScrollUpRect(const CRect& clientRect) const;
	CRect CalcScrollDownRect(const CRect& clientRect) const;
	CRect CalcScrollThumbRect(const CRect& clientRect) const;

	void UpdatePointerState(const CRect& clientRect, CPoint point);
	void UpdateScrollPosition(int scrollPos);
	void UpdateHotItem(CPoint point);

	ComboCtrl* mOwner;
	int mNumItems;
	int mHotItem;

	xsc::D2DRenderer mRenderer;

	ID2D1SolidColorBrush* mBrush;
	ID2D1Bitmap* mArrowUN;
	ID2D1Bitmap* mArrowUH;
	ID2D1Bitmap* mArrowDN;
	ID2D1Bitmap* mArrowDH;

	enum PointerState
	{
		POINTER_OUTSIDE,
		POINTER_CONTENT,
		POINTER_SCROLL_PAGE,
		POINTER_SCROLL_UP,
		POINTER_SCROLL_DOWN,
		POINTER_SCROLL_THUMB,
		POINTER_SCROLLING
	};
	PointerState mPointerState;

	bool mScrolled;
	int mScrollRange;
	int mScrollPosition;

	CPoint mScrollingDownPoint;
	int mScrollingDownPosition;
};

BEGIN_MESSAGE_MAP(ItemListWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

ItemListWnd::ItemListWnd()
{
	mPointerState = POINTER_OUTSIDE;

	mScrolled = false;
	mScrollRange = 0;
	mScrollPosition = 0;

	mBrush = nullptr;

	mArrowUN = nullptr; mArrowUH = nullptr;
	mArrowDN = nullptr; mArrowDH = nullptr;
}

ItemListWnd::~ItemListWnd()
{
}

void ItemListWnd::CreateWnd(ComboCtrl* owner)
{
	mOwner = owner;

	DWORD style = WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

	CRect ownerRect;
	owner->GetWindowRect(&ownerRect);

	int x = ownerRect.left;
	int y = ownerRect.bottom + 1;
	int w = ownerRect.Width();
	mNumItems = (int)mOwner->mItems.size();
	int h = mNumItems * FIXED_ITEM_HEIGHT;
	if (h > MAX_LIST_HEIGHT)
	{
		mScrolled = true;
		mScrollRange = h - MAX_LIST_HEIGHT;
		h = MAX_LIST_HEIGHT;
	}

	CRect targetScreenRect;
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	if (GetMonitorInfoW(::MonitorFromPoint(CPoint(x, y), MONITOR_DEFAULTTONEAREST), &mi))
	{
		targetScreenRect = mi.rcWork;
	}
	else
	{
		SystemParametersInfoW(SPI_GETWORKAREA, 0, &targetScreenRect, 0);
	}
	int dropEnd = y + h;
	if (dropEnd > targetScreenRect.bottom)
	{
		y = ownerRect.top - h - 1;
	}

	mHotItem = -1;

	CreateEx(exStyle, AfxRegisterWndClass(0), nullptr, style, x, y, w, h, owner->m_hWnd, nullptr, nullptr);

	SetCapture();
}

int ItemListWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mRenderer.Create(m_hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mBrush);

	if (mScrolled)
	{
		WicBitmap wicBitmap;

		wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_COMBO_ARROW_UN);
		mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowUN);

		wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_COMBO_ARROW_UH);
		mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowUH);

		wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_COMBO_ARROW_DN);
		mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowDN);

		wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_COMBO_ARROW_DH);
		mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mArrowDH);
	}

	return 0;
}

void ItemListWnd::OnDestroy()
{
	SafeRelease(mBrush);
	SafeRelease(mArrowUN);
	SafeRelease(mArrowUH);
	SafeRelease(mArrowDN);
	SafeRelease(mArrowDH);
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void ItemListWnd::OnSize(UINT nType, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

void ItemListWnd::UpdatePointerState(const CRect& clientRect, CPoint point)
{
	if (clientRect.PtInRect(point))
	{
		CRect scrollBarRect(CalcScrollBarRect(clientRect));

		if (scrollBarRect.PtInRect(point))
		{
			CRect upRect(CalcScrollUpRect(clientRect));
			CRect downRect(CalcScrollDownRect(clientRect));
			CRect thumbRect(CalcScrollThumbRect(clientRect));
			PointerState oldPointerState = mPointerState;

			if (upRect.PtInRect(point))
			{
				mPointerState = POINTER_SCROLL_UP;
			}
			else if (downRect.PtInRect(point))
			{
				mPointerState = POINTER_SCROLL_DOWN;
			}
			else if (thumbRect.PtInRect(point))
			{
				mPointerState = POINTER_SCROLL_THUMB;
			}
			else
			{
				mPointerState = POINTER_SCROLL_PAGE;
			}

			if (mPointerState != oldPointerState)
			{
				Invalidate(FALSE);
			}
		}
		else
		{
			if (mPointerState != POINTER_CONTENT)
			{
				mPointerState = POINTER_CONTENT;
				Invalidate(FALSE);
			}
		}
	}
	else
	{
		if (mPointerState != POINTER_OUTSIDE)
		{
			mPointerState = POINTER_OUTSIDE;
			Invalidate(FALSE);
		}
	}
}

void ItemListWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	if (mScrolled)
	{
		if (mPointerState == POINTER_SCROLLING)
		{
			int viewDelta = point.y - mScrollingDownPoint.y;
			double viewScrollArea = clientRect.Height() - (2 * COMMON_GADGET_SIZE) - FIXED_SCROLL_THUMB_HEIGHT;
			double viewToLogical = mScrollRange / viewScrollArea;
			int scrollDelta = (int)(viewToLogical * viewDelta);
			UpdateScrollPosition(mScrollingDownPosition + scrollDelta);
		}
		else
		{
			UpdatePointerState(clientRect, point);
			if (mPointerState == POINTER_CONTENT)
			{
				UpdateHotItem(point);
			}
		}
	}
	else
	{
		if (clientRect.PtInRect(point))
		{
			if (mPointerState != POINTER_CONTENT)
			{
				mPointerState = POINTER_CONTENT;
				Invalidate(FALSE);
			}
			UpdateHotItem(point);
		}
		else
		{
			if (mPointerState != POINTER_OUTSIDE)
			{
				mPointerState = POINTER_OUTSIDE;
				Invalidate(FALSE);
			}
		}
	}
}

BOOL ItemListWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int oldScrollPosition = mScrollPosition;

	mScrollPosition -= int(zDelta * 0.1f);
	if (mScrollPosition < 0)
	{
		mScrollPosition = 0;
	}
	else if (mScrollPosition > mScrollRange)
	{
		mScrollPosition = mScrollRange;
	}

	CPoint point;
	::GetCursorPos(&point);
	ScreenToClient(&point);
	UpdateHotItem(point);

	if (oldScrollPosition != mScrollPosition)
	{
		RedrawWindow();
	}

	return TRUE;
}

void ItemListWnd::UpdateHotItem(CPoint point)
{
	int modelPosition = point.y + mScrollPosition;
	int itemIndex = modelPosition / FIXED_ITEM_HEIGHT;
	if (itemIndex >= mNumItems)
	{
		itemIndex = mNumItems - 1;
	}

	if (mHotItem != itemIndex)
	{
		mHotItem = itemIndex;
		Invalidate(FALSE);
	}
}

void ItemListWnd::UpdateScrollPosition(int scrollPos)
{
	int oldScrollPosition = mScrollPosition;

	if (scrollPos < 0)
	{
		mScrollPosition = 0;
	}
	else if (scrollPos > mScrollRange)
	{
		mScrollPosition = mScrollRange;
	}
	else
	{
		mScrollPosition = scrollPos;
	}

	if (mScrollPosition != oldScrollPosition)
	{
		Invalidate(FALSE);
	}
}

void ItemListWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (mPointerState == POINTER_SCROLL_UP)
	{
		UpdateScrollPosition(mScrollPosition - FIXED_ITEM_HEIGHT);
	}
	else if (mPointerState == POINTER_SCROLL_DOWN)
	{
		UpdateScrollPosition(mScrollPosition + FIXED_ITEM_HEIGHT);
	}
	else if (mPointerState == POINTER_SCROLL_PAGE)
	{
		CRect clientRect;
		GetClientRect(&clientRect);
		CRect thumbRect(CalcScrollThumbRect(clientRect));
		if (point.y < thumbRect.top)
		{
			UpdateScrollPosition(mScrollPosition - MAX_LIST_HEIGHT);
		}
		else
		{
			UpdateScrollPosition(mScrollPosition + MAX_LIST_HEIGHT);
		}
	}
	else if (mPointerState == POINTER_SCROLL_THUMB)
	{
		mScrollingDownPoint = point;
		mScrollingDownPosition = mScrollPosition;

		mPointerState = POINTER_SCROLLING;
	}
	else
	{
		ReleaseCapture();

		CRect clientRect;
		GetClientRect(&clientRect);
		if (clientRect.PtInRect(point))
		{
			mOwner->FinishSelection(mHotItem);
		}
		else
		{
			mOwner->FinishSelection(-1);
		}
	}
}

void ItemListWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect clientRect;
	GetClientRect(&clientRect);
	UpdatePointerState(clientRect, point);
	if (mPointerState == POINTER_CONTENT)
	{
		UpdateHotItem(point);
	}
}

BOOL ItemListWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void ItemListWnd::DrawContent(const CRect& clientRect)
{
	int clientHeight = clientRect.Height();
	int startIndex = mScrollPosition / FIXED_ITEM_HEIGHT;
	int endIndex = startIndex + (clientHeight / FIXED_ITEM_HEIGHT);
	if (endIndex >= mNumItems)
	{
		endIndex = mNumItems - 1;
	}

	D2D1_RECT_F itemRect;
	D2D1_RECT_F iconRect;
	D2D1_RECT_F textRect;
	itemRect.left = 0.0f;
	itemRect.right = (FLOAT)clientRect.right;
	iconRect.left = 2.0f;
	iconRect.right = iconRect.left + FIXED_ITEM_HEIGHT;
	textRect.left = iconRect.right + 1;
	textRect.right = clientRect.right - 2.0f;

	int itemIndex = 0;
	for (auto& item : mOwner->mItems)
	{
		if (startIndex <= itemIndex && itemIndex <= endIndex)
		{
			itemRect.top = (FLOAT)itemIndex * FIXED_ITEM_HEIGHT;
			itemRect.top -= mScrollPosition;
			itemRect.bottom = itemRect.top + FIXED_ITEM_HEIGHT;
			iconRect.top = textRect.top = itemRect.top;
			iconRect.bottom = textRect.bottom = itemRect.bottom;
			textRect.top -= 1.0f;
			textRect.bottom -= 1.0f;

			if (itemIndex == mHotItem)
			{
				mBrush->SetColor(VisualManager::CLR_BLUE);
				mRenderer.FillRectangle(itemRect, mBrush);
			}

			bool iconExists;
			if (item.image >= 0)
			{
				iconExists = true;
			}
			else if (item.image == -1)
			{
				iconExists = true;
				mRenderer.DrawBitmap(mOwner->mDefaultIcon, &iconRect);
			}
			else
			{
				iconExists = false;
				textRect.left = iconRect.left;
			}

			if (itemIndex == mHotItem)
			{
				mBrush->SetColor(VisualManager::CLR_WHITE);
			}
			else
			{
				mBrush->SetColor(VisualManager::CLR_LEVEL1);
			}
			mRenderer.DrawText(
				item.text.c_str(),
				static_cast<UINT32>(item.text.length()),
				DWrite::segoe12R, textRect, mBrush);
		}
		++itemIndex;
	}
}

CRect ItemListWnd::CalcScrollBarRect(const CRect& clientRect) const
{
	CRect rect(clientRect);
	rect.left = clientRect.right - COMMON_GADGET_SIZE;
	return rect;
}

CRect ItemListWnd::CalcScrollUpRect(const CRect& clientRect) const
{
	CRect rect(clientRect);

	rect.left = rect.right - COMMON_GADGET_SIZE;
	rect.bottom = rect.top + COMMON_GADGET_SIZE;

	return rect;
}

CRect ItemListWnd::CalcScrollDownRect(const CRect& clientRect) const
{
	CRect rect(clientRect);

	rect.left = rect.right - COMMON_GADGET_SIZE;
	rect.top = rect.bottom - COMMON_GADGET_SIZE;

	return rect;
}

CRect ItemListWnd::CalcScrollThumbRect(const CRect& clientRect) const
{
	CRect rect(clientRect);

	rect.left = rect.right - COMMON_GADGET_SIZE + 3;
	rect.right = rect.right - 3;

	double viewScrollArea =
		clientRect.Height() - (2 * COMMON_GADGET_SIZE) - FIXED_SCROLL_THUMB_HEIGHT;
	double logicalToView = viewScrollArea / mScrollRange;
	int viewScrollPos = (int)(logicalToView * mScrollPosition);

	rect.top = COMMON_GADGET_SIZE + viewScrollPos;
	rect.bottom = rect.top + FIXED_SCROLL_THUMB_HEIGHT;

	return rect;
}

void ItemListWnd::DrawScrollBar(const CRect& clientRect)
{
	if (false == mScrolled)
	{
		return;
	}

	D2D1_RECT_F itemRect;

	itemRect.right = (FLOAT)clientRect.right;
	itemRect.left = itemRect.right - COMMON_GADGET_SIZE + 1;
	itemRect.top = 1.0f;
	itemRect.bottom = (FLOAT)clientRect.bottom;

	mBrush->SetColor(VisualManager::CLR_BASE);
	mRenderer.FillRectangle(itemRect, mBrush);

	CRect upRect(CalcScrollUpRect(clientRect));
	itemRect.left = (FLOAT)upRect.left;
	itemRect.right = (FLOAT)upRect.right;
	itemRect.top = (FLOAT)upRect.top;
	itemRect.bottom = (FLOAT)upRect.bottom;
	if (mPointerState == POINTER_SCROLL_UP)
	{
		mRenderer.DrawBitmap(mArrowUH, &itemRect);
	}
	else
	{
		mRenderer.DrawBitmap(mArrowUN, &itemRect);
	}

	CRect downRect(CalcScrollDownRect(clientRect));
	itemRect.left = (FLOAT)downRect.left;
	itemRect.right = (FLOAT)downRect.right;
	itemRect.top = (FLOAT)downRect.top;
	itemRect.bottom = (FLOAT)downRect.bottom;
	if (mPointerState == POINTER_SCROLL_DOWN)
	{
		mRenderer.DrawBitmap(mArrowDH, &itemRect);
	}
	else
	{
		mRenderer.DrawBitmap(mArrowDN, &itemRect);
	}

	CRect thumbRect(CalcScrollThumbRect(clientRect));
	itemRect.left = static_cast<FLOAT>(thumbRect.left);
	itemRect.right = static_cast<FLOAT>(thumbRect.right);
	itemRect.top = static_cast<FLOAT>(thumbRect.top);
	itemRect.bottom = static_cast<FLOAT>(thumbRect.bottom);
	if (mPointerState == POINTER_SCROLL_THUMB || mPointerState == POINTER_SCROLLING)
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL2);
		mRenderer.FillRectangle(itemRect, mBrush);
	}
	else
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL3);
		mRenderer.FillRectangle(itemRect, mBrush);
	}

	mBrush->SetColor(VisualManager::CLR_LEVEL3);

	D2D1_POINT_2F point0;
	point0.x = clientRect.right - COMMON_GADGET_SIZE + 1.0f;
	point0.y = 0;

	D2D1_POINT_2F point1;
	point1.x = point0.x;
	point1.y = (FLOAT)clientRect.bottom;

	mRenderer.DrawLine(point0, point1, mBrush);
}

void ItemListWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);

	mRenderer.BeginDraw();

	mRenderer.Clear(VisualManager::CLR_SEMI_WHITE);

	DrawContent(clientRect);

	DrawScrollBar(clientRect);

	mBrush->SetColor(VisualManager::CLR_LEVEL3);

	D2D1_POINT_2F pt0;
	D2D1_POINT_2F pt1;
	pt0.x = 0.0f;
	pt0.y = 1.0f;
	pt1.x = (FLOAT)clientRect.Width();
	pt1.y = pt0.y;
	mRenderer.DrawLine(pt0, pt1, mBrush);

	pt0.x = 1.0f;
	pt0.y = 0.0f;
	pt1.x = pt0.x;
	pt1.y = (FLOAT)clientRect.Height();
	mRenderer.DrawLine(pt0, pt1, mBrush);

	pt0.x = 0.0f;
	pt0.y = (FLOAT)clientRect.Height();
	pt1.x = (FLOAT)clientRect.Width();
	pt1.y = pt0.y;
	mRenderer.DrawLine(pt0, pt1, mBrush);

	pt0.x = (FLOAT)clientRect.Width();
	pt0.y = 0.0f;
	pt1.x = pt0.x;
	pt1.y = (FLOAT)clientRect.Height();
	mRenderer.DrawLine(pt0, pt1, mBrush);

	mRenderer.EndDraw();

	mRenderer.Display();
}

const int ComboCtrl::FIXED_HEIGHT = 22;
const int ComboCtrl::NO_ITEM_ICON = -2;
const int ComboCtrl::DEFAULT_ICON = -1;

BEGIN_MESSAGE_MAP(ComboCtrl, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

ComboCtrl::ComboCtrl()
{
	mListener = nullptr;

	mCurrentItem = -1;

	mActive = false;
	mList = nullptr;

	mBrush = nullptr;
	mThumbN = nullptr;
	mThumbH = nullptr;
	mDefaultIcon = nullptr;
}

ComboCtrl::~ComboCtrl()
{
}

void ComboCtrl::SetListener(IListener* listener)
{
	mListener = listener;
}

void ComboCtrl::ResetListner()
{
	mListener = nullptr;
}

void ComboCtrl::CreateWnd(CWnd* parent, int x, int y, int w)
{
	assert(GetSafeHwnd() == nullptr);

	CRect rect(x, y, x + w, y + FIXED_HEIGHT);
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	Create(nullptr, nullptr, style, rect, parent, 0);
}

void ComboCtrl::MoveWnd(int x, int y)
{
	assert(GetSafeHwnd());

	UINT swpFlag = SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE;
	SetWindowPos(nullptr, x, y, 0, 0, swpFlag);
}

void ComboCtrl::ResizeWnd(int w)
{
	assert(GetSafeHwnd());

	UINT swpFlag = SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE;
	SetWindowPos(nullptr, 0, 0, w, FIXED_HEIGHT, swpFlag);
}

void ComboCtrl::MoveAndResizeWnd(int x, int y, int w)
{
	assert(GetSafeHwnd());

	UINT swpFlag = SWP_NOZORDER | SWP_NOACTIVATE;
	SetWindowPos(nullptr, x, y, w, FIXED_HEIGHT, swpFlag);
}

int ComboCtrl::AddItem(const ComboCtrl::Item& item)
{
	assert(GetSafeHwnd());

	if (mList)
	{
		mList->DestroyWindow();
		SafeDelete(mList);
	}

	int itemIndex = (int)mItems.size();
	mItems.push_back(item);
	return itemIndex;
}

void ComboCtrl::RemoveItem(int itemHandle)
{
	assert(GetSafeHwnd());

	int itemIndex = 0;
	for (list<Item>::const_iterator iter = mItems.begin(); iter != mItems.end(); ++iter)
	{
		if (itemIndex == itemHandle)
		{
			mItems.erase(iter);

			if (mList)
			{
				mList->DestroyWindow();
				SafeDelete(mList);
			}

			if (itemHandle == mCurrentItem)
			{
				mCurrentItem = -1;
			}

			return; /**********/
		}

		++itemIndex;
	}
}

void ComboCtrl::RemoveAllItems()
{
	if (mList)
	{
		mList->DestroyWindow();
		SafeDelete(mList);
	}

	mItems.clear();
	mCurrentItem = -1;
}

void ComboCtrl::SetCurrentItem(int itemHandle)
{
	assert(GetSafeHwnd());

	if (mCurrentItem != itemHandle)
	{
		mCurrentItem = itemHandle;
	}

	if (mCurrentItem < -1)
	{
		mCurrentItem = -1;
	}
}

int ComboCtrl::GetCurrentItem() const
{
	return mCurrentItem;
}

void* ComboCtrl::CurrentItemData() const
{
	assert(mCurrentItem != -1);

	int currentItem = 0;
	for (list<Item>::const_iterator iter = mItems.begin(); iter != mItems.end(); ++iter)
	{
		if (currentItem == mCurrentItem)
		{
			return iter->data;
		}

		currentItem++;
	}

	return nullptr;
}

void ComboCtrl::FinishSelection(int itemHandle)
{
	int oldItem = mCurrentItem;
	if (itemHandle > -1)
	{
		mCurrentItem = itemHandle;
	}

	Invalidate(FALSE);

	mList->DestroyWindow();
	SafeDelete(mList);

	if (oldItem != mCurrentItem)
	{
		if (mListener) mListener->OnSelectionChanged(this);
	}
}

int ComboCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mClientWidth = lpCreateStruct->cx;
	mClientHeight = lpCreateStruct->cy;

	mRenderer.Create(m_hWnd, lpCreateStruct->cx, lpCreateStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mBrush);

	WicBitmap wicBitmap(hsui::Module::handle, HSUI_IDR_COMBO_THUMB_N);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mThumbN);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_COMBO_THUMB_H);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mThumbH);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_COMBO_ITEM_ICON);
	mRenderer.CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mDefaultIcon);

	return 0;
}

void ComboCtrl::OnDestroy()
{
	if (mList)
	{
		mList->DestroyWindow();
		SafeDelete(mList);
	}

	SafeRelease(mDefaultIcon);
	SafeRelease(mThumbN);
	SafeRelease(mThumbH);
	SafeRelease(mBrush);
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void ComboCtrl::OnSize(UINT nType, int cx, int cy)
{
	mClientWidth = cx;
	mClientHeight = cy;

	mRenderer.Resize(cx, cy);
}

void ComboCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (mActive == false)
	{
		mActive = true;
		Invalidate(FALSE);

		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.dwFlags = TME_LEAVE;
		trackMouseEvent.hwndTrack = m_hWnd;
		TrackMouseEvent(&trackMouseEvent);
	}
}

void ComboCtrl::OnMouseLeave()
{
	mActive = false;
	Invalidate(FALSE);
}

void ComboCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (mList == nullptr)
	{
		mList = new ItemListWnd;
		mList->CreateWnd(this);
		GetTopLevelParent()->SetActiveWindow();
	}
}

void ComboCtrl::OnEnable(BOOL bEnable)
{
	if (mList)
	{
		mList->DestroyWindow();
		SafeDelete(mList);
	}

	Invalidate(FALSE);
}

BOOL ComboCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void ComboCtrl::OnPaint()
{
	CPaintDC dc(this);

	if (GetStyle() & WS_DISABLED)
	{
		RenderDisabled(dc.m_hDC);
	}
	else
	{
		RenderEnabled(dc.m_hDC);
	}
}

void ComboCtrl::RenderEnabled(HDC dc)
{
	ID2D1Bitmap* thumbImage;
	D2D_COLOR_F borderColor;
	if (mActive || mList)
	{
		thumbImage = mThumbH;
		borderColor = VisualManager::CLR_DARK_BLUE;
	}
	else
	{
		thumbImage = mThumbN;
		borderColor = VisualManager::CLR_LEVEL3;
	}

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_SEMI_WHITE);

	mBrush->SetColor(VisualManager::CLR_LEVEL1);
	if (mCurrentItem != -1)
	{
		D2D1_RECT_F itemRect;

		itemRect.left = 2.0f;
		itemRect.right = itemRect.left + FIXED_ITEM_HEIGHT;
		itemRect.top = 1.0f;
		itemRect.bottom = itemRect.top + FIXED_ITEM_HEIGHT;
		int itemIndex = 0;
		for (auto& item : mItems)
		{
			if (mCurrentItem == itemIndex)
			{
				bool iconExists;
				if (item.image >= 0)
				{
					iconExists = true;
				}
				else if (item.image == DEFAULT_ICON)
				{
					iconExists = true;
					mRenderer.DrawBitmap(mDefaultIcon, &itemRect);
				}
				else
				{
					iconExists = false;
				}

				itemRect.top -= 1;
				itemRect.bottom -= 1;
				if (iconExists)
				{
					itemRect.left = itemRect.right + 1;
				}
				itemRect.right = mClientWidth - COMMON_GADGET_SIZE - 2.0f;

				mRenderer.DrawText(item.text.c_str(), (UINT32)item.text.length(),
					DWrite::segoe12R, itemRect, mBrush, D2D1_DRAW_TEXT_OPTIONS_CLIP);
				break;
			}
			++itemIndex;
		}
	}

	D2D1_RECT_F thumbRect;
	thumbRect.left = (FLOAT)mClientWidth - COMMON_GADGET_SIZE - 1;
	thumbRect.right = thumbRect.left + COMMON_GADGET_SIZE;
	thumbRect.top = 1.0f;
	thumbRect.bottom = thumbRect.top + COMMON_GADGET_SIZE;
	mRenderer.DrawBitmap(thumbImage, &thumbRect);

	mBrush->SetColor(borderColor);
	D2D1_RECT_F borderRect;
	borderRect.left = 1;
	borderRect.top = 1;
	borderRect.right = (FLOAT)mClientWidth;
	borderRect.bottom = (FLOAT)mClientHeight;
	mRenderer.DrawRectangle(borderRect, mBrush);

	D2D1_POINT_2F pt0;
	D2D1_POINT_2F pt1;
	pt0.x = (FLOAT)mClientWidth - COMMON_GADGET_SIZE + 1;
	pt0.y = 0;
	pt1.x = pt0.x;
	pt1.y = (FLOAT)mClientHeight;
	mRenderer.DrawLine(pt0, pt1, mBrush);

	mRenderer.EndDraw();

	mRenderer.Display();
}

void ComboCtrl::RenderDisabled(HDC dc)
{
	RenderEnabled(dc);
}

ComboCtrl::Item::Item()
{
	image = DEFAULT_ICON;
	data = nullptr;
}

ComboCtrl::Item::Item(const ComboCtrl::Item& item)
{
	image = item.image;
	text = item.text;
	data = item.data;
}

ComboCtrl::Item& ComboCtrl::Item::operator=(const ComboCtrl::Item& item)
{
	image = item.image;
	text = item.text;
	data = item.data;
	return *this;
}

} // namespace hsui
