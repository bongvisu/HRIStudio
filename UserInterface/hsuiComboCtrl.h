#pragma once

#include <xscD2DRenderer.h>
#include <hsuiModule.h>

namespace hsui {

class HSUI_API ComboCtrl : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	class HSUI_API IListener
	{
	public:
		virtual void OnSelectionChanged(ComboCtrl* comboCtrl) = 0;
	};
	void SetListener(IListener* listener);
	void ResetListner();

public:
	ComboCtrl();
	virtual ~ComboCtrl() override;

	static const int FIXED_HEIGHT;
	void CreateWnd(CWnd* parent, int x, int y, int w);

	void MoveWnd(int x, int y);
	void ResizeWnd(int w);
	void MoveAndResizeWnd(int x, int y, int w);

	static const int NO_ITEM_ICON;
	static const int DEFAULT_ICON;
	class HSUI_API Item
	{
	public:
		Item();
		Item(const Item& item);
		Item& operator=(const Item& item);

		int image;
		std::wstring text;
		void* data;
	};
	int AddItem(const Item& item);
	void RemoveItem(int itemHandle);
	void RemoveAllItems();
	void SetCurrentItem(int itemHandle);
	int GetCurrentItem() const;
	void* CurrentItemData() const;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnEnable(BOOL enable);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

	void RenderEnabled(HDC dc);
	void RenderDisabled(HDC dc);

	void FinishSelection(int itemHandle);

protected:
	IListener* mListener;

	std::list<Item> mItems;
	int mCurrentItem;
	
	int mClientWidth;
	int mClientHeight;

	friend class ItemListWnd;
	ItemListWnd* mList;
	bool mActive;
	
	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
	ID2D1Bitmap1* mThumbN;
	ID2D1Bitmap1* mThumbH;
	ID2D1Bitmap1* mDefaultIcon;
};

} // namespace hsui
