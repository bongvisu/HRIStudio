#include "hsuiPrecompiled.h"
#include "hsuiParamWindow.h"
#include "hsuiParamManager.h"
#include "hsuiVisualManager.h"
#include "hsuiResource.h"
#include <xscDWrite.h>

using namespace std;
using namespace xsc;
using namespace hsmo;

namespace hsui {

// ====================================================================================================================
static const wchar_t* WND_CLASS_NAME = L"hsui.ParamWindow";

IDWriteTextFormat* ParamWindow::msTextFormat;
ID2D1Bitmap1* ParamWindow::msBlockIcon;
ID2D1Bitmap1* ParamWindow::msLinkIcon;
ID2D1Bitmap1* ParamWindow::msModelIcon;

void ParamWindow::InitParamWindow()
{
	WNDCLASSW wndClass;
	memset(&wndClass, 0, sizeof(wndClass));
	wndClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = ::DefWindowProcW;
	wndClass.hInstance = nullptr;
	wndClass.hIcon = nullptr;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = WND_CLASS_NAME;
	AfxRegisterClass(&wndClass);

	DWrite::factory->CreateTextFormat(
		L"Segoe UI", nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		12.0f, L"", &msTextFormat);
	msTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	msTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	msTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	DWRITE_TRIMMING trimming;
	trimming.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;
	trimming.delimiter = 0;
	trimming.delimiterCount = 0;
	msTextFormat->SetTrimming(&trimming, nullptr);

	WicBitmap wicBitmap;

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_PARAM_WINDOW_ICON_BLOCK);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &msBlockIcon);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_PARAM_WINDOW_ICON_LINK);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &msLinkIcon);

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_PARAM_WINDOW_ICON_MODEL);
	D2DRenderer::resourcingContext->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &msModelIcon);
}

void ParamWindow::TermParamWindow()
{
	msModelIcon->Release();
	msLinkIcon->Release();
	msBlockIcon->Release();

	msTextFormat->Release();
}
// ====================================================================================================================

unordered_map<const wchar_t*, ParamWindow::FactoryFunction> ParamWindow::msFactory;

ParamWindow* ParamWindow::CreateDynamicInstance(Component* component)
{
	auto iter = msFactory.find(component->GetRtti().GetName());
	if (iter != msFactory.end())
	{
		return (iter->second)(component);
	}
	return nullptr;
}

wchar_t ParamWindow::msTextBuf[1024];

static const int BORDER_SIZE = 3;
static const int CAPTION_SIZE = 20;
static const int BUTTON_WIDTH = 50;
static const int BUTTON_HEIGHT = 18;

const int ParamWindow::TOP_MARGIN = 30;
const int ParamWindow::CONTENT_MARGIN = 7;
const int ParamWindow::INTERCONTROL_GAP = 3;

BEGIN_MESSAGE_MAP(ParamWindow, CWnd)
	ON_WM_NCACTIVATE()
	ON_WM_NCHITTEST()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

ParamWindow::ParamWindow(Component* component)
	:
	mComponent(component),
	mActive(FALSE),
	mIcon(msBlockIcon)
{
}

ParamWindow::~ParamWindow()
{
}

const Component* ParamWindow::GetComponent() const
{
	return mComponent;
}

void ParamWindow::CreateWnd(CWnd* owner)
{
	int width, height;
	GetDimension(width, height);

	CRect rect(0, 0, width, height);
	CreateEx(0, WND_CLASS_NAME, nullptr, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, owner, 0);

	CenterWindow();
	ShowWindow(SW_SHOW);
}

void ParamWindow::GetDimension(int& w, int& h) const
{
	w = 320;
	h = 240;
}

void ParamWindow::OnInitWnd()
{
}

void ParamWindow::OnTermWnd()
{
}

void ParamWindow::OnResizeWnd(int w, int h)
{
}

void ParamWindow::OnPaintWnd()
{
}

void ParamWindow::OnClicked(ButtonCtrl* buttonCtrl)
{
	if (buttonCtrl == &mApplyButton)
	{
		OnApply();
	}
	else if (buttonCtrl == &mCloseButton)
	{
		PostMessage(WM_CLOSE);
	}
	else
	{
		OnButtonClick(buttonCtrl);
	}
}

void ParamWindow::OnApply()
{
}

void ParamWindow::OnButtonClick(ButtonCtrl* buttonCtrl)
{
}

bool ParamWindow::IsVerticallyResizable() const
{
	return false;
}

bool ParamWindow::IsHorizontallyResizable() const
{
	return false;
}

void ParamWindow::BuilidTitleString() const
{
	swprintf_s(msTextBuf, L"");
}

BOOL ParamWindow::OnNcActivate(BOOL active)
{
	mActive = active;
	Invalidate(FALSE);
	return CWnd::OnNcActivate(active);
}

