#include "hsmnPrecompiled.h"
#include "hsmnInfoGadget.h"
#include "hsmnApplication.h"
#include <xscDWrite.h>
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(InfoGadget, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()

	ON_WM_SETTEXT()
END_MESSAGE_MAP()

InfoGadget::InfoGadget()
{
}

InfoGadget::~InfoGadget()
{
}

int InfoGadget::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mRenderer.Create(m_hWnd, 8, 8);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL3, &mBrush);

	return 0;
}

void InfoGadget::OnDestroy()
{
	mBrush->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void InfoGadget::OnSize(UINT nType, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

int InfoGadget::OnSetText(LPCTSTR lpszText)
{
	if (lpszText == nullptr)
	{
		mText = L"";
	}
	else
	{
		mText = lpszText;
	}

	Invalidate(FALSE);

	return FALSE;
}

BOOL InfoGadget::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void InfoGadget::OnPaint()
{
	CPaintDC dc(this);

	CRect clientRect;
	GetClientRect(&clientRect);

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);
		
	D2D1_RECT_F rect;
	rect.left = clientRect.left + 1.0f;
	rect.top = clientRect.top + 1.0f;
	rect.right = (FLOAT)clientRect.right;
	rect.bottom = (FLOAT)clientRect.bottom;
	
	D2D1_RECT_F textRect = rect;
	textRect.top -= 1;
	textRect.bottom -= 2;
	textRect.left += 3;
	textRect.right -= 3;
	mBrush->SetColor(VisualManager::CLR_LEVEL1);
	mRenderer.DrawText(mText, (UINT32)mText.GetLength(), DWrite::segoe12R, textRect, mBrush);

	mBrush->SetColor(VisualManager::CLR_LEVEL3);
	mRenderer.DrawRectangle(rect, mBrush);

	mRenderer.EndDraw();
	mRenderer.Display();
}

} // namespace hsmn
