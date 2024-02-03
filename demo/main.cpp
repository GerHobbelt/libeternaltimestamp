
#include <eternal_timestamp/eternal_timestamp.h>

#if defined(_WIN32)
#include <crtdbg.h>
#endif
#include <algorithm>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string_view>

#include "monolithic_examples.h"


using namespace eternal_timestamp;


// The type T can be formatted using the format specification of the wrapped value
template<class CharT>
struct std::formatter<eternal_timestamp_t, CharT>
{
	bool quoted = false;

	template<class ParseContext>
	constexpr ParseContext::iterator parse(ParseContext& ctx)
	{
		auto it = ctx.begin();
		if (it == ctx.end())
			return it;

		if (*it == '#')
		{
			quoted = true;
			++it;
		}
		if (*it != '}')
			throw std::format_error("Invalid format args for QuotableString.");

		return it;
	}

	template<class FmtContext>
	FmtContext::iterator format(eternal_timestamp_t t, FmtContext& ctx) const
	{
		std::ostringstream rv;
		eternal_time_tm tm;
		if (!EternalTimestamp::cvt_to_timeinfo_struct(tm, t)) {
			rv << (EternalTimestamp::is_modern_format(t) ? "(Modern)" : "(PreHistoric)") << "" << tm.year << "." << tm.month << "." << tm.day << "(T)" << std::format("{:02}", tm.hour) << ":" << std::format("{:02}", tm.minute) << ":" << std::format("{:02}", tm.seconds) << "." << std::format("{:03}", tm.milliseconds) << "." << std::format("{:03}", tm.microseconds);
		}
		else {
			rv << "(Invalid EternalTimestamp; Cannot Parse " << std::format("{:#016x}", t.t) << ")";
		}
		std::ostringstream out;
		if (quoted)
			out << std::quoted(rv.str());
		else
			out << rv.str();

		return std::ranges::copy(std::move(out).str(), ctx.out()).out;
	}
};



#if defined(BUILD_MONOLITHIC)
#define main(cnt, arr)      eternalty_demo_main(cnt, arr)
#endif

int main(int argc, const char **argv) 
{
	eternal_timestamp_t t = EternalTimestamp::now();
	std::cout << "now: " << std::format("{:#}", t) << '\n';
	return 0;
}
