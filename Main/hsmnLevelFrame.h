#pragma once

#include "hsmnViewFrame.h"
#include "hsmnLevelPanel.h"

namespace hsmn {

class LevelFrame : public ViewFrame
{
	DECLARE_MESSAGE_MAP()

public:
	LevelFrame(xsc::Level* level, CMDIFrameWnd* parent);
	virtual ~LevelFrame();

	virtual BOOL OnCmdMsg(UINT, int, void*, AFX_CMDHANDLERINFO*) override;
	
	xsc::Level* GetLevel() const;
	virtual std::wstring GetNameText() const override;
	virtual std::wstring GetPathText() const override;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnClose();

	afx_msg void OnSetFocus(CWnd* pOldWnd);

protected:
	xsc::Level* mLevel;
	std::unique_ptr<LevelPanel> mPanel;
};

} // namespace hsmn
