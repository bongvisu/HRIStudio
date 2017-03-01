#include "hsuiPrecompiled.h"
#include "hsuiCodeEditor.h"
#include "hsuiEditCtrl.h"
#include "hsuiVisualManager.h"
#include <imm.h>
#include <xscDWrite.h>
#include <xscStringUtil.h>

#undef min
#undef max

using namespace std;
using namespace xsc;

namespace hsui {

static const int FIXED_UNIT_PITCH = 7;
static const int FIXED_WIDE_PITCH = 14;
static const int FIXED_LINE_HEIGHT = 18;
static const int FIXED_TAB_UNITS = 4;

static wchar_t gsLineBuf[4096];

inline static int GetCharacterUnits(int accumulated, wchar_t c)
{
	if (32 <= c && c <= 176)
	{
		return 1;
	}
	else if (c == L'\t')
	{
		return (FIXED_TAB_UNITS - (accumulated % FIXED_TAB_UNITS));
	}
	return 2;
}

Token::Token()
{
}

Token::Token(Type argType, int argIndex, int argCount)
	:
	type(argType),
	index(argIndex),
	count(argCount)
{
}

CodeLine::CodeLine()
{
	UpdateInternalData();
}

CodeLine::CodeLine(const wstring& str) : mString(str)
{
	UpdateInternalData();
}

CodeLine::CodeLine(wstring&& str) : mString(std::move(str))
{
	UpdateInternalData();
}

CodeLine::CodeLine(const CodeLine& line)
{
	this->operator=(line);
}

CodeLine& CodeLine::operator=(const CodeLine& line)
{
	mExtent = line.mExtent;
	mString = line.mString;
	mIntegralUnits = line.mIntegralUnits;
	return *this;
}

CodeLine::CodeLine(CodeLine&& line)
{
	this->operator=(std::move(line));
}

CodeLine& CodeLine::operator=(CodeLine&& line)
{
	mExtent = line.mExtent;
	mString = std::move(line.mString);
	mIntegralUnits = std::move(line.mIntegralUnits);
	return *this;
}

int CodeLine::GetExtent() const
{
	return mExtent;
}

const wstring& CodeLine::GetString() const
{
	return mString;
}

int CodeLine::GetNumUnits() const
{
	return mIntegralUnits[mString.length()];
}

int CodeLine::GetNumColumns() const
{
	return static_cast<int>(mString.length());
}

int CodeLine::GetExtentTo(int j) const
{
	assert(0 <= j && j <= mString.length());
	return (mIntegralUnits[j] * FIXED_UNIT_PITCH);
}

int CodeLine::GetColumnFromPoint(int x) const
{
	assert(x >= 0);

	int lineLen = static_cast<int>(mString.length());
	if (x >= mExtent)
	{
		return lineLen;
	}

	int pivot;
	for (int i = 0; i < lineLen; ++i)
	{
		pivot = mIntegralUnits[i + 1] * FIXED_UNIT_PITCH;
		if (x < pivot)
		{
			return i;
		}
	}

	assert(false);
	return 0;
}

int CodeLine::GetColumnFromPoint(int x, int& extent) const
{
	assert(x >= 0);

	int lineLen = static_cast<int>(mString.length());
	if (x >= mExtent)
	{
		extent = mExtent;
		return lineLen;
	}

	int pivot;
	for (int i = 0; i < lineLen; ++i)
	{
		pivot = mIntegralUnits[i + 1] * FIXED_UNIT_PITCH;
		if (x < pivot)
		{
			extent = mIntegralUnits[i] * FIXED_UNIT_PITCH;
			return i;
		}
	}

	assert(false);
	return 0;
}

int CodeLine::GetColumnFromUnit(int units) const
{
	int lineLen = static_cast<int>(mString.length());
	for (int i = 0; i < lineLen; ++i)
	{
		if (units < mIntegralUnits[i + 1])
		{
			return i;
		}
	}
	return lineLen;
}

int CodeLine::GetUnitFromColumn(int column) const
{
	int lineLen = static_cast<int>(mString.length());
	if (column < lineLen)
	{
		return mIntegralUnits[column];
	}
	return mIntegralUnits[lineLen];
}

void CodeLine::UpdateCharacter(int i, wchar_t c)
{
	assert(0 <= i && i < mString.length());
	mString[i] = c;
}

void CodeLine::InsertCharacter(int i, wchar_t c)
{
	assert(0 <= i && i <= mString.length());
	mString.insert(i, 1, c);

	UpdateInternalData();
}

void CodeLine::RemoveCharacter(int i)
{
	assert(0 <= i && i < mString.length());
	mString.erase(i, 1);

	UpdateInternalData();
}

void CodeLine::Assign(const wstring& str)
{
	mString = str;

	UpdateInternalData();
}

void CodeLine::Assign(wstring&& str)
{
	mString = std::move(str);

	UpdateInternalData();
}

void CodeLine::UpdateInternalData()
{
	int lineLen = static_cast<int>(mString.length());
	mIntegralUnits.resize(lineLen + 1);

	int accumulated = 0;
	const wchar_t* lineData = mString.data();
	for (int i = 0; i < lineLen; ++i)
	{
		mIntegralUnits[i] = accumulated;
		accumulated += GetCharacterUnits(accumulated, lineData[i]);
	}
	mIntegralUnits[lineLen] = accumulated;

	mExtent = accumulated * FIXED_UNIT_PITCH;
}

int CodeLine::GetFlattenedString(int start, int end, wchar_t* buf) const
{
	int bufIndex = 0;

	const wchar_t* lineData = mString.data();
	for (int i = start; i <= end; ++i)
	{
		if (lineData[i] == L'\t')
		{
			int numUnits = mIntegralUnits[i + 1] - mIntegralUnits[i];
			for (int j = 0; j < numUnits; ++j)
			{
				if (buf) buf[bufIndex] = L' ';
				bufIndex++;
			}
		}
		else
		{
			if (buf) buf[bufIndex] = lineData[i];
			bufIndex++;
		}
	}

	return bufIndex;
}

int CodeLine::GetFlattenedColumn(int col) const
{
	return mIntegralUnits[col];
}

bool CodeLine::DoLexicalAnalysis(bool inComment)
{
	const wchar_t* lineData = mString.c_str();
	auto contentStart = mString.find_first_not_of(L" \t");
	auto contentEnd = mString.find_last_not_of(L" \t");

	mTokens.clear();
	if (contentStart == wstring::npos || contentEnd == wstring::npos)
	{
		return inComment;
	}

	int start = static_cast<int>(contentStart);
	int end = static_cast<int>(contentEnd);
	int len = end - start + 1;

	if (inComment)
	{
		Token token(Token::TYPE_COMMENT, start, len);
		mTokens.push_back(token);
		if (len >= 2)
		{
			if (lineData[end - 1] == L'*' && lineData[end] == L'/')
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}

	if (len >= 2)
	{
		if (lineData[start] == L'/' && lineData[start + 1] == L'*')
		{
			mTokens.push_back({ Token::TYPE_COMMENT, start, len });
			if (len >= 4)
			{
				if (lineData[end - 1] == L'*' && lineData[end] == L'/')
				{
					return false;
				}
				else
				{
					return true;
				}
			}
			else
			{
				return true;
			}
		}
	}

	Token tailComment;
	auto tailCommentStart = mString.find(L"//");
	if (tailCommentStart != wstring::npos)
	{
		int commentStart = static_cast<int>(tailCommentStart);
		tailComment.type = Token::TYPE_COMMENT;
		tailComment.index = commentStart;
		tailComment.count = end - commentStart + 1;
		if (commentStart == start)
		{
			mTokens.push_back(tailComment);
			return false;
		}
		end = commentStart - 1;
	}

	len = end - start + 1;
	assert(len >= 1);
#if 0
	mTokens.push_back({ Token::TYPE_GENERIC, start, len });
#else
	Token contentToken;
	int genericStart = -1;
	for (int i = start; i <= end; i++)
	{
		if (lineData[i] == L' ' ||
			lineData[i] == L'\t')
		{
			if (genericStart != -1)
			{
				contentToken.type = Token::TYPE_GENERIC;
				contentToken.index = genericStart;
				contentToken.count = i - genericStart;
				mTokens.push_back(contentToken);

				genericStart = -1;
			}
			continue;
		}

		if (lineData[i] == L'<' ||
			lineData[i] == L'>' ||
			lineData[i] == L'=' ||
			lineData[i] == L'!' ||
			lineData[i] == L'@' ||
			lineData[i] == L'{' ||
			lineData[i] == L'}' ||
			lineData[i] == L'+' ||
			lineData[i] == L'-' ||
			lineData[i] == L'|' ||
			lineData[i] == L'(' ||
			lineData[i] == L')' ||
			lineData[i] == L'*')
		{
			if (genericStart != -1)
			{
				contentToken.type = Token::TYPE_GENERIC;
				contentToken.index = genericStart;
				contentToken.count = i - genericStart;
				mTokens.push_back(contentToken);

				genericStart = -1;
			}

			contentToken.type = Token::TYPE_SPECIAL;
			contentToken.index = i;
			contentToken.count = 1;
			mTokens.push_back(contentToken);
		}
		else if (genericStart == -1)
		{
			genericStart = i;
		}
	}
	if (genericStart != -1)
	{
		contentToken.type = Token::TYPE_GENERIC;
		contentToken.index = genericStart;
		contentToken.count = end - genericStart + 1;
		mTokens.push_back(contentToken);
	}
#endif

	if (tailCommentStart != wstring::npos)
	{
		mTokens.push_back(tailComment);
	}
	return false;
}

const vector<Token>& CodeLine::GetTokens() const
{
	return mTokens;
}

CodeBuffer::CodeBuffer()
{
	Reset();
}

void CodeBuffer::Clear()
{
	mLines.clear();
	Reset();
}

void CodeBuffer::Reset()
{
	mLines.push_back(CodeLine());

	mWorldWidth = 0;
	mWorldHeight = FIXED_LINE_HEIGHT;
}

int CodeBuffer::GetWorldWidth() const
{
	return mWorldWidth;
}

int CodeBuffer::GetWorldHeight() const
{
	return mWorldHeight;
}

const vector<CodeLine>& CodeBuffer::GetLines() const
{
	return mLines;
}

int CodeBuffer::GetNumLines() const
{
	return static_cast<int>(mLines.size());
}

int CodeBuffer::GetExtentTo(int l, int i) const
{
	assert(0 <= l && l < mLines.size());
	return mLines[l].GetExtentTo(i);
}

int CodeBuffer::GetColumnFromPoint(int l, int x) const
{
	assert(0 <= l && l < mLines.size());
	return mLines[l].GetColumnFromPoint(x);
}

int CodeBuffer::GetUnitFromColumn(int l, int c) const
{
	assert(0 <= l && l < mLines.size());
	return mLines[l].GetUnitFromColumn(c);
}

int CodeBuffer::GetColumnFromUnit(int l, int u) const
{
	assert(0 <= l && l < mLines.size());
	return mLines[l].GetColumnFromUnit(u);
}

int CodeBuffer::GetNumUnits(int l) const
{
	assert(0 <= l && l < mLines.size());
	return mLines[l].GetNumUnits();
}

int CodeBuffer::GetExtent(int l) const
{
	assert(0 <= l && l < mLines.size());
	return mLines[l].GetExtent();
}

int CodeBuffer::GetNumColumns(int l) const
{
	assert(0 <= l && l < mLines.size());
	return static_cast<int>(mLines[l].GetString().length());
}

void CodeBuffer::DoLexicalAnalysis()
{
	bool inComment = false;
	for (auto& line : mLines)
	{
		inComment = line.DoLexicalAnalysis(inComment);
	}
}

void CodeBuffer::UpdateCharacter(int l, int i, wchar_t c)
{
	assert(0 <= l && l < mLines.size());
	mLines[l].UpdateCharacter(i, c);

	DoLexicalAnalysis();
}

void CodeBuffer::InsertCharacter(int l, int i, wchar_t c)
{
	assert(0 <= l && l < mLines.size());
	mLines[l].InsertCharacter(i, c);

	UpdateWorldWidth();
	DoLexicalAnalysis();
}

void CodeBuffer::RemoveCharacter(int l, int i)
{
	assert(0 <= l && l < mLines.size());
	mLines[l].RemoveCharacter(i);

	UpdateWorldWidth();
	DoLexicalAnalysis();
}

wstring CodeBuffer::GetString() const
{
	int lMax = static_cast<int>(mLines.size()) - 1;
	int iMax = mLines[lMax].GetNumColumns();
	return CopyBlock(0, 0, lMax, iMax);
}

wstring CodeBuffer::CopyBlock(int lMin, int iMin, int lMax, int iMax) const
{
	wstring ret;

	assert(0 <= lMin && lMin < mLines.size());
	assert(0 <= lMax && lMax < mLines.size());
	assert(0 <= iMin && iMin <= mLines[lMin].GetNumColumns());
	assert(0 <= iMax && iMax <= mLines[lMax].GetNumColumns());

	if (lMin == lMax)
	{
		ret = std::move(mLines[lMin].GetString().substr(iMin, iMax - iMin));
	}
	else
	{
		for (int i = lMin; i <= lMax; ++i)
		{
			if (i == lMin)
			{
				ret = mLines[i].GetString().substr(iMin) + L"\n";
			}
			else if (i == lMax)
			{
				ret += mLines[i].GetString().substr(0, iMax);
			}
			else
			{
				ret += mLines[i].GetString() + L"\n";
			}
		}
	}

	return ret;
}

void CodeBuffer::Insert(int l, int i, const wstring& text, int& row, int& col)
{
	assert(0 <= l && l < mLines.size());
	int lineLen = mLines[l].GetNumColumns();
	assert(0 <= i && i <= lineLen);

	StringUtil::StringList lines;
	StringUtil::ToLines(text, lines, true);
	int numLines = static_cast<int>(lines.size());
	if (numLines == 0)
	{
		row = l;
		col = i;
		return;
	}

	row = l + numLines - 1;

	wstring firstHalf(std::move(mLines[l].GetString().substr(0, i)));
	wstring secondHalf(std::move(mLines[l].GetString().substr(i)));

	mLines[l].Assign(std::move(firstHalf + lines[0]));
	for (int i = 1; i < numLines; ++i)
	{
		mLines.insert(mLines.begin() + l + i, std::move(lines[i]));
	}

	col = mLines[row].GetNumColumns();
	mLines[row].Assign(std::move(mLines[row].GetString() + secondHalf));

	UpdateWorldWidth();
	UpdateWorldHeight();
	DoLexicalAnalysis();
}

void CodeBuffer::RemoveBlock(int lMin, int iMin, int lMax, int iMax)
{
	assert(0 <= lMin && lMin < mLines.size());
	assert(0 <= lMax && lMax < mLines.size());
	assert(0 <= iMin && iMin <= mLines[lMin].GetString().length());
	assert(0 <= iMax && iMax <= mLines[lMax].GetString().length());

	wstring strMin(std::move(mLines[lMin].GetString().substr(0, iMin)));
	wstring strMax(std::move(mLines[lMax].GetString().substr(iMax)));
	mLines.erase(mLines.begin() + lMin, mLines.begin() + lMax);
	mLines[lMin].Assign(std::move(strMin + strMax));

	UpdateWorldWidth();
	UpdateWorldHeight();
	DoLexicalAnalysis();
}

void CodeBuffer::SplitToTwoLines(int l, int i)
{
	assert(0 <= l && l < mLines.size());
	assert(0 <= i && i <= mLines[l].GetString().length());

	const wstring& currLine = mLines[l].GetString();

	wstring firstLine(std::move(currLine.substr(0, i)));
	wstring secondLine(currLine.substr(i));

	mLines[l].Assign(std::move(firstLine));
	mLines.insert(mLines.begin() + l + 1, std::move(secondLine));

	UpdateWorldWidth();
	UpdateWorldHeight();
	DoLexicalAnalysis();
}

void CodeBuffer::MergeToPrevLine(int l)
{
	assert(0 < l && l < mLines.size());

	CodeLine currLine(std::move(mLines[l]));
	mLines.erase(mLines.begin() + l, mLines.begin() + l + 1);

	int prevLine = l - 1;
	const wstring& prevLineString(mLines[prevLine].GetString());
	mLines[prevLine].Assign(std::move(prevLineString + currLine.GetString()));

	UpdateWorldWidth();
	UpdateWorldHeight();
	DoLexicalAnalysis();
}

void CodeBuffer::MergeWithNextLine(int l)
{
	assert(0 <= l && l < (mLines.size() - 1));

	int nextLine = l + 1;
	wstring mergedString(std::move(mLines[l].GetString() + mLines[nextLine].GetString()));
	mLines[l].Assign(std::move(mergedString));
	mLines.erase(mLines.begin() + nextLine, mLines.begin() + nextLine + 1);

	UpdateWorldWidth();
	UpdateWorldHeight();
	DoLexicalAnalysis();
}

void CodeBuffer::UpdateWorldWidth()
{
	mWorldWidth = 0;
	for (auto& line : mLines)
	{
		mWorldWidth = std::max(mWorldWidth, line.GetExtent());
	}
}

void CodeBuffer::UpdateWorldHeight()
{
	mWorldHeight = static_cast<int>(mLines.size()) * FIXED_LINE_HEIGHT;
}

ICodeItemDropTarget::~ICodeItemDropTarget()
{
}

static const UINT CW_CARET_TIMER = 1000;

BEGIN_MESSAGE_MAP(CodeWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()

	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()

	ON_WM_MOUSEWHEEL()

	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_COMMAND(ID_EDIT_CUT, OnCut)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)

	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_MESSAGE(WM_IME_SETCONTEXT, OnSetContext)
	ON_MESSAGE(WM_IME_COMPOSITION, OnComposition)
END_MESSAGE_MAP()

CodeWnd::CodeWnd()
{
	Reset();
	mShowCaret = false;
}

CodeWnd::~CodeWnd()
{
}

void CodeWnd::HandleCodeItemDrop(CPoint point, const wstring& codeItem)
{
	ScreenToClient(&point);

	int row, col;
	GetCursorFromPoint(point, row, col);

	InsertString(row, col, codeItem);

	SetFocus();
}

void CodeWnd::Reset()
{
	mCaretRow = 0;
	mCaretCol = 0;
	mPivotCaretUnit = -1;

	mComposing = false;
	mSelection = false;
	mSelRow = -1;
	mSelCol = -1;
}

void CodeWnd::CreateWnd(CWnd* parent, int x, int y, int w, int h)
{
	mEditor = static_cast<CodeEditor*>(parent);
	mBuffer = mEditor->GetCodeBuffer();

	auto className = AfxRegisterWndClass(0, LoadCursorW(nullptr, IDC_IBEAM));

	int r = x + w;
	int b = y + h;
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
	Create(className, nullptr, style, CRect(x, y, r, b), parent, 0);
}

void CodeWnd::SetContent(const std::wstring& text)
{
	assert(GetSafeHwnd());

	Reset();

	mBuffer->Clear();
	mBuffer->Insert(0, 0, text, mCaretRow, mCaretCol);

	mEditor->OnBufferGeometryChanged();

	Invalidate(FALSE);
}

wstring CodeWnd::GetContent() const
{
	return std::move(mBuffer->GetString());
}

void CodeWnd::InsertString(int row, int col, const wstring& str)
{
	mBuffer->Insert(row, col, str, mCaretRow, mCaretCol);

	mSelection = true;
	mSelRow = row;
	mSelCol = col;

	mEditor->OnBufferGeometryChanged();

	Invalidate(FALSE);
}

int CodeWnd::GetCursorRow() const
{
	return mCaretRow;
}

int CodeWnd::GetCursorColumn() const
{
#if 1
	auto& lines = mBuffer->GetLines();
	auto& line = lines[mCaretRow];
	return line.GetFlattenedColumn(mCaretCol);
#else
	return mCaretCol;
#endif
}

void CodeWnd::OnSetFocus(CWnd* oldWnd)
{
	RestartCaretBlink();
	Invalidate(FALSE);
}

void CodeWnd::OnKillFocus(CWnd* newWnd)
{
	KillTimer(CW_CARET_TIMER);

	mShowCaret = false;
	Invalidate(FALSE);
}

void CodeWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	GetCursorFromPoint(point, mCaretRow, mCaretCol);

	mSelection = true;
	mSelRow = mCaretRow;
	mSelCol = mCaretCol;

	AutoScroll();

	mEditor->OnCursorPositionChanged();

	mPivotCaretUnit = -1;
	RestartCaretBlink();
	Invalidate(FALSE);

	SetCapture();
}

void CodeWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		OnMouseMove(0, point);
		ReleaseCapture();
	}
}

void CodeWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetCapture() == this)
	{
		GetCursorFromPoint(point, mCaretRow, mCaretCol);

		AutoScroll();

		mEditor->OnCursorPositionChanged();

		mPivotCaretUnit = -1;
		RestartCaretBlink();
		Invalidate(FALSE);
	}
}

BOOL CodeWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	mEditor->DoWheelScroll(-zDelta);

	Invalidate(FALSE);
	return TRUE;
}

LRESULT CodeWnd::OnSetContext(WPARAM wParam, LPARAM lParam)
{
	lParam &= ~(ISC_SHOWUICOMPOSITIONWINDOW | ISC_SHOWUIALLCANDIDATEWINDOW);
	return CWnd::DefWindowProc(WM_IME_SETCONTEXT, wParam, lParam);
}

LRESULT CodeWnd::OnComposition(WPARAM wParam, LPARAM compFlag)
{
	bool bufferGeometryChanged = false;

	if (RemoveSelection())
	{
		bufferGeometryChanged = true;
	}

	HIMC imc = ImmGetContext(m_hWnd);

	wchar_t c;
	LONG ret = 0;
	if (compFlag & GCS_COMPSTR)
	{
		ret = ImmGetCompositionStringW(imc, GCS_COMPSTR, &c, 2);
	}
	else if (compFlag & GCS_RESULTSTR)
	{
		ret = ImmGetCompositionStringW(imc, GCS_RESULTSTR, &c, 2);
	}

	if (ret > 0)
	{
		if (compFlag & GCS_RESULTSTR)
		{
			mComposing = false;

			mBuffer->UpdateCharacter(mCaretRow, mCaretCol, c);
			mCaretCol = mCaretCol + 1;

			mPivotCaretUnit = -1;
			RestartCaretBlink();
		}
		else
		{
			if (mComposing)
			{
				mBuffer->UpdateCharacter(mCaretRow, mCaretCol, c);
			}
			else
			{
				mComposing = true;

				mBuffer->InsertCharacter(mCaretRow, mCaretCol, c);

				bufferGeometryChanged = true;

				RestartCaretBlink();
			}
		}
	}
	else
	{
		mComposing = false;

		mBuffer->RemoveCharacter(mCaretRow, mCaretCol);

		bufferGeometryChanged = true;

		RestartCaretBlink();
	}

	ImmReleaseContext(m_hWnd, imc);

	if (bufferGeometryChanged)
	{
		mEditor->OnBufferGeometryChanged();
	}
	else
	{
		mEditor->OnCursorPositionChanged();
	}

	AutoScroll();

	Invalidate(FALSE);
	return 0;
}

void CodeWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bufferGeometryChanged = false;

	if (nChar == VK_BACK)
	{
		if (mSelection)
		{
			if (RemoveSelection())
			{
				bufferGeometryChanged = true;

				mPivotCaretUnit = -1;
				RestartCaretBlink();
			}
		}
		else
		{
			if (mCaretCol > 0)
			{
				mCaretCol -= 1;
				mBuffer->RemoveCharacter(mCaretRow, mCaretCol);

				bufferGeometryChanged = true;

				mPivotCaretUnit = -1;
				RestartCaretBlink();
			}
			else if (mCaretRow > 0)
			{
				int prevRow = mCaretRow - 1;
				mCaretCol = mBuffer->GetNumColumns(prevRow);
				mBuffer->MergeToPrevLine(mCaretRow);
				mCaretRow = prevRow;

				bufferGeometryChanged = true;

				mPivotCaretUnit = -1;
				RestartCaretBlink();
			}
		}
	}
	else if (nChar == VK_RETURN)
	{
		RemoveSelection();

		mBuffer->SplitToTwoLines(mCaretRow, mCaretCol);
		mCaretRow += 1;
		mCaretCol = 0;

		bufferGeometryChanged = true;

		mPivotCaretUnit = -1;
		RestartCaretBlink();
	}
	else if (nChar != VK_ESCAPE && nChar != '\n')
	{
		RemoveSelection();

		mBuffer->InsertCharacter(mCaretRow, mCaretCol, static_cast<wchar_t>(nChar));
		mCaretCol += 1;

		bufferGeometryChanged = true;

		mPivotCaretUnit = -1;
		RestartCaretBlink();
	}

	if (bufferGeometryChanged)
	{
		mEditor->OnBufferGeometryChanged();
	}
	else
	{
		mEditor->OnCursorPositionChanged();
	}

	AutoScroll();

	Invalidate(FALSE);
}

