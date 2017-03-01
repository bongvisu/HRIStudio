#pragma once

#include <hsuiModule.h>

namespace hsui {

class HSUI_API ShadowBorder : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	ShadowBorder();
	virtual ~ShadowBorder();

	static const int FIXED_SIZE;
	enum Role { ROLE_LEFT = 0, ROLE_RIGHT, ROLE_TOP, ROLE_BOTTOM, NUMBER_OF_ROLES };
	void CreateWnd(CWnd* owner, Role role, bool initiallyVisible = true);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnWindowPosChanged(WINDOWPOS* windowPos);
	void RenderLayered();

protected:
	Role mRole;

	// ================================================================================================================
	private: friend Module;
	static void InitShadowBorder();
	static void TermShadowBorder();
	// ================================================================================================================
};

} // namespace hsui
