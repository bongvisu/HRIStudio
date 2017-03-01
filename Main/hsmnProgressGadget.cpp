#include "hsmnPrecompiled.h"
#include "hsmnProgressGadget.h"
#include "hsmnApplication.h"
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

static const UINT_PTR UPDATE_TIMER_ID = 1000;

BEGIN_MESSAGE_MAP(ProgressGadget, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

ProgressGadget::ProgressGadget()
{
}

ProgressGadget::~ProgressGadget()
{
}

int ProgressGadget::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWnd::OnCreate(lpCreateStruct);

	mRenderer.Create(m_hWnd, 8, 8);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL3, &mBrush);

	theModelManager->AddObserver(static_cast<IModelManagerObserver*>(this));
	return 0;
}

void ProgressGadget::OnDestroy()
{
	theModelManager->RemoveObserver(static_cast<IModelManagerObserver*>(this));

	mBrush->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void ProgressGadget::OnSize(UINT nType, int cx, int cy)
{
	mRenderer.Resize(cx, cy);
}

BOOL ProgressGadget::OnEraseBkgnd(CDC* dc)
{
	return TRUE;
}

void ProgressGadget::OnCodeTransferStarting()
{
	SetTimer(UPDATE_TIMER_ID, 10, nullptr);
}

void ProgressGadget::OnCodeTransferCompleted()
{
	KillTimer(UPDATE_TIMER_ID);
	Invalidate(FALSE);
}

void ProgressGadget::OnMonitorConnected()
{
}

void ProgressGadget::OnMonitorDisconnected()
{
}

void ProgressGadget::OnMonitorStateUpdated()
{
}

void ProgressGadget::OnTimer(UINT_PTR timerID)
{
	if (timerID == UPDATE_TIMER_ID)
	{
		Invalidate(FALSE);
	}
	else
	{
		CWnd::OnTimer(timerID);
	}
}

void ProgressGadget::OnPaint()
{
	CPaintDC dc(this);

	CRect clientRect;
	GetClientRect(&clientRect);

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);
		
	D2D1_RECT_F rect;

	auto progress = theModelManager->GetDownloadProgress();
	int w = static_cast<int>(progress * 0.01 * clientRect.Width());
	rect.left = static_cast<FLOAT>(clientRect.left);
	rect.top = static_cast<FLOAT>(clientRect.top);
	rect.right = rect.left + w;
	rect.bottom = static_cast<FLOAT>(clientRect.bottom);
	mBrush->SetColor(VisualManager::CLR_DARK_BLUE);
	mRenderer.FillRectangle(rect, mBrush);

	rect.left = clientRect.left + 1.0f;
	rect.top = clientRect.top + 1.0f;
	rect.right = static_cast<FLOAT>(clientRect.right);
	rect.bottom = static_cast<FLOAT>(clientRect.bottom);
	mBrush->SetColor(VisualManager::CLR_LEVEL3);
	mRenderer.DrawRectangle(rect, mBrush);

	mRenderer.EndDraw();
	mRenderer.Display();
}

} // namespace hsmn
