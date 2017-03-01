#include "hsuiPrecompiled.h"
#include "hsuiShadowBorder.h"
#include "hsuiResource.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;

namespace hsui {

// ====================================================================================================================
static const wchar_t* WND_CLASS_NAME = L"hsui.ShadowBorder";

static ID2D1Bitmap* gsShadowL;
static ID2D1Bitmap* gsShadowR;
static ID2D1Bitmap* gsShadowT;
static ID2D1Bitmap* gsShadowB;
static ID2D1Bitmap* gsShadowLT;
static ID2D1Bitmap* gsShadowLB;
static ID2D1Bitmap* gsShadowRT;
static ID2D1Bitmap* gsShadowRB;

void ShadowBorder::InitShadowBorder()
{
	WNDCLASSW wndClass;
	memset(&wndClass, 0, sizeof(wndClass));

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = ::DefWindowProcW;
	wndClass.hInstance = nullptr;
	wndClass.hIcon = nullptr;
	wndClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wndClass.hbrBackground = nullptr;
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = WND_CLASS_NAME;

	AfxRegisterClass(&wndClass);

	WicBitmap wicBitmap;
	ID2D1DCRenderTarget* dcRenderTarget = D2DRenderer::dcRenderTarget;

	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SHADOW_BORDER_L);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsShadowL);
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SHADOW_BORDER_R);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsShadowR);
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SHADOW_BORDER_T);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsShadowT);
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SHADOW_BORDER_B);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsShadowB);
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SHADOW_BORDER_LT);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsShadowLT);
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SHADOW_BORDER_LB);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsShadowLB);
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SHADOW_BORDER_RT);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsShadowRT);
	wicBitmap.LoadFromResource(hsui::Module::handle, HSUI_IDR_SHADOW_BORDER_RB);
	dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &gsShadowRB);
}

void ShadowBorder::TermShadowBorder()
{
	gsShadowL->Release();
	gsShadowR->Release();
	gsShadowT->Release();
	gsShadowB->Release();
	gsShadowLT->Release();
	gsShadowLB->Release();
	gsShadowRT->Release();
	gsShadowRB->Release();
}
// ====================================================================================================================

const int ShadowBorder::FIXED_SIZE = 7;

BEGIN_MESSAGE_MAP(ShadowBorder, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

ShadowBorder::ShadowBorder()
{
	mRole = ROLE_LEFT;
}

ShadowBorder::~ShadowBorder()
{
}

void ShadowBorder::CreateWnd(CWnd* owner, Role role, bool initiallyVisible)
{
	mRole = role;

	DWORD style = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | ((initiallyVisible) ? WS_VISIBLE : 0);
	CreateEx(WS_EX_LAYERED, WND_CLASS_NAME, nullptr, style, { 0, 0, 8, 8 }, owner, 0);
}

int ShadowBorder::OnCreate(LPCREATESTRUCT createStruct)
{
	CWnd::OnCreate(createStruct);

	RenderLayered();

	return 0;
}

void ShadowBorder::OnDestroy()
{
	CWnd::OnDestroy();
}

void ShadowBorder::OnWindowPosChanged(WINDOWPOS* windowPos)
{
	CWnd::OnWindowPosChanged(windowPos);

	RenderLayered();
}

void ShadowBorder::RenderLayered()
{
	CClientDC dcDesktop(nullptr);

	CRect rect;
	GetWindowRect(&rect);
	int wndX = rect.left;
	int wndY = rect.top;
	rect.OffsetRect(-wndX, -wndY);

	CDC dcOffscreen;
	dcOffscreen.CreateCompatibleDC(&dcDesktop);

	CBitmap offscreenSurface;
	offscreenSurface.CreateCompatibleBitmap(&dcDesktop, rect.Width(), rect.Height());

	CBitmap* oldSurface = dcOffscreen.SelectObject(&offscreenSurface);

	{
		ID2D1DCRenderTarget* dcTarget = D2DRenderer::dcRenderTarget;

		dcTarget->BindDC(dcOffscreen.m_hDC, &rect);
		dcTarget->BeginDraw();
		dcTarget->Clear({ 0.0f, 0.0f, 0.0f, 0.0f });

		D2D1_RECT_F fillRect;
		if (mRole == ROLE_LEFT)
		{
			fillRect.left = 0;
			fillRect.top = FIXED_SIZE;
			fillRect.right = FIXED_SIZE;
			fillRect.bottom = (FLOAT)rect.bottom - FIXED_SIZE;
			dcTarget->DrawBitmap(gsShadowL, fillRect);

			fillRect.left = 0;
			fillRect.top = 0;
			fillRect.right = FIXED_SIZE;
			fillRect.bottom = (FLOAT)FIXED_SIZE;
			dcTarget->DrawBitmap(gsShadowLT, fillRect);

			fillRect.left = 0;
			fillRect.top = (FLOAT)rect.bottom - FIXED_SIZE;
			fillRect.right = FIXED_SIZE;
			fillRect.bottom = (FLOAT)rect.bottom;
			dcTarget->DrawBitmap(gsShadowLB, fillRect);
		}
		else if (mRole == ROLE_RIGHT)
		{
			fillRect.left = 0;
			fillRect.top = FIXED_SIZE;
			fillRect.right = FIXED_SIZE;
			fillRect.bottom = (FLOAT)rect.bottom - FIXED_SIZE;
			dcTarget->DrawBitmap(gsShadowR, fillRect);

			fillRect.left = 0;
			fillRect.top = 0;
			fillRect.right = FIXED_SIZE;
			fillRect.bottom = (FLOAT)FIXED_SIZE;
			dcTarget->DrawBitmap(gsShadowRT, fillRect);

			fillRect.left = 0;
			fillRect.top = (FLOAT)rect.bottom - FIXED_SIZE;
			fillRect.right = FIXED_SIZE;
			fillRect.bottom = (FLOAT)rect.bottom;
			dcTarget->DrawBitmap(gsShadowRB, fillRect);
		}
		else if (mRole == ROLE_TOP)
		{
			fillRect.left = 0;
			fillRect.top = 0;
			fillRect.right = (FLOAT)rect.right;
			fillRect.bottom = (FLOAT)FIXED_SIZE;
			dcTarget->DrawBitmap(gsShadowT, fillRect);
		}
		else if (mRole == ROLE_BOTTOM)
		{
			fillRect.left = 0;
			fillRect.top = 0;
			fillRect.right = (FLOAT)rect.right;
			fillRect.bottom = (FLOAT)FIXED_SIZE;
			dcTarget->DrawBitmap(gsShadowB, fillRect);
		}

		dcTarget->EndDraw();
	}

	CPoint ptDest(wndX, wndY);
	CSize sizeDest(rect.Width(), rect.Height());
	CPoint ptSrc(0, 0);
	BLENDFUNCTION blendFunc = { 0 };
	blendFunc.BlendOp = AC_SRC_OVER;
	blendFunc.SourceConstantAlpha = 255;
	blendFunc.AlphaFormat = AC_SRC_ALPHA;
	UpdateLayeredWindow(&dcDesktop, &ptDest, &sizeDest, &dcOffscreen, &ptSrc, 0, &blendFunc, ULW_ALPHA);

	dcOffscreen.SelectObject(oldSurface);
}

} // namespace hsui
