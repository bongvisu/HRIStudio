#pragma once

#include <xscD2DRenderer.h>
#include <hsuiVertScrollBar.h>
#include <hsuiHorzScrollBar.h>

namespace hsui {

class CodeEditor;

enum LexicalContext
{
	CE_LEXICAL_CONTEXT_NONE,
	CE_LEXICAL_CONTEXT_COMMENT,
	CE_LEXICAL_CONTEXT_DEFINE,
	CE_LEXICAL_CONTEXT_TRIGGER,
	CE_LEXICAL_CONTEXT_CHAINED_TRIGGER,
	CE_LEXICAL_CONTEXT_RESPONSE,
	CE_LEXICAL_CONTEXT_CONDITIONAL_RESPONSE
};

class Token
{
public:
	enum Type
	{
		TYPE_COMMENT,
		TYPE_SPECIAL,
		TYPE_GENERIC
	};

	Token();
	Token(Type type, int index, int count);

	Type type;
	int index;
	int count;
};

class CodeLine
{
public:
	CodeLine();

	CodeLine(const std::wstring& str);
	CodeLine(std::wstring&& str);

	CodeLine(const CodeLine& line);
	CodeLine& operator=(const CodeLine& line);

	CodeLine(CodeLine&& line);
	CodeLine& operator=(CodeLine&& line);

	int GetExtent() const;
	int GetExtentTo(int i) const;
	int GetColumnFromPoint(int x) const;
	int GetColumnFromPoint(int x, int& extent) const;

	int GetColumnFromUnit(int units) const;
	int GetUnitFromColumn(int column) const;

	int GetNumUnits() const;
	int GetNumColumns() const;
	const std::wstring& GetString() const;

	void UpdateCharacter(int i, wchar_t c);
	void InsertCharacter(int i, wchar_t c);
	void RemoveCharacter(int i);

	void Assign(const std::wstring& str);
	void Assign(std::wstring&& str);

	bool DoLexicalAnalysis(bool inComment);
	const std::vector<Token>& GetTokens() const;

	int GetFlattenedString(int start, int end, wchar_t* buf) const;
	int GetFlattenedColumn(int col) const;

protected:
	void UpdateInternalData();

	int mExtent;
	std::wstring mString;
	std::vector<int> mIntegralUnits;

	std::vector<Token> mTokens;
};

class CodeBuffer
{
public:
	CodeBuffer();

	void Clear();

	int GetWorldWidth() const;
	int GetWorldHeight() const;
	const std::vector<CodeLine>& GetLines() const;

	int GetNumLines() const;
	int GetNumUnits(int l) const;
	int GetNumColumns(int l) const;

	int GetExtent(int l) const;
	int GetExtentTo(int l, int i) const;
	int GetColumnFromPoint(int l, int x) const;
	int GetUnitFromColumn(int l, int i) const;
	int GetColumnFromUnit(int l, int u) const;

	void UpdateCharacter(int l, int i, wchar_t c);
	void InsertCharacter(int l, int i, wchar_t c);
	void RemoveCharacter(int l, int i);

	std::wstring CopyBlock(int lMin, int iMin, int lMax, int iMax) const;
	void Insert(int l, int i, const std::wstring& text, int& row, int& col);
	void RemoveBlock(int lMin, int iMin, int lMax, int iMax);
	void SplitToTwoLines(int l, int i);
	void MergeToPrevLine(int l);
	void MergeWithNextLine(int l);

	std::wstring GetString() const;

protected:
	void Reset();

	void UpdateWorldWidth();
	void UpdateWorldHeight();
	void DoLexicalAnalysis();

