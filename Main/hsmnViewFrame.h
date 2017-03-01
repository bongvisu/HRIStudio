#pragma once

namespace hsmn {

class ViewFrame : public CMDIChildWndEx
{
	DECLARE_MESSAGE_MAP()

public:
	ViewFrame();
	virtual ~ViewFrame();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	
	virtual std::wstring GetNameText() const;
	virtual std::wstring GetPathText() const;

protected:
	afx_msg void OnMDIActivate(BOOL activate, CWnd* activateWnd, CWnd* deactivateWnd);
};

} // namespace hsmn
