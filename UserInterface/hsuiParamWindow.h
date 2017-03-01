#pragma once

#include <xscComponent.h>
#include <hsuiButtonCtrl.h>

namespace hsui {

class HSUI_API ParamWindow
	:
	public CWnd,
	public ButtonCtrl::IListener
{
	DECLARE_MESSAGE_MAP()

public:
	typedef ParamWindow* (* FactoryFunction) (xsc::Component*);
	static ParamWindow* CreateDynamicInstance(xsc::Component* component);

public:
	void CreateWnd(CWnd* owner);
	const xsc::Component* GetComponent() const;

	virtual void OnClicked(ButtonCtrl* buttonCtrl) override final;

protected:
	ParamWindow(xsc::Component* component);
	virtual ~ParamWindow();

	virtual void OnInitWnd();
	virtual void OnTermWnd();

	virtual bool IsVerticallyResizable() const;
	virtual bool IsHorizontallyResizable() const;
	virtual void GetDimension(int& w, int& h) const;
	virtual void BuilidTitleString() const;

	virtual void OnApply();
	virtual void OnButtonClick(ButtonCtrl* buttonCtrl);
	virtual void OnResizeWnd(int w, int h);
	virtual void OnPaintWnd();

	afx_msg BOOL OnNcActivate(BOOL);
	afx_msg LRESULT OnNcHitTest(CPoint point);

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	virtual void PostNcDestroy() override;

	afx_msg void OnSize(UINT type, int cx, int cy);
	
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	xsc::Component* mComponent;

	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
	ID2D1Bitmap1* mIcon;

	BOOL mActive;
	int mContentX;
	int mContentY;
	int mContentWidth;
	int mContentHeight;

	ButtonCtrl mApplyButton;
	ButtonCtrl mCloseButton;

protected:
	static IDWriteTextFormat* msTextFormat;
	static ID2D1Bitmap1* msBlockIcon;
	static ID2D1Bitmap1* msLinkIcon;
	static ID2D1Bitmap1* msModelIcon;
	static wchar_t msTextBuf[1024];

	static const int TOP_MARGIN;
	static const int CONTENT_MARGIN;
	static const int INTERCONTROL_GAP;

protected:
	static std::unordered_map<const wchar_t*, FactoryFunction> msFactory;

	// ================================================================================================================
	private: friend Module;
	static void InitParamWindow();
	static void TermParamWindow();
	// ================================================================================================================
};

#define HSUI_PW_LOCAL_RECT(ctrl, rect) \
	ctrl.GetWindowRect(&rect); \
	ScreenToClient(&rect);

} // namespace hsui
