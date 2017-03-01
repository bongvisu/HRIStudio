#pragma once

namespace hsmn {

class MenuStrip : public CMFCMenuBar
{
	DECLARE_MESSAGE_MAP()

public:
	MenuStrip();
	virtual ~MenuStrip();

	virtual int GetRowHeight() const override;

protected:
	virtual void DoPaint(CDC* dc) override;

	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);

	ID2D1Bitmap* mLogo;
};

} // namespace hsmn
