
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

	{
		FILE *f = fopen("test.file", "w");
		fprintf(f, "heelo world\n");
		fclose(f);
	}

#ifdef _WIN32
	{
		auto h = CreateFileA("test.file", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		FILETIME at, mt, ct;
		GetFileTime(h, &ct, &at, &mt);

		EternalTimestamp::cvt_from_Win32FileTime(t, mt);
		std::cout << "file modify time (now): " << std::format("{:#}", t) << '\n';

		struct tm ti{0};
		ti.tm_year = 1980;
		ti.tm_mon = 5;
		ti.tm_mday = 21;
		ti.tm_hour = 9;
		ti.tm_sec = 35;
		ti.tm_isdst = 0;
		time_t tt = mktime(&ti);

		EternalTimestamp::cvt_from_time_t(t, tt);
		std::cout << "time_t @ 1980/5/21 @ 09:00:35: " << std::format("{:#}", t) << '\n';

		FILETIME ft;
		ULARGE_INTEGER time_value;
		time_value.QuadPart = (tt * 10000000LL) + 116444736000000000LL;
		ft.dwLowDateTime = time_value.LowPart;
		ft.dwHighDateTime = time_value.HighPart;

		EternalTimestamp::cvt_from_Win32FileTime(t, ft);
		std::cout << "ditto FILETIME: " << std::format("{:#}", t) << '\n';

		SetFileTime(h, &ft, &ft, &ft);
		CloseHandle(h);
	}
#else
	{
		struct stat st;
		struct utimbuf ubuf;
		int result;

		result = stat("test.file", &st);

		ubuf.actime  = st.st_atime;
		ubuf.modtime = st.st_mtime;

		result = utime("test.file", &ubuf);
	}
#endif

	{
		struct stat st;
		int result;

		result = stat("test.file", &st);

		EternalTimestamp::cvt_from_time_t(t, st.st_mtime);
		std::cout << "modified file timestamp: " << std::format("{:#}", t) << '\n';
	}

	return 0;
}
