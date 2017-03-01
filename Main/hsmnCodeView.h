#pragma once

#include <hsmoModel.h>
#include <hsuiSystemButtons.h>
#include <hsuiCodeEditor.h>

namespace hsmn {

class CodeView : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	CodeView();
	virtual ~CodeView();

	void CreateWnd(CWnd* owner);

	void Update(hsmo::Model* model);
	void UpdateAndDownload(hsmo::Model* model);

protected:
	afx_msg BOOL OnNcActivate(BOOL);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	
	afx_msg int OnCreate(LPCREATESTRUCT createStruct);
	afx_msg void OnDestroy();
	virtual void PostNcDestroy() override;
	afx_msg void OnClose();
	afx_msg void OnSize(UINT type, int cx, int cy);
	
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnPaint();

protected:
	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
	ID2D1Bitmap1* mIcon;

	hsui::SystemButtonClose mSysCloseButton;
	hsui::CodeEditor mEdit;

	BOOL mActive;
};

} // namespace hsmn