	std::vector<CodeLine> mLines;
	int mWorldWidth;
	int mWorldHeight;
};

class HSUI_API ICodeItemDropTarget
{
public:
	virtual void HandleCodeItemDrop(CPoint point, const std::wstring& codeItem) = 0;

protected:
	virtual ~ICodeItemDropTarget();
};

class CodeWnd
	:
	public CWnd,
	public ICodeItemDropTarget
{
	DECLARE_MESSAGE_MAP()

public:
	CodeWnd();
	virtual ~CodeWnd();

	void CreateWnd(CWnd* parent, int x, int y, int w, int h);

	void SetContent(const std::wstring& text);
	std::wstring GetContent() const;

	void GetCursorFromPoint(CPoint point, int& row, int& col) const;
	void InsertString(int row, int col, const std::wstring& str);

	int GetCursorRow() const;
	int GetCursorColumn() const;

	afx_msg void OnSelectAll();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	afx_msg int OnCreate(CREATESTRUCT* createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);

	afx_msg void OnSetFocus(CWnd* oldWnd);
	afx_msg void OnKillFocus(CWnd* newWnd);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg LRESULT OnSetContext(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnComposition(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnTimer(UINT_PTR timerID);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	virtual void HandleCodeItemDrop(CPoint point, const std::wstring& codeItem) override;

	void Reset();
	void RestartCaretBlink();
	void GetSelectionBlock(int& srow, int& scol, int& erow, int& ecol) const;
	bool RemoveSelection();
	void AutoScroll();

	void RenderBase(const CRect& viewRect);
	void RenderContent(const CRect& viewRect);

	CodeEditor* mEditor;
	CodeBuffer* mBuffer;

	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mSelectionBrush;
	ID2D1SolidColorBrush* mTextBrushNormal;
	ID2D1SolidColorBrush* mTextBrushSpecial;
	ID2D1SolidColorBrush* mTextBrushComment;
	ID2D1SolidColorBrush* mCaretBrush;
	ID2D1SolidColorBrush* mCompositionBrush;

	int mCaretRow;
	int mCaretCol;

	bool mShowCaret;
	bool mComposing;
	int mPivotCaretUnit;

	bool mSelection;
	int mSelRow;
	int mSelCol;
};

class LineWnd : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	LineWnd();
	virtual ~LineWnd();

	void CreateWnd(CWnd* parent, int x, int y, int w, int h);

protected:
	afx_msg int OnCreate(CREATESTRUCT* createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	CodeEditor* mEditor;
	CodeBuffer* mBuffer;

	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mTextBrush;
};

class InfoGadget : public CWnd
{
	DECLARE_MESSAGE_MAP()

public:
	InfoGadget();
	virtual ~InfoGadget();

	static const int FIXED_WIDTH;
	void CreateWnd(CWnd* parent, int x, int y);

protected:
	afx_msg int OnCreate(CREATESTRUCT* createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	CodeEditor* mEditor;

	xsc::D2DRenderer mRenderer;
	ID2D1SolidColorBrush* mBrush;
};

class VirtualScroll
{
public:
	VirtualScroll();

	void SetRange(int range);

	void SetScrollPosition(int pos);
	int GetScrollPosition() const;

protected:
	int mRange;
	int mPosition;
};

class HSUI_API CodeEditor
	:
	public CWnd,
	public ScrollBar::IListener
{
	DECLARE_MESSAGE_MAP()

public:
	CodeEditor();
	virtual ~CodeEditor();

	void CreateWnd(CWnd* parent, int x, int y, int w, int h,
		bool vertScrollBar = true, bool horzScrollBar = true, bool lineNumber = true);

	void SetBorderColor(D2D_COLOR_F color);

	void SetContent(const std::wstring& text);
	std::wstring GetContent() const;
	void CopyToClipboard();

	void EnableEditor();
	void DisableEditor();

protected:
	afx_msg int OnCreate(CREATESTRUCT* createStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* oldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	void OnScrollPositionChanged(ScrollBar* scrollBar) override final;
	void OnBufferGeometryChanged();
	void OnCursorPositionChanged();

	void LayoutChildren(int cx, int cy);
	void AdjustScrollRange();

	void DoAutoScroll(int xOffset, int yOffset);
	void DoWheelScroll(int delta);

	CodeBuffer* GetCodeBuffer() const;
	int GetVertOffset() const;
	int GetHorzOffset() const;
	int GetCursorRow() const;
	int GetCursorColumn() const;

	std::unique_ptr<CodeBuffer> mCodeBuffer;

	CodeWnd mCodeWnd;
	LineWnd mLineWnd;

	VertScrollBar mVertScrollBar;
	VirtualScroll mVertScroll;

	HorzScrollBar mHorzScrollBar;
	VirtualScroll mHorzScroll;
	InfoGadget mInfoGadget;

	CBrush mBrush;

	IDWriteTextFormat* mCodeFont;
	IDWriteTextFormat* mDecoFont;

	friend class CodeBuffer;
	friend class CodeWnd;
	friend class LineWnd;
	friend class InfoGadget;
};

} // namespace hsui
