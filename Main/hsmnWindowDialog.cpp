#include "hsmnPrecompiled.h"
#include "hsmnWindowDialog.h"
#include "hsmnApplication.h"
#include "hsmnMainFrame.h"
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

static const int WND_WIDTH = 600;
static const int WND_HEIGHT = 400;

WindowDialog::WindowDialog()
{
	mW = WND_WIDTH;
	mH = WND_HEIGHT;
}

WindowDialog::~WindowDialog()
{
}

void WindowDialog::InitWindow()
{
	SetWindowTextW(L"Windows");

	vector<ListCtrl::Column> columns(2);
	ListCtrl::Column column;
	column.text = L"Name";
	column.width = 200;
	columns[0] = column;
	column.text = L"Path";
	column.width = 362;
	columns[1] = column;
	mList.CreateWnd(this, columns, 10, 38, 580, 352);
	mList.ShowHorzScrollBar(false);

	mActivateButton.CreateWnd(this, L"Activate", 490, 6, 70, 20);
	mActivateButton.SetMode(ButtonCtrl::MODE_TEXT);
	mActivateButton.SetListener(static_cast<ButtonCtrl::IListener*>(this));

	MainFrame* mainFrame = static_cast<MainFrame*>(theApp.m_pMainWnd);
	const auto& viewFrames = mainFrame->mCommandToView;
	for (auto viewInfo : viewFrames)
	{
		ListCtrl::Item item;
		item.data = reinterpret_cast<void*>(static_cast<uint64_t>(viewInfo.first));

		ListCtrl::Subitem subitem;
		subitem.text = viewInfo.second.name;
		item.subitems.push_back(subitem);
		subitem.text = viewInfo.second.path;
		item.subitems.push_back(subitem);

		mList.AddItem(item);
	}
}

void WindowDialog::OnOK()
{
	Dialog::OnOK();
}

void WindowDialog::OnClicked(ButtonCtrl* buttonCtrl)
{
	if (&mActivateButton == buttonCtrl)
	{
		MainFrame* mainFrame = static_cast<MainFrame*>(theApp.m_pMainWnd);
		UINT command = static_cast<UINT>(reinterpret_cast<uint64_t>(mList.GetSelectedItemData()));
		mainFrame->OnActivateWindow(command);

		OnOK();
	}
}

void WindowDialog::PaintContent(CDC* dc)
{
	CRect rect;
	mList.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.InflateRect(1, 1);

	auto borderColor = VisualManager::ToGdiColor(VisualManager::CLR_LEVEL3);
	dc->Draw3dRect(&rect, borderColor, borderColor);
}

} // namespace hsmn
