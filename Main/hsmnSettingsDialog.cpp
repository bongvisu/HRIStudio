#include "hsmnPrecompiled.h"
#include "hsmnSettingsDialog.h"
#include <hsmoModelManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

SettingsDialog::SettingsDialog()
{
	mW = 640;
	mH = 145;
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::InitWindow()
{
	static const int INTER_CONTROL_GAP = 4;
	static const int LABEL_WIDTH = 100;
	static const int FIELD_WIDTH = 207;

	SetWindowTextW(L"Settings");

	int y = FIXED_CAPTION_HEIGHT + INTER_CONTROL_GAP;
	int x = FIXED_BORDER_SIZE + INTER_CONTROL_GAP;
	CRect rect;
	CString text;

	mIPLabel.CreateWnd(this, L"Host", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mIPLabel.SetDrawBorder(true);
	mIPLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.right + INTER_CONTROL_GAP;
	mIPEdit.CreateWnd(this, x, y, FIELD_WIDTH);
	mIPEdit.GetEditWnd().ModifyStyle(ES_LEFT, ES_CENTER);
	mIPEdit.SetEditText(theModelManager->GetPlatformIP().c_str());

	mIPEdit.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.right + INTER_CONTROL_GAP;
	mPortLabel.CreateWnd(this, L"Port", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mPortLabel.SetDrawBorder(true);
	mPortLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.right + INTER_CONTROL_GAP;
	mPortEdit.CreateWnd(this, x, y, FIELD_WIDTH);
	mPortEdit.GetEditWnd().ModifyStyle(ES_LEFT, ES_RIGHT | ES_NUMBER);
	text.Format(L"%d", theModelManager->GetPlatformPort());
	mPortEdit.SetEditText(text);

	y += EditCtrl::FIXED_HEIGHT + INTER_CONTROL_GAP;
	mIPLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.left;
	mIDLabel.CreateWnd(this, L"ID", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mIDLabel.SetDrawBorder(true);
	mIPEdit.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.left;
	mIDEdit.CreateWnd(this, x, y, FIELD_WIDTH);
	mIDEdit.GetEditWnd().ModifyStyle(ES_LEFT, ES_CENTER);
	mIDEdit.SetEditText(theModelManager->GetPlatformID().c_str());

	mPortLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.left;
	mPWLabel.CreateWnd(this, L"Password", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mPWLabel.SetDrawBorder(true);
	mPortEdit.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.left;
	mPWEdit.CreateWnd(this, x, y, FIELD_WIDTH);
	mPWEdit.GetEditWnd().ModifyStyle(ES_LEFT, ES_CENTER);
	mPWEdit.GetEditWnd().SetPasswordChar(L'¡Ü');
	mPWEdit.SetEditText(theModelManager->GetPlatformPassword().c_str());

	mIDLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.left;
	y += EditCtrl::FIXED_HEIGHT + INTER_CONTROL_GAP;
	mDirLabel.CreateWnd(this, L"Directory", x, y, LABEL_WIDTH, EditCtrl::FIXED_HEIGHT);
	mDirLabel.SetDrawBorder(true);
	mIDEdit.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.left;
	int w = 2 * FIELD_WIDTH + 2 * INTER_CONTROL_GAP + LABEL_WIDTH;
	mDirEdit.CreateWnd(this, x, y, w);
	mDirEdit.SetEditText(theModelManager->GetPlatformDirectory().c_str());

	mDirLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.left;
	y += EditCtrl::FIXED_HEIGHT + 2 * INTER_CONTROL_GAP;
	w = LABEL_WIDTH + INTER_CONTROL_GAP + FIELD_WIDTH;
	mOK.CreateWnd(this, L"OK", x, y, w, EditCtrl::FIXED_HEIGHT);
	mOK.SetMode(ButtonCtrl::MODE_TEXT);
	mOK.SetListener(static_cast<ButtonCtrl::IListener*>(this));

	mPWLabel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	x = rect.left;
	mCancel.CreateWnd(this, L"Cancel", x, y, w, EditCtrl::FIXED_HEIGHT);
	mCancel.SetMode(ButtonCtrl::MODE_TEXT);
	mCancel.SetListener(static_cast<ButtonCtrl::IListener*>(this));
}

void SettingsDialog::OnOK()
{
	Dialog::OnOK();
}

void SettingsDialog::OnClicked(ButtonCtrl* buttonCtrl)
{
	if (&mOK == buttonCtrl)
	{
		CString text;

		text = mIPEdit.GetEditText();
		text.Trim();
		theModelManager->SetPlatformIP((const wchar_t*)text);

		text = mPortEdit.GetEditText();
		text.Trim();
		theModelManager->SetPlatformPort(_wtoi(text));

		text = mIDEdit.GetEditText();
		text.Trim();
		theModelManager->SetPlatformID((const wchar_t*)text);

		text = mPWEdit.GetEditText();
		text.Trim();
		theModelManager->SetPlatformPassword((const wchar_t*)text);

		text = mDirEdit.GetEditText();
		text.Trim();
		theModelManager->SetPlatformDirectory((const wchar_t*)text);

		OnOK();
	}
	else
	{
		Dialog::OnCancel();
	}
}

} // namespace hsmn
