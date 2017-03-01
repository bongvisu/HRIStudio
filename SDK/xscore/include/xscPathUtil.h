#pragma once

#include <xscModule.h>

namespace xsc {

class XSC_API PathUtil
{
public:
	static bool Canonicalize(const std::wstring& inputPath, std::wstring& outputPath);
	
	static bool RelativePath(
		const std::wstring& baseDir, const std::wstring& absolutePath, std::wstring& relativePath);

	static void SplitToDirectoryAndName(
		const std::wstring& path, std::wstring& dir, std::wstring& name, bool includeExt = false);

	static void SplitPath(
		const std::wstring& path, std::wstring& drive, std::wstring& dir, std::wstring& name, std::wstring& ext);
};

} // namespace xsc
