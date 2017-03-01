#pragma once

#include <hsuiHorzScrollBar.h>
#include <hsuiVertScrollBar.h>

namespace hsui {

class HSUI_API ListCtrl
	:
	public CWnd,
	public ScrollBar::IListener
{
	DECLARE_MESSAGE_MAP()

public:
	class HSUI_API IListener
	{
	public:
		virtual void OnSelectionChanged(ListCtrl* listCtrl) = 0;
		virtual void OnItemDoubleClicked(ListCtrl* listCtrl) = 0;
	};
	void SetListener(IListener* listener);
	void ResetListener();

public:
	ListCtrl();
	virtual ~ListCtrl();

	class HSUI_API Column
	{
	public:
		Column();
		Column(const std::wstring& text, int width, int image = -1);
		Column(const Column& column);
		Column& operator=(const Column& column);

		Column(Column&& column);
		Column& operator=(Column&& column);

		std::wstring text;
		int width;
		int image;
	};

	void CreateWnd(CWnd* parent, const std::vector<Column>& columns, int x, int y, int w, int h);

	void MoveWnd(int x, int y);
	void ResizeWnd(int w, int h);
	void MoveAndResizeWnd(int x, int y, int w, int h);

	void Refresh();

	void BuildImageList(HMODULE module, int numItems, const UINT* list);

	void SetColumnWidth(int column, int width);
	void SetColumnText(int column, const std::wstring& text);

	void SetShowSelected(bool show);

	void ShowHorzScrollBar(bool show);
	void ShowVertScrollBar(bool show);

	class HSUI_API Subitem
	{
	public:
		Subitem();
		Subitem(const Subitem& subitem);
		Subitem& operator=(const Subitem& subitem);

		Subitem(Subitem&& subitem);
		Subitem& operator=(Subitem&& subitem);

		std::wstring text;

		bool useBackgroundColor;
		D2D_COLOR_F backgroundColor;

		bool useForegroundColor;
		D2D_COLOR_F foregroundColor;
	};

	static const int ITEM_IMAGE_NONE;
	static const int ITEM_IMAGE_DEFAULT;

	class HSUI_API Item
	{
	public:
		Item();
		Item(const std::vector<Subitem>& subitems, int image = ITEM_IMAGE_DEFAULT, void* data = nullptr);
		Item(const Item& item);
		Item& operator=(const Item& item);

		Item(Item&& item);
		Item& operator=(Item&& item);

		std::vector<Subitem> subitems;
		int image;
		void* data;
	};
	
	int AddItem(const Item& item);
	void RemoveItem(int item);
	void RemoveAllItems();
	int GetNumItems() const;

	void SetItemImage(int item, int image);
	void SetItemText(int item, int subitem, const std::wstring& text);
	
	void* GetSelectedItemData() const;

	virtual void OnScrollPositionChanged(ScrollBar* scrollBar) override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* oldWnd);
	afx_msg void OnKillFocus(CWnd* newWnd);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* wnd, UINT hitTest, UINT message);
	afx_msg BOOL OnMouseWheel(UINT flags, short zDelta, CPoint point);

	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();
	void Render();

	void AdjustScrollBarGeometryByVisibilities();
	void UpdateScrollParameters();
	bool UpdateColumnResizingParameters(CPoint point);
	int SelectHitItem(CPoint point);

protected:
	IListener* mListener;

	HorzScrollBar mHorz;
	VertScrollBar mVert;

	int mClientWidth;
	int mClientHeight;

	std::vector<Column> mColumns;
	std::list<Item> mItems;
	
	bool mFocused;
	bool mTrackingMouseLeave;

	int mHotItem;
	int mSelectedItem;
	bool mShowActive;

	bool mShowScrollBar[ScrollBar::NUMBER_OF_TYPES];
	int  mScrollBarSize[ScrollBar::NUMBER_OF_TYPES];

	HCURSOR mResizingCursor;
	int mResizingColumnIndex;
	int mResizingPivotPosition;
	int mResizingPivotWidth;

	xsc::D2DRenderer mRenderer;

	ID2D1Bitmap1* mHeaderFill;
	ID2D1Bitmap1* mDefaultIcon;
	std::vector<ID2D1Bitmap*> mImageList;
	ID2D1SolidColorBrush* mBorderBrush;
	ID2D1SolidColorBrush* mItemBrushGeneric;
	ID2D1SolidColorBrush* mItemBrushHot;
	ID2D1SolidColorBrush* mItemBrushSelectedFocused;
	ID2D1SolidColorBrush* mItemBrushSelectedUnfocused;
	ID2D1SolidColorBrush* mTextBrushHeader;
	ID2D1SolidColorBrush* mTextBrushActive;
	ID2D1SolidColorBrush* mTextBrushNormal;
};

} // namespace hsui