LRESULT ParamWindow::OnNcHitTest(CPoint point)
{
	CRect wndRect;
	GetWindowRect(&wndRect);
	CRect rect;

	if (IsHorizontallyResizable() && IsVerticallyResizable())
	{
		rect = wndRect;
		rect.right = rect.left + BORDER_SIZE;
		rect.bottom = rect.top + BORDER_SIZE;
		if (rect.PtInRect(point)) return HTTOPLEFT;
	}

	if (IsHorizontallyResizable() && IsVerticallyResizable())
	{
		rect = wndRect;
		rect.left = rect.right - BORDER_SIZE;
		rect.top = rect.bottom - BORDER_SIZE;
		if (rect.PtInRect(point)) return HTBOTTOMRIGHT;
	}

	if (IsHorizontallyResizable() && IsVerticallyResizable())
	{
		rect = wndRect;
		rect.right = rect.left + BORDER_SIZE;
		rect.top = rect.bottom - BORDER_SIZE;
		if (rect.PtInRect(point)) return HTBOTTOMLEFT;
	}

	if (IsHorizontallyResizable() && IsVerticallyResizable())
	{
		rect = wndRect;
		rect.left = rect.right - BORDER_SIZE;
		rect.bottom = rect.top + BORDER_SIZE;
		if (rect.PtInRect(point)) return HTTOPRIGHT;
	}

	if (IsVerticallyResizable())
	{
		rect = wndRect;
		rect.bottom = rect.top + BORDER_SIZE;
		if (rect.PtInRect(point)) return HTTOP;
	}

	if (IsHorizontallyResizable())
	{
		rect = wndRect;
		rect.right = rect.left + BORDER_SIZE;
		if (rect.PtInRect(point)) return HTLEFT;
	}

	if (IsHorizontallyResizable())
	{
		rect = wndRect;
		rect.left = rect.right - BORDER_SIZE;
		if (rect.PtInRect(point)) return HTRIGHT;
	}

	if (IsVerticallyResizable())
	{
		rect = wndRect;
		rect.top = rect.bottom - BORDER_SIZE;
		if (rect.PtInRect(point)) return HTBOTTOM;
	}

	rect = wndRect;
	rect.right -= BORDER_SIZE;
	rect.bottom = rect.top + CAPTION_SIZE;
	if (rect.PtInRect(point)) return HTCAPTION;

	return CWnd::OnNcHitTest(point);
}

int ParamWindow::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	mContentX = BORDER_SIZE;
	mContentY = BORDER_SIZE;
	mContentWidth = createStruct->cx - 2 * BORDER_SIZE;
	mContentHeight = createStruct->cy - 2 * BORDER_SIZE - CAPTION_SIZE;

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush({ 0, 0, 0, 1 }, &mBrush);
	
	mApplyButton.CreateWnd(this, L"Apply", 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	mApplyButton.SetMode(ButtonCtrl::MODE_TEXT);
	mApplyButton.SetListener(static_cast<ButtonCtrl::IListener*>(this));

	mCloseButton.CreateWnd(this, L"Close", 0, 0, BUTTON_WIDTH, BUTTON_HEIGHT);
	mCloseButton.SetMode(ButtonCtrl::MODE_TEXT);
	mCloseButton.SetListener(static_cast<ButtonCtrl::IListener*>(this));

	mCloseButton;

	OnInitWnd();

	theParamManager->RegisterWindow(this);
	return 0;
}

void ParamWindow::OnDestroy()
{
	theParamManager->UnregisterWindow(this);

	mCloseButton.ResetListener();
	mApplyButton.ResetListener();

	OnTermWnd();

	mBrush->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void ParamWindow::PostNcDestroy()
{
	delete this;
}

void ParamWindow::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);

	mRenderer.Resize(cx, cy);

	HDWP hdwp = BeginDeferWindowPos(2);

	int x, y;

	x = cx - BORDER_SIZE - BUTTON_WIDTH;
	y = BORDER_SIZE;
	DeferWindowPos(hdwp, mCloseButton.m_hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

	x -= BUTTON_WIDTH + 2;
	DeferWindowPos(hdwp, mApplyButton.m_hWnd, nullptr, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

	EndDeferWindowPos(hdwp);

	mContentX = BORDER_SIZE;
	mContentY = BORDER_SIZE + CAPTION_SIZE;
	mContentWidth = cx - 2 * BORDER_SIZE;
	mContentHeight = cy - 2 * BORDER_SIZE - CAPTION_SIZE;

	OnResizeWnd(cx, cy);
}

BOOL ParamWindow::OnEraseBkgnd(CDC*)
{
	return TRUE;
}

void ParamWindow::OnPaint()
{
	CPaintDC dc(this);

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);

	OnPaintWnd();

	D2D1_RECT_F rect;
	rect.left = 1;
	rect.top = 1;
	rect.right = static_cast<FLOAT>(mContentX + mContentWidth + BORDER_SIZE);
	rect.bottom = static_cast<FLOAT>(mContentY + mContentHeight + BORDER_SIZE);
	if (mActive)
	{
		mBrush->SetColor(VisualManager::CLR_LEVEL3);
	}
	else
	{
		mBrush->SetColor(VisualManager::CLR_DARK_BASE);
	}
	mRenderer.DrawRectangle(rect, mBrush);

	D2D1_POINT_2F pt0;
	D2D1_POINT_2F pt1;
	pt0.x = 0;
	pt0.y = static_cast<FLOAT>(mContentY + 1);
	pt1.x = rect.right;
	pt1.y = pt0.y;
	mRenderer.DrawLine(pt0, pt1, mBrush);

	rect.left = static_cast<FLOAT>(BORDER_SIZE);
	rect.top = BORDER_SIZE - 1.0f;
	rect.right = rect.left + 20;
	rect.bottom = rect.top + 20;
	mRenderer.DrawBitmap(mIcon, rect);

	rect.top -= 1;
	rect.bottom -= 1;
	rect.left = rect.right + 2;
	rect.right = rect.left + 1000;

	BuilidTitleString();
	UINT32 titleLen = static_cast<UINT32>(wcslen(msTextBuf));

	mBrush->SetColor(VisualManager::CLR_LEVEL1);
	mRenderer.DrawText(msTextBuf, titleLen, msTextFormat, rect, mBrush);

	mRenderer.EndDraw();
	mRenderer.Display();
}

} // namespace hsui
