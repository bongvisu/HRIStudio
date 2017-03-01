#pragma once

#include <xscObject.h>
#include <hsuiVertScrollBar.h>

namespace hsmn {

class LibraryView
	:
	public CWnd,
	public hsui::ScrollBar::IListener
{
	DECLARE_MESSAGE_MAP()

public:
	LibraryView();
	virtual ~LibraryView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();

	afx_msg void OnSize(UINT type, int cx, int cy);
	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd* capturingWnd);

	afx_msg void OnPaint();
	void Render();

	void PopulateItems();
	void UpdateGeometricParameters(bool reconfigure);
	virtual void OnScrollPositionChanged(hsui::ScrollBar* scrollBar) override;

	enum ItemType { ITEM_TYPE_ELEMENT, ITEM_TYPE_GROUP };

	class Item : public xsc::Object
	{
	public:
		Item();
		
		virtual ItemType GetItemType() const = 0;

	protected:
		bool mHot;
		bool mSelected;

		friend class LibraryView;
	};
	typedef xsc::Pointer<Item> ItemPtr;

	class Element : public Item
	{
	public:
		Element(const wchar_t* rttiName);
		virtual ItemType GetItemType() const override;

		Item* FindHit(int offset, CPoint point, LibraryView* view);
		void Draw(int offset, LibraryView* view);

	protected:
		const wchar_t* mRttiName;
		ID2D1Bitmap1* mBitmap;
		CRect mRect;

		friend class Group;
		friend class LibraryView;
	};
	typedef xsc::Pointer<Element> ElementPtr;

	class Group : public Item
	{
	public:
		Group(const wchar_t* displayName);

		virtual ItemType GetItemType() const override;

		int GetExtent() const; 
		void Reconfigure(int offset, LibraryView* view);
		void UpdateOffset(int offset);

		void ToggleExpasion();
		bool ConsumeLButtonDown(CPoint point, LibraryView* view);
		Item* FindHit(CPoint point, LibraryView* view);
		void Draw(LibraryView* view);

	protected:
		const wchar_t* mDisplayName;

		bool mExpanded;

		int mNumRows;
		int mOffset;
		int mContentExtent;
		std::vector<ElementPtr> mElements;

		friend class LibraryView;
	};
	typedef xsc::Pointer<Group> GroupPtr;

	void UpdateHotItem(CPoint point);

protected:
	IDWriteTextFormat* mTextFormat;

	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
	ID2D1Bitmap* mIconExpaned;
	ID2D1Bitmap* mIconCollapsed;

	hsui::VertScrollBar mScrollBar;
	int mWidth, mHeight;
	int mNumColumns;
	double mIntercolumnGap;

	std::vector<GroupPtr> mGroups;
	Item* mHotItem;
	Item* mSelectedItem;
	bool mTrackingMouseLeave;
	
	CImageList mDragIcon;
	bool mDragEntered;
	
private:
	friend class Element;
	friend class Group;
};

} // namespace hsmn