void CodeWnd::RestartCaretBlink()
{
	mShowCaret = true;
	SetTimer(CW_CARET_TIMER, 500, nullptr);
}

void CodeWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bufferGeometryChanged = false;

	if (nChar == VK_LEFT ||
		nChar == VK_RIGHT ||
		nChar == VK_UP ||
		nChar == VK_DOWN ||
		nChar == VK_HOME ||
		nChar == VK_END ||
		nChar == VK_PRIOR ||
		nChar == VK_NEXT)
	{
		if (GetKeyState(VK_SHIFT) & 0x8000)
		{
			if (!mSelection)
			{
				mSelRow = mCaretRow;
				mSelCol = mCaretCol;
				mSelection = true;
			}
		}
		else
		{
			mSelection = false;
		}
	}
	else if (nChar == VK_ESCAPE)
	{
		mSelection = false;
	}

	if (nChar == VK_UP)
	{
		if (mPivotCaretUnit < 0)
		{
			mPivotCaretUnit = mBuffer->GetUnitFromColumn(mCaretRow, mCaretCol);
		}

		if (mCaretRow > 0)
		{
			mCaretRow -= 1;
			int lineUnits = mBuffer->GetNumUnits(mCaretRow);
			int minUnit = std::min(mPivotCaretUnit, lineUnits);
			mCaretCol = mBuffer->GetColumnFromUnit(mCaretRow, minUnit);

			RestartCaretBlink();
		}
	}
	else if (nChar == VK_DOWN)
	{
		if (mPivotCaretUnit < 0)
		{
			mPivotCaretUnit = mBuffer->GetUnitFromColumn(mCaretRow, mCaretCol);
		}

		int lastLine = mBuffer->GetNumLines() - 1;
		if (mCaretRow < lastLine)
		{
			mCaretRow += 1;
			int lineUnits = mBuffer->GetNumUnits(mCaretRow);
			int minUnit = std::min(mPivotCaretUnit, lineUnits);
			mCaretCol = mBuffer->GetColumnFromUnit(mCaretRow, minUnit);

			RestartCaretBlink();
		}
	}
	else if (nChar == VK_PRIOR)
	{
		if (mPivotCaretUnit < 0)
		{
			mPivotCaretUnit = mBuffer->GetUnitFromColumn(mCaretRow, mCaretCol);
		}

		if (mCaretRow > 0)
		{
			CRect rect;
			GetClientRect(&rect);
			int height = rect.Height();
			int pageRows = height / FIXED_LINE_HEIGHT;
			mCaretRow = std::max(0, mCaretRow - pageRows);
			int lineUnits = mBuffer->GetNumUnits(mCaretRow);
			int minUnit = std::min(mPivotCaretUnit, lineUnits);
			mCaretCol = mBuffer->GetColumnFromUnit(mCaretRow, minUnit);

			RestartCaretBlink();
		}
	}
	else if (nChar == VK_NEXT)
	{
		if (mPivotCaretUnit < 0)
		{
			mPivotCaretUnit = mBuffer->GetUnitFromColumn(mCaretRow, mCaretCol);
		}

		int lastLine = mBuffer->GetNumLines() - 1;
		if (mCaretRow < lastLine)
		{
			CRect rect;
			GetClientRect(&rect);
			int height = rect.Height();
			int pageRows = height / FIXED_LINE_HEIGHT;
			mCaretRow = std::min(lastLine, mCaretRow + pageRows);
			int lineUnits = mBuffer->GetNumUnits(mCaretRow);
			int minUnit = std::min(mPivotCaretUnit, lineUnits);
			mCaretCol = mBuffer->GetColumnFromUnit(mCaretRow, minUnit);

			RestartCaretBlink();
		}
	}
	else if (nChar == VK_LEFT)
	{
		if (mCaretCol > 0)
		{
			mCaretCol -= 1;

			mPivotCaretUnit = -1;
			RestartCaretBlink();
		}
		else if (mCaretRow > 0)
		{
			mCaretRow -= 1;
			mCaretCol = mBuffer->GetNumColumns(mCaretRow);

			mPivotCaretUnit = -1;
			RestartCaretBlink();
		}
	}
	else if (nChar == VK_RIGHT)
	{
		int lastLine = mBuffer->GetNumLines() - 1;
		int lineLen = mBuffer->GetNumColumns(mCaretRow);

		if (mCaretCol < lineLen)
		{
			mCaretCol += 1;

			mPivotCaretUnit = -1;
			RestartCaretBlink();
		}
		else if (mCaretRow < lastLine)
		{
			mCaretRow += 1;
			mCaretCol = 0;

			mPivotCaretUnit = -1;
			RestartCaretBlink();
		}
	}
	else if (nChar == VK_HOME)
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			if (mCaretRow != 0 || mCaretCol != 0)
			{
				mCaretRow = 0;
				mCaretCol = 0;

				mPivotCaretUnit = -1;
				RestartCaretBlink();
			}
		}
		else
		{
			if (mCaretCol != 0)
			{
				mCaretCol = 0;

				mPivotCaretUnit = -1;
				RestartCaretBlink();
			}
		}
	}
	else if (nChar == VK_END)
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			mCaretRow = mBuffer->GetNumLines() - 1;
			mCaretCol = mBuffer->GetNumColumns(mCaretRow);

			mPivotCaretUnit = -1;
			RestartCaretBlink();
		}
		else
		{
			mCaretCol = mBuffer->GetNumColumns(mCaretRow);

			mPivotCaretUnit = -1;
			RestartCaretBlink();
		}
	}
	else if (nChar == VK_DELETE)
	{
		if (mSelection)
		{
			if (RemoveSelection())
			{
				bufferGeometryChanged = true;

				mPivotCaretUnit = -1;
				RestartCaretBlink();
			}
		}
		else
		{
			int lineLen = mBuffer->GetNumColumns(mCaretRow);
			if (mCaretCol == lineLen)
			{
				int lastLine = mBuffer->GetNumLines() - 1;
				if (mCaretRow < lastLine)
				{
					mBuffer->MergeWithNextLine(mCaretRow);

					bufferGeometryChanged = true;

					RestartCaretBlink();
				}
			}
			else
			{
				mBuffer->RemoveCharacter(mCaretRow, mCaretCol);

				bufferGeometryChanged = true;

				RestartCaretBlink();
			}
		}
	}

	if (bufferGeometryChanged)
	{
		mEditor->OnBufferGeometryChanged();
	}
	else
	{
		mEditor->OnCursorPositionChanged();
	}

	AutoScroll();

	Invalidate(FALSE);
}

