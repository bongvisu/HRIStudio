#include "hsmnPrecompiled.h"
#include "hsmnNewModelDialog.h"
#include <hsmoModelManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

static const int WND_WIDTH = 496;
static const int WND_HEIGHT = 106;
static const int INTER_CONTROL_GAP = 4;

NewModelDialog::NewModelDialog()
{
	mW = WND_WIDTH;
	mH = WND_HEIGHT;
}

NewModelDialog::~NewModelDialog()
{
}

void NewModelDialog::InitWindow()
{
	SetWindowTextW(L"New Model");

	int y = FIXED_CAPTION_HEIGHT + 10;
	int x = FIXED_BORDER_SIZE + 3;
	int width;
	
	width = 60;
	mLocationLabel.CreateWnd(this, L"Location", x, y, width, EditCtrl::FIXED_HEIGHT);
	mLocationLabel.SetDrawBorder(true);
	x += width + INTER_CONTROL_GAP;

	width = 355;
	mLocationEdit.CreateWnd(this, x, y, width);
	x += width + INTER_CONTROL_GAP;

	width = 60;
	mLocationButton.CreateWnd(this, L"Find...", x, y, width, EditCtrl::FIXED_HEIGHT);
	mLocationButton.SetMode(ButtonCtrl::MODE_TEXT);
	mLocationButton.SetListener(static_cast<ButtonCtrl::IListener*>(this));

	y += EditCtrl::FIXED_HEIGHT + 10;

	CRect rect;

	mLocationLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	mNameLabel.CreateWnd(this, L"Name", rect.left, y, rect.right - rect.left, rect.bottom - rect.top);
	mNameLabel.SetDrawBorder(true);

	mLocationEdit.GetWindowRect(&rect);
	ScreenToClient(&rect);
	mNameEdit.CreateWnd(this, rect.left, y, rect.right - rect.left);

	mLocationButton.GetWindowRect(&rect);
	ScreenToClient(&rect);
	mOKButton.CreateWnd(this, L"OK", rect.left, y, rect.right - rect.left, EditCtrl::FIXED_HEIGHT);
	mOKButton.SetMode(ButtonCtrl::MODE_TEXT);
	mOKButton.SetListener(static_cast<ButtonCtrl::IListener*>(this));
}

void NewModelDialog::OnOK()
{
	CString name(mNameEdit.GetEditText());
	CString location(mLocationEdit.GetEditText());
	
	name.Trim();
	location.Trim();
	if (location.GetLength() && name.GetLength())
	{
		if (location.Right(1) != L"\\")
		{
			location += L"\\";
		}
		CString filepath = location + name + Model::FILE_EXTENSION;
		theModelManager->NewModel((const wchar_t*)filepath);
	}

	Dialog::OnOK();
}

void NewModelDialog::OnClicked(ButtonCtrl* buttonCtrl)
{
	if (&mLocationButton == buttonCtrl)
	{
		IFileDialog2* fileDialog = nullptr;
		CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
			__uuidof(IFileDialog2), reinterpret_cast<void**>(&fileDialog));

		FILEOPENDIALOGOPTIONS fileOpenDialogOptions;
		fileDialog->GetOptions(&fileOpenDialogOptions);
		fileOpenDialogOptions |= FOS_PICKFOLDERS;
		fileDialog->SetOptions(fileOpenDialogOptions);
		if (SUCCEEDED(fileDialog->Show(nullptr)))
		{
			IShellItem* shellItem = nullptr;
			fileDialog->GetResult(&shellItem);
			
			wchar_t* folderPath = nullptr;
			shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &folderPath);

			mLocationEdit.SetEditText(folderPath);
			
			CoTaskMemFree(folderPath);
			shellItem->Release();
		}
		fileDialog->Release();
	}
	else if (&mOKButton == buttonCtrl)
	{
		OnOK();
	}
}

} // namespace hsmn
