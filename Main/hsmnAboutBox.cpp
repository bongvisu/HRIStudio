#include "hsmnPrecompiled.h"
#include "hsmnAboutBox.h"
#include <hsuiVisualManager.h>

using namespace std;
using namespace xsc;
using namespace hsmo;
using namespace hsui;

namespace hsmn {

BEGIN_MESSAGE_MAP(AboutDlg, CDialogEx)
END_MESSAGE_MAP()

AboutDlg::AboutDlg() : CDialogEx(AboutDlg::IDD)
{
}

void AboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL AboutDlg::OnInitDialog()
{
	BOOL ret = CDialogEx::OnInitDialog();
	
	SetBackgroundColor(VisualManager::ToGdiColor(VisualManager::CLR_BASE));
	
	return ret;
}

} // namespace hsmn
