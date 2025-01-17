#pragma once
#include <string>

namespace converter {

	/** Convert a \c std::wstring to \c std::string */
	std::string to_string(const std::wstring& wstr);

}
