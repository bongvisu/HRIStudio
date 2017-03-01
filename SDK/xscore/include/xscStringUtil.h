#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API StringUtil
{
public:
	static void Trim(std::wstring& line);

	typedef std::vector<std::wstring> StringList;
	static void ToLines(const std::wstring& text, StringList& lines, bool includeEmptyLine = false);

	static void Split(const std::wstring& text, const std::wstring& pattern, StringList& lines);
	
	static void ToLower(std::wstring& text);
	static void ToUpper(std::wstring& text);
	
	static void ReplaceLinebreakTo(const std::wstring& replace, std::wstring& text);

	static void ToMultiByte(const std::wstring& source, std::string& converted, bool utf8 = true);
	static void ToWide(const std::string& source, std::wstring& converted);
};

} // namespace xsc