void CodeWnd::OnSelectAll()
{
	mSelection = true;
	mSelRow = 0;
	mSelCol = 0;
	mCaretRow = mBuffer->GetNumLines() - 1;
	mCaretCol = mBuffer->GetNumColumns(mCaretRow);

	mPivotCaretUnit = -1;
	RestartCaretBlink();

	Invalidate(FALSE);
}

void CodeWnd::OnCut()
{
	if ((false == mSelection) || (mSelRow == mCaretRow && mSelCol == mCaretCol))
	{
		return;
	}

	OnCopy();

	RemoveSelection();

	mEditor->OnBufferGeometryChanged();

	AutoScroll();

	mPivotCaretUnit = -1;
	RestartCaretBlink();

	Invalidate(FALSE);
}

void CodeWnd::OnCopy()
{
	if ((false == mSelection) || (mSelRow == mCaretRow && mSelCol == mCaretCol))
	{
		return;
	}

	int srow, scol, erow, ecol;
	GetSelectionBlock(srow, scol, erow, ecol);

	wstring cutString(std::move(mBuffer->CopyBlock(srow, scol, erow, ecol)));
	auto len = (cutString.length() + 1) * sizeof(wchar_t);
	HGLOBAL clipData = GlobalAlloc(GHND, len);
	void* buf = GlobalLock(clipData);
	memcpy(buf, cutString.data(), len);
	GlobalUnlock(clipData);

	OpenClipboard();
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, clipData);
	CloseClipboard();
}

void CodeWnd::OnPaste()
{
	OpenClipboard();
	HANDLE clipData = GetClipboardData(CF_UNICODETEXT);
	if (nullptr == clipData)
	{
		CloseClipboard();
		return;
	}
	wstring text(reinterpret_cast<wchar_t*>(GlobalLock(clipData)));
	GlobalUnlock(clipData);
	CloseClipboard();

	if (text.length() == 0)
	{
		return;
	}

	RemoveSelection();

	mBuffer->Insert(mCaretRow, mCaretCol, text, mCaretRow, mCaretCol);

	mEditor->OnBufferGeometryChanged();

	AutoScroll();

	mPivotCaretUnit = -1;
	RestartCaretBlink();

	Invalidate(FALSE);
}

BOOL CodeWnd::PreTranslateMessage(MSG* msg)
{
	if (TranslateAcceleratorW(m_hWnd, EditCtrl::accelerator, msg))
	{
		return TRUE;
	}
	return CWnd::PreTranslateMessage(msg);
}

void CodeWnd::AutoScroll()
{
	int xOffset = mEditor->GetHorzOffset();
	int yOffset = mEditor->GetVertOffset();

	CRect viewRect;
	GetClientRect(&viewRect);
	viewRect.OffsetRect(xOffset, yOffset);

	int caretX = mBuffer->GetExtentTo(mCaretRow, mCaretCol);
	int caretY = mCaretRow * FIXED_LINE_HEIGHT;
	int right = caretX + FIXED_WIDE_PITCH;
	int bottom = caretY + FIXED_LINE_HEIGHT;
	int xNew = xOffset;
	if (caretX < viewRect.left)
	{
		xNew = caretX;
	}
	else if (right > viewRect.right)
	{
		xNew += right - viewRect.right;
	}

	int yNew = yOffset;
	if (caretY < viewRect.top)
	{
		yNew = caretY;
	}
	else if (bottom > viewRect.bottom)
	{
		yNew += bottom - viewRect.bottom;
	}

	if (xOffset != xNew || yOffset != yNew)
	{
		mEditor->DoAutoScroll(xNew, yNew);
	}
}

void CodeWnd::GetCursorFromPoint(CPoint point, int& row, int& col) const
{
	int offsetX = mEditor->GetHorzOffset();
	int offsetY = mEditor->GetVertOffset();
	point.x += offsetX;
	point.y += offsetY;
	if (point.x < 0) point.x = 0;
	if (point.y < 0) point.y = 0;

	int numLines = mBuffer->GetNumLines();
	row = point.y / FIXED_LINE_HEIGHT;
	row = std::min(row, numLines - 1);
	assert(row >= 0);
	col = mBuffer->GetColumnFromPoint(row, point.x);
}

void CodeWnd::GetSelectionBlock(int& srow, int& scol, int& erow, int& ecol) const
{
	if (mSelRow < mCaretRow)
	{
		srow = mSelRow;
		scol = mSelCol;
		erow = mCaretRow;
		ecol = mCaretCol;
	}
	else if (mSelRow > mCaretRow)
	{
		srow = mCaretRow;
		scol = mCaretCol;
		erow = mSelRow;
		ecol = mSelCol;
	}
	else
	{
		srow = erow = mSelRow;
		if (mSelCol < mCaretCol)
		{
			scol = mSelCol;
			ecol = mCaretCol;
		}
		else
		{
			scol = mCaretCol;
			ecol = mSelCol;
		}
	}
}

bool CodeWnd::RemoveSelection()
{
	if ((false == mSelection) || (mSelRow == mCaretRow && mSelCol == mCaretCol))
	{
		mSelection = false;
		return false;
	}

	int yMin, xMin, yMax, xMax;
	GetSelectionBlock(yMin, xMin, yMax, xMax);
	mBuffer->RemoveBlock(yMin, xMin, yMax, xMax);

	mCaretRow = yMin;
	mCaretCol = xMin;

	mSelection = false;
	return true;
}

int CodeWnd::OnCreate(CREATESTRUCT* createStruct)
{
	CWnd::OnCreate(createStruct);

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL1, &mTextBrushNormal);
	mRenderer.CreateSolidColorBrush({ 1.0f, 0.0f, 0.0f, 1.0f }, &mTextBrushSpecial);
	mRenderer.CreateSolidColorBrush({ 0.0f, 0.6f, 0.0f, 1.0f }, &mTextBrushComment);

	FLOAT r = 156.0f / 255.0f;
	FLOAT g = 199.0f / 255.0f;
	FLOAT b = 230.0f / 255.0f;
	mRenderer.CreateSolidColorBrush({ r, g, b, 0.5f }, &mSelectionBrush);
	mRenderer.CreateSolidColorBrush({ 0, 0, 0, 0.7f }, &mCaretBrush);
	mRenderer.CreateSolidColorBrush({ 0.5f, 0.5f, 0.5f, 0.3f }, &mCompositionBrush);
	
	return 0;
}

