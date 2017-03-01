#include "hsmnPrecompiled.h"
#include "hsmnMenuStrip.h"
#include "hsmnApplication.h"
#include <xscWicBitmap.h>
#include <xscD2DRenderer.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(MenuStrip, CMFCMenuBar)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

MenuStrip::MenuStrip()
{
}

MenuStrip::~MenuStrip()
{
}

int MenuStrip::GetRowHeight() const
{
	return 22;
}

int MenuStrip::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CMFCMenuBar::OnCreate(lpCreateStruct);

	WicBitmap wicBitmap;
	wicBitmap.LoadFromResource(theApp.m_hInstance, HSMN_IDR_LOGO_IMAGE);
	D2DRenderer::dcRenderTarget->CreateBitmapFromWicBitmap(wicBitmap.GetSource(), &mLogo);

	return 0;
}

void MenuStrip::OnDestroy()
{
	mLogo->Release();

	CMFCMenuBar::OnDestroy();
}

void MenuStrip::OnSize(UINT type, int cx, int cy)
{
	CMFCMenuBar::OnSize(type, cx, cy);
}

void MenuStrip::DoPaint(CDC* dc)
{
	CMFCMenuBar::DoPaint(dc);

	CRect rectClient;
	GetClientRect(rectClient);

	ID2D1DCRenderTarget* dcRenderTarget = D2DRenderer::dcRenderTarget;
	dcRenderTarget->BindDC(dc->m_hDC, &rectClient);
	dcRenderTarget->BeginDraw();

	D2D1_RECT_F rect;
	rect.left = rectClient.right - 176.0f;
	rect.right = rect.left + 176;
	rect.top = static_cast<FLOAT>((rectClient.Height() - 22) / 2);
	rect.bottom = rect.top + 22;
	dcRenderTarget->DrawBitmap(mLogo, rect);

	dcRenderTarget->EndDraw();
}

} // namespace hsmn
