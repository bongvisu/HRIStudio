#pragma once

#include <xscMaxHeap.h>
#include <xscD2DRenderer.h>
#include <hsuiHorzScrollBar.h>
#include <hsuiVertScrollBar.h>

namespace hsui {

class HSUI_API TreeCtrl
	:
	public CWnd,
	public ScrollBar::IListener
{
	DECLARE_MESSAGE_MAP()

public:
	class HSUI_API IListener
	{
	public:
		virtual void OnSelectionChanged(TreeCtrl* treeCtrl) = 0;
		virtual void OnItemDoubleClicked(TreeCtrl* treeCtrl) = 0;
		virtual void OnLButtonDownOnItem(TreeCtrl* treeCtrl) = 0;
	};
	void SetListener(IListener* listener);
	void ResetListener();

public:
	TreeCtrl();
	virtual ~TreeCtrl();

	void CreateWnd(CWnd* parent, int x, int y, int w, int h);

	void MoveWnd(int x, int y);
	void ResizeWnd(int w, int h);
	void MoveAndResizeWnd(int x, int y, int w, int h);

	void Refresh();

	void BuildImageList(HMODULE module, int numItems, const UINT* list);

	class HSUI_API Item
	{
	public:
		Item();
		Item(const Item& item);
		Item& operator=(const Item& item);

		Item(Item&& item);
		Item& operator=(Item&& item);

		int image;
		std::wstring text;
		bool bold;
		void* data;
	};

	int GetSelectedItem() const;
	void* GetItemData(int handle) const;

	int InsertItem(const Item& item, int parent = 0, int insertAfter = 0);
	void RemoveItem(int handle);
	void RemoveAllItems();

	void SetSelectedItem(int handle);
	void SetItemText(int handle, const std::wstring& text);
	void SetItemData(int handle, void* data);
	void SetItemImage(int handle, int image);
	void SetItemTextBoldness(int handle, bool bold);

	void Reorder(int parent, const std::vector<int>& children);

	void CollapseItem(int handle = 0);
	void ExpandItem(int handle = 0);
	bool IsLeaf(int handle) const;

	void ShowHorzScrollBar(bool show);
	void ShowVertScrollBar(bool show);

	virtual void OnScrollPositionChanged(ScrollBar* scrollBar) override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* oldWnd);
	afx_msg void OnKillFocus(CWnd* newWnd);
	afx_msg void OnMouseMove(UINT flags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnMouseWheel(UINT flags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonDown(UINT flags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT flags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

	class ItemNode
	{
	public:
		ItemNode();
		ItemNode(const Item& item, ItemNode* parent, int handle);
		ItemNode(Item&& item, ItemNode* parent, int handle);

		Item item;

		int handle;
		xsc::MaxHeap<int, int>::Record* extentRecord;

		int level;
		int pivot;
		bool Expanded;

		ItemNode* parent;
		std::list<std::unique_ptr<ItemNode>> children;
	};

	void Render();
	void RenderItemRecursively(const ItemNode* itemNode,
		int horzOffset, int& vertDisplacement,
		int xViewExtent, int yViewExtent);

	void ReconfigureScrolls();
	bool CheckExposednessRecursively(ItemNode* itemNode);
	void RemoveItemRecursively(ItemNode* node, bool calcVertDelta);
	void ApplyCollapseQuantityRecursively(ItemNode* itemNode);
	void AccumulateExpandQuantityRecursively(ItemNode* itemNode);

	void UpdateHotItem(CPoint point);
	void CheckHotStateRecursively(ItemNode* itemNode,
		int horzOffset, int& vertDisplacement,
		int xViewExtent, int yViewExtent,
		CPoint point);
	
protected:
	IListener* mListener;

	HorzScrollBar mHorz;
	VertScrollBar mVert;
	bool mShowHorz;
	bool mShowVert;

	int mHandleLedger;

	typedef std::unordered_map<int, ItemNode*> ItemToNode;
	ItemToNode mItemToNode;
	std::unique_ptr<ItemNode> mRootNode;

	int mClientWidth;
	int mClientHeight;

	int mVertExtent;
	xsc::MaxHeap<int, int> mHorzExtent;

	bool mFocused;
	bool mTrackingMouseLeave;
	int mHotItem;
	int mSelectedItem;

	enum HotZone { HOT_ZONE_GIZMO, HOT_ZONE_OTHER };
	HotZone mHotZone;

	xsc::D2DRenderer mRenderer;
	ID2D1Bitmap* mExpandedGizmoNormal;
	ID2D1Bitmap* mExpandedGizmoBright;
	ID2D1Bitmap* mCollapsedGizmoNormal;
	ID2D1Bitmap* mCollapsedGizmoBright;
	ID2D1Bitmap* mDefaultIcon;
	std::vector<ID2D1Bitmap*> mImageList;
	ID2D1SolidColorBrush* mBrushScrollCorner;
	ID2D1SolidColorBrush* mBrushHot;
	ID2D1SolidColorBrush* mBrushSelectedFocused;
	ID2D1SolidColorBrush* mBrushSelectedUnfocused;
	ID2D1SolidColorBrush* mTextBrushWhite;
	ID2D1SolidColorBrush* mTextBrushNormal;
	ID2D1SolidColorBrush* mTextBrushBlack;
};

} // namespace hsui