void CodeWnd::OnDestroy()
{
	KillTimer(CW_CARET_TIMER);

	mCompositionBrush->Release();
	mCaretBrush->Release();
	mSelectionBrush->Release();
	mTextBrushSpecial->Release();
	mTextBrushNormal->Release();
	mTextBrushComment->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void CodeWnd::OnSize(UINT type, int w, int h)
{
	mRenderer.Resize(w, h);
}

void CodeWnd::OnTimer(UINT_PTR timerID)
{
	if (timerID == CW_CARET_TIMER)
	{
		if (GetFocus() == this)
		{
			mShowCaret = !mShowCaret;
		}
		else
		{
			mShowCaret = false;
		}
		Invalidate(FALSE);
	}
	else
	{
		CWnd::OnTimer(timerID);
	}
}

BOOL CodeWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CodeWnd::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	mRenderer.BeginDraw();
	if (IsWindowEnabled())
	{
		mRenderer.Clear(VisualManager::CLR_WHITE);
	}
	else
	{
		mRenderer.Clear(VisualManager::CLR_BASE);
	}

	int xOffset = mEditor->GetHorzOffset();
	int yOffset = mEditor->GetVertOffset();
	rect.OffsetRect(xOffset, yOffset);

	auto context = mRenderer.GetContext();

	D2D1_MATRIX_3X2_F xform;
	context->GetTransform(&xform);
	D2D1_MATRIX_3X2_F trans = xform;
	trans._31 = -static_cast<FLOAT>(xOffset);
	trans._32 = -static_cast<FLOAT>(yOffset);
	context->SetTransform(trans);

	RenderBase(rect);
	RenderContent(rect);

	context->SetTransform(xform);

	mRenderer.EndDraw();
	mRenderer.Display();
}

void CodeWnd::RenderBase(const CRect& viewRect)
{
	if ((false == mSelection) || (mSelRow == mCaretRow && mSelCol == mCaretCol))
	{
		return;
	}

	if (mSelRow == mCaretRow)
	{
		int start = std::min(mSelCol, mCaretCol);
		int end = std::max(mSelCol, mCaretCol);

		LONG startPos = mBuffer->GetExtentTo(mSelRow, start);
		LONG endPos = mBuffer->GetExtentTo(mSelRow, end);

		LONG visibleStart = std::max<LONG>(startPos, viewRect.left);
		LONG visibleEnd = std::min<LONG>(endPos, viewRect.right);

		D2D1_RECT_F rect;
		rect.left = static_cast<FLOAT>(visibleStart);
		rect.right = static_cast<FLOAT>(visibleEnd);
		rect.top = static_cast<FLOAT>(mSelRow * FIXED_LINE_HEIGHT);
		rect.bottom = rect.top + FIXED_LINE_HEIGHT;
		mRenderer.FillRectangle(rect, mSelectionBrush);
	}
	else
	{
		int startRow;
		int endRow;
		int startCol;
		int endCol;
		if (mSelRow < mCaretRow)
		{
			startRow = mSelRow;
			startCol = mSelCol;
			endRow = mCaretRow;
			endCol = mCaretCol;
		}
		else
		{
			startRow = mCaretRow;
			startCol = mCaretCol;
			endRow = mSelRow;
			endCol = mSelCol;
		}

		int visibleLineStart = viewRect.top / FIXED_LINE_HEIGHT;
		int visibleLineEnd = viewRect.bottom / FIXED_LINE_HEIGHT;

		LONG startPos;
		LONG endPos;
		LONG visibleStart;
		LONG visibleEnd;
		for (int i = visibleLineStart; i <= visibleLineEnd; ++i)
		{
			if (i < startRow)	continue;
			if (i > endRow)		break;

			startPos = 0;
			endPos = 0;
			if (i == startRow)
			{
				startPos = mBuffer->GetExtentTo(i, startCol);
				endPos = mBuffer->GetExtent(i) + FIXED_UNIT_PITCH;
			}
			else if (i == endRow)
			{
				endPos = mBuffer->GetExtentTo(i, endCol);
			}
			else
			{
				endPos = mBuffer->GetExtent(i) + FIXED_UNIT_PITCH;
			}

			visibleStart = std::max<LONG>(startPos, viewRect.left);
			visibleEnd = std::min<LONG>(endPos, viewRect.right);

			D2D1_RECT_F rect;
			rect.left = static_cast<FLOAT>(visibleStart);
			rect.right = static_cast<FLOAT>(visibleEnd);
			rect.top = static_cast<FLOAT>(i * FIXED_LINE_HEIGHT);
			rect.bottom = rect.top + FIXED_LINE_HEIGHT;
			mRenderer.FillRectangle(rect, mSelectionBrush);
		}
	}
}

void CodeWnd::RenderContent(const CRect& viewRect)
{
	LONG startRow = viewRect.top / FIXED_LINE_HEIGHT;
	assert(startRow >= 0);
	LONG endRow = viewRect.bottom / FIXED_LINE_HEIGHT;
	endRow = std::min<LONG>(endRow, mBuffer->GetNumLines() - 1);

	D2D1_RECT_F textRect;
	auto& lines = mBuffer->GetLines();
	auto font = mEditor->mCodeFont;

	for (LONG l = startRow; l <= endRow; ++l)
	{
		textRect.top = static_cast<FLOAT>(l * FIXED_LINE_HEIGHT);
		textRect.bottom = textRect.top + FIXED_LINE_HEIGHT;

		const auto& line = lines[l];
		const auto& tokens = line.GetTokens();

		int startCol = line.GetColumnFromPoint(viewRect.left);
		int endCol = line.GetColumnFromPoint(viewRect.right);

		for (auto& token : tokens)
		{
			int tokenStart = token.index;
			int tokenEnd = token.index + token.count - 1;
			if (tokenStart > endCol)
			{
				break;
			}

			tokenStart = std::max(tokenStart, startCol);
			tokenEnd = std::min(tokenEnd, endCol);
			textRect.left = static_cast<FLOAT>(line.GetExtentTo(tokenStart));
			textRect.right = static_cast<FLOAT>(line.GetExtentTo(tokenEnd + 1)) + 10;

			UINT32 len = line.GetFlattenedString(tokenStart, tokenEnd, gsLineBuf);

			switch (token.type)
			{
			case Token::TYPE_COMMENT:
				mRenderer.DrawText(gsLineBuf, len, font, textRect, mTextBrushComment);
				break;
			case Token::TYPE_SPECIAL:
				mRenderer.DrawText(gsLineBuf, len, font, textRect, mTextBrushSpecial);
				break;
			default:
				mRenderer.DrawText(gsLineBuf, len, font, textRect, mTextBrushNormal);
				break;
			}
		}
	}

	if (mShowCaret)
	{
		D2D1_RECT_F caretRect;
		caretRect.top = static_cast<FLOAT>(FIXED_LINE_HEIGHT * mCaretRow);
		caretRect.bottom = caretRect.top + FIXED_LINE_HEIGHT;
		caretRect.left = static_cast<FLOAT>(mBuffer->GetExtentTo(mCaretRow, mCaretCol));
		if (mComposing)
		{
			caretRect.right = caretRect.left + FIXED_WIDE_PITCH;
			mRenderer.FillRectangle(caretRect, mCompositionBrush);
		}
		else
		{
			caretRect.right = caretRect.left + 1;
			mRenderer.FillRectangle(caretRect, mCaretBrush);
		}
	}
}

