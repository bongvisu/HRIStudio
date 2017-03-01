#pragma once

#include "hsmnResource.h"

namespace hsmn {

class AboutDlg : public CDialogEx
{
	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = HSMN_IDR_ABOUT_BOX };
	AboutDlg();

	virtual BOOL OnInitDialog() override;

protected:
	virtual void DoDataExchange(CDataExchange* dx) override;
};

} // namespace hsmn