BEGIN_MESSAGE_MAP(LineWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

LineWnd::LineWnd()
{
}

LineWnd::~LineWnd()
{
}

void LineWnd::CreateWnd(CWnd* parent, int x, int y, int w, int h)
{
	mEditor = static_cast<CodeEditor*>(parent);
	mBuffer = mEditor->GetCodeBuffer();

	int r = x + w;
	int b = y + h;
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
	Create(nullptr, nullptr, style, CRect(x, y, r, b), parent, 0);
}

int LineWnd::OnCreate(CREATESTRUCT* createStruct)
{
	CWnd::OnCreate(createStruct);

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL2, &mTextBrush);

	return 0;
}

void LineWnd::OnDestroy()
{
	mTextBrush->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void LineWnd::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);
	mRenderer.Resize(cx, cy);
}

BOOL LineWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void LineWnd::OnPaint()
{
	CPaintDC dc(this);

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);

	CRect viewRect;
	GetClientRect(&viewRect);

	int yOffset = mEditor->GetVertOffset();

	int srow = (viewRect.top + yOffset) / FIXED_LINE_HEIGHT;
	int erow = (viewRect.bottom + yOffset) / FIXED_LINE_HEIGHT;
	int endLine = mBuffer->GetNumLines() - 1;
	erow = std::min(erow, endLine);

	auto context = mRenderer.GetContext();

	D2D1_RECT_F rect;
	rect.left = 0;
	rect.right = static_cast<FLOAT>(viewRect.Width());
	rect.top = 1;
	rect.bottom = viewRect.Height() - 1.0f;
	context->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_ALIASED);

	D2D1_MATRIX_3X2_F xform;
	context->GetTransform(&xform);

	D2D1_MATRIX_3X2_F trans = xform;
	trans._32 = -(FLOAT)yOffset;
	context->SetTransform(trans);

	auto font = mEditor->mDecoFont;
	auto textAlignment = font->GetTextAlignment();
	font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	for (int i = srow; i <= erow; ++i)
	{
		rect.top = i * FIXED_LINE_HEIGHT + 1.0f;
		rect.bottom = rect.top + FIXED_LINE_HEIGHT;
		rect.right = viewRect.right - 3.0f;
		swprintf_s(gsLineBuf, L"%d", i + 1);
		mRenderer.DrawText(gsLineBuf, (UINT32)wcslen(gsLineBuf), font, rect, mTextBrush);
	}
	font->SetTextAlignment(textAlignment);

	context->SetTransform(xform);

	context->PopAxisAlignedClip();
	mRenderer.EndDraw();
	mRenderer.Display();
}

const int InfoGadget::FIXED_WIDTH = 110;

BEGIN_MESSAGE_MAP(InfoGadget, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

InfoGadget::InfoGadget()
{
}

InfoGadget::~InfoGadget()
{
}

void InfoGadget::CreateWnd(CWnd* parent, int x, int y)
{
	mEditor = static_cast<CodeEditor*>(parent);

	int r = x + FIXED_WIDTH;
	int b = y + ScrollBar::FIXED_SIZE;
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
	Create(nullptr, nullptr, style, CRect(x, y, r, b), parent, 0);
}

int InfoGadget::OnCreate(CREATESTRUCT* createStruct)
{
	CWnd::OnCreate(createStruct);

	mRenderer.Create(m_hWnd, createStruct->cx, createStruct->cy);
	mRenderer.SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	mRenderer.CreateSolidColorBrush(VisualManager::CLR_LEVEL2, &mBrush);

	return 0;
}

void InfoGadget::OnDestroy()
{
	mBrush->Release();
	mRenderer.Destroy();

	CWnd::OnDestroy();
}

void InfoGadget::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);
	mRenderer.Resize(cx, cy);
}

BOOL InfoGadget::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void InfoGadget::OnPaint()
{
	CPaintDC dc(this);

	mRenderer.BeginDraw();
	mRenderer.Clear(VisualManager::CLR_BASE);

	D2D1_RECT_F textRect;
	textRect.left = 0;
	textRect.top = 0;
	textRect.right = (FLOAT)FIXED_WIDTH;
	textRect.bottom = (FLOAT)ScrollBar::FIXED_SIZE;
	auto font = mEditor->mDecoFont;
	CString info;
	info.Format(L"L : %d   C : %d", mEditor->GetCursorRow() + 1, mEditor->GetCursorColumn() + 1);
	mRenderer.DrawText(info, info.GetLength(), font, textRect, mBrush);

	mRenderer.EndDraw();
	mRenderer.Display();
}

VirtualScroll::VirtualScroll()
{
	mRange = 0;
	mPosition = 0;
}

void VirtualScroll::SetRange(int range)
{
	mRange = range;
	if (mRange < 0)
	{
		mRange = 0;
	}

	if (mPosition > mRange)
	{
		mPosition = mRange;
	}
}

void VirtualScroll::SetScrollPosition(int pos)
{
	mPosition = pos;

	if (mPosition < 0)
	{
		mPosition = 0;
	}
	else if (mPosition > mRange)
	{
		mPosition = mRange;
	}
}

int VirtualScroll::GetScrollPosition() const
{
	return mPosition;
}

static const int LINE_NUMBER_AREA = 40;

BEGIN_MESSAGE_MAP(CodeEditor, CWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

CodeEditor::CodeEditor()
{
}

CodeEditor::~CodeEditor()
{
}

void CodeEditor::CreateWnd(CWnd* parent, int x, int y, int w, int h,
	bool vertScrollBar, bool horzScrollBar, bool lineNumber)
{
	DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	Create(nullptr, nullptr, style, CRect(x, y, x + w, y + h), parent, 0);

	if (vertScrollBar)
	{
		mVertScrollBar.CreateWnd(this, 0, 0, 8);
		mVertScrollBar.SetStepSize(FIXED_LINE_HEIGHT);
		mVertScrollBar.SetListener(static_cast<ScrollBar::IListener*>(this));
	}

	if (horzScrollBar)
	{
		mHorzScrollBar.CreateWnd(this, 0, 0, 8);
		mHorzScrollBar.SetStepSize(FIXED_UNIT_PITCH);
		mHorzScrollBar.SetListener(static_cast<ScrollBar::IListener*>(this));

		mInfoGadget.CreateWnd(this, 0, 0);
	}

	if (lineNumber)
	{
		mLineWnd.CreateWnd(this, 0, 0, 8, 8);
	}

	mCodeWnd.CreateWnd(this, 0, 0, 8, 8);

	LayoutChildren(w, h);
}

void CodeEditor::SetBorderColor(D2D_COLOR_F color)
{
	if (mBrush.GetSafeHandle())
	{
		mBrush.DeleteObject();
	}
	mBrush.CreateSolidBrush(VisualManager::ToGdiColor(color));
}

void CodeEditor::SetContent(const wstring& text)
{
	mCodeWnd.SetContent(text);
}

wstring CodeEditor::GetContent() const
{
	return std::move(mCodeWnd.GetContent());
}

void CodeEditor::CopyToClipboard()
{
	mCodeWnd.OnSelectAll();
	mCodeWnd.OnCopy();
}

void CodeEditor::EnableEditor()
{
	EnableWindow(TRUE);
	mCodeWnd.EnableWindow(TRUE);

	mCodeWnd.Invalidate(FALSE);
}

void CodeEditor::DisableEditor()
{
	mCodeWnd.EnableWindow(FALSE);
	EnableWindow(FALSE);

	mCodeWnd.Invalidate(FALSE);
}

void CodeEditor::DoWheelScroll(int delta)
{
	int pos;
	if (mVertScrollBar.GetSafeHwnd())
	{
		pos = mVertScrollBar.GetScrollPosition();
	}
	else
	{
		pos = mVertScroll.GetScrollPosition();
	}

	pos += delta;
	if (mVertScrollBar.GetSafeHwnd())
	{
		mVertScrollBar.SetScrollPosition(pos);
	}
	else
	{
		mVertScroll.SetScrollPosition(pos);
	}

	if (mLineWnd.GetSafeHwnd())
	{
		mLineWnd.Invalidate(FALSE);
	}
}

void CodeEditor::DoAutoScroll(int xOffset, int yOffset)
{
	if (mVertScrollBar.GetSafeHwnd())
	{
		mVertScrollBar.SetScrollPosition(yOffset);
	}
	else
	{
		mVertScroll.SetScrollPosition(yOffset);
	}

	if (mHorzScrollBar.GetSafeHwnd())
	{
		mHorzScrollBar.SetScrollPosition(xOffset);
	}
	else
	{
		mHorzScroll.SetScrollPosition(xOffset);
	}
}

void CodeEditor::AdjustScrollRange()
{
	int worldWidth = mCodeBuffer->GetWorldWidth() + FIXED_WIDE_PITCH;
	int worldHeight = mCodeBuffer->GetWorldHeight();

	CRect codeRect;
	mCodeWnd.GetWindowRect(&codeRect);
	int viewWidth = codeRect.Width();
	int viewHeight = codeRect.Height();

	int vertRange = worldHeight - viewHeight;
	int horzRange = worldWidth - viewWidth;

	if (mVertScrollBar.GetSafeHwnd())
	{
		mVertScrollBar.SetRange(0, vertRange);
		mVertScrollBar.SetPageSize(viewHeight);
	}
	else
	{
		mVertScroll.SetRange(vertRange);
	}

	if (mHorzScrollBar.GetSafeHwnd())
	{
		mHorzScrollBar.SetRange(0, horzRange);
		mHorzScrollBar.SetPageSize(viewWidth);
	}
	else
	{
		mHorzScroll.SetRange(horzRange);
	}
}

void CodeEditor::OnScrollPositionChanged(ScrollBar* scrollBar)
{
	mCodeWnd.Invalidate(FALSE);

	if (mLineWnd.GetSafeHwnd())
	{
		mLineWnd.Invalidate(FALSE);
	}
}

void CodeEditor::OnBufferGeometryChanged()
{
	AdjustScrollRange();
	OnCursorPositionChanged();
}

void CodeEditor::OnCursorPositionChanged()
{
	if (mLineWnd.GetSafeHwnd())
	{
		mLineWnd.Invalidate(FALSE);
	}
	if (mInfoGadget.GetSafeHwnd())
	{
		mInfoGadget.Invalidate(FALSE);
	}
}

int CodeEditor::OnCreate(CREATESTRUCT* createStruct)
{
	CWnd::OnCreate(createStruct);

	mCodeBuffer = make_unique<CodeBuffer>();

	mBrush.CreateSolidBrush(VisualManager::ToGdiColor(VisualManager::CLR_BASE));

	DWRITE_TRIMMING trimming;
	trimming.granularity = DWRITE_TRIMMING_GRANULARITY_NONE;
	trimming.delimiter = 0;
	trimming.delimiterCount = 0;

	DWrite::factory->CreateTextFormat(
		L"D2Coding", nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		14.0f, L"",
		&mCodeFont);
	mCodeFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	mCodeFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	mCodeFont->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	mCodeFont->SetTrimming(&trimming, nullptr);

	DWrite::factory->CreateTextFormat(
		L"Cambria", nullptr,
		DWRITE_FONT_WEIGHT_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		12.0f, L"", &mDecoFont);
	mDecoFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mDecoFont->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	mDecoFont->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
	mDecoFont->SetTrimming(&trimming, nullptr);

	return 0;
}

void CodeEditor::OnDestroy()
{
	mVertScrollBar.ResetListener();
	mHorzScrollBar.ResetListener();

	mCodeBuffer.reset();

	mDecoFont->Release();
	mCodeFont->Release();

	CWnd::OnDestroy();
}

void CodeEditor::OnSize(UINT type, int cx, int cy)
{
	CWnd::OnSize(type, cx, cy);

	if (mCodeWnd.GetSafeHwnd())
	{
		LayoutChildren(cx, cy);
	}
}

CodeBuffer* CodeEditor::GetCodeBuffer() const
{
	return mCodeBuffer.get();
}

int CodeEditor::GetVertOffset() const
{
	if (mVertScrollBar.GetSafeHwnd())
	{
		return mVertScrollBar.GetScrollPosition();
	}
	else
	{
		return mVertScroll.GetScrollPosition();
	}
}

int CodeEditor::GetHorzOffset() const
{
	if (mHorzScrollBar.GetSafeHwnd())
	{
		return mHorzScrollBar.GetScrollPosition();
	}
	else
	{
		return mHorzScroll.GetScrollPosition();
	}
}

int CodeEditor::GetCursorRow() const
{
	return mCodeWnd.GetCursorRow();
}

int CodeEditor::GetCursorColumn() const
{
	return mCodeWnd.GetCursorColumn();
}

void CodeEditor::LayoutChildren(int cx, int cy)
{
	CRect codeRect(1, 1, cx - 1, cy - 1);
	int x, y, w, h;

	HDWP hdwp = BeginDeferWindowPos(5);
	UINT dwpFlags = SWP_NOZORDER | SWP_NOACTIVATE;

	if (mLineWnd.GetSafeHwnd())
	{
		x = 0;
		y = 0;
		w = LINE_NUMBER_AREA;
		h = cy;

		if (mHorzScrollBar.GetSafeHwnd())
		{
			h -= ScrollBar::FIXED_SIZE;
		}
		DeferWindowPos(hdwp, mLineWnd, nullptr, x, y, w, h, dwpFlags);

		codeRect.left += w;
	}

	if (mVertScrollBar.GetSafeHwnd())
	{
		x = cx - ScrollBar::FIXED_SIZE;
		y = 0;
		w = ScrollBar::FIXED_SIZE;
		h = cy;
		if (mHorzScrollBar.GetSafeHwnd())
		{
			h -= ScrollBar::FIXED_SIZE;
		}
		DeferWindowPos(hdwp, mVertScrollBar, nullptr, x, y, w, h, dwpFlags);

		codeRect.right -= w;
	}

	if (mHorzScrollBar.GetSafeHwnd())
	{
		x = 0;
		y = cy - ScrollBar::FIXED_SIZE;
		w = cx - InfoGadget::FIXED_WIDTH;
		h = ScrollBar::FIXED_SIZE;
		DeferWindowPos(hdwp, mHorzScrollBar, nullptr, x, y, w, h, dwpFlags);

		dwpFlags |= SWP_NOSIZE;
		x = cx - InfoGadget::FIXED_WIDTH;
		DeferWindowPos(hdwp, mInfoGadget, nullptr, x, y, 0, 0, dwpFlags);

		codeRect.bottom -= h;
	}

	x = codeRect.left;
	y = codeRect.top;
	w = codeRect.Width();
	h = codeRect.Height();
	DeferWindowPos(hdwp, mCodeWnd, HWND_TOP, x, y, w, h, SWP_NOACTIVATE);

	EndDeferWindowPos(hdwp);

	AdjustScrollRange();
}

void CodeEditor::OnSetFocus(CWnd* oldWnd)
{
	mCodeWnd.SetFocus();
}

BOOL CodeEditor::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CodeEditor::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	dc.FillRect(&rect, &mBrush);
}

} // namespace hsui
