
#pragma once

#ifndef __ETERNAL_TIMESTAMP_HPP__
#define __ETERNAL_TIMESTAMP_HPP__

#include "eternal_timestamp.h"
#include <time.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif


namespace eternal_timestamp 
{
	class EternalTimestamp
	{
		static eternal_timestamp_t now();

		static eternal_timestamp_t today();
		static eternal_timestamp_t today_at(int hour = 0, int minute = 0, int second = 0);

		static constexpr inline eternal_timestamp_t epoch();

		// produce a timestamp that has all fields set to 'not specified':
		static constexpr inline eternal_timestamp_t unknown();

		static bool is_valid(const eternal_timestamp_t t);
		static int validate(const eternal_timestamp_t t);
		static bool is_partial_timestamp(const eternal_timestamp_t t);
		// convert a partial timestamp by rebasing it against the given base timestamp (using epoch() as default)
		static eternal_timestamp_t normalize(const eternal_timestamp_t t, const eternal_timestamp_t base = {0});

		static bool has_century(const eternal_timestamp_t t);
		static bool has_year(const eternal_timestamp_t t);
		static bool has_century_and_year(const eternal_timestamp_t t);
		// and probably more appropriate for *prehistoric* dates to ask: do we know the year to *any* precision at all?
		static bool has_age(const eternal_timestamp_t t);
		static bool has_month(const eternal_timestamp_t t);
		static bool has_day(const eternal_timestamp_t t);
		static bool has_hour(const eternal_timestamp_t t);
		static bool has_minute(const eternal_timestamp_t t);
		static bool has_seconds(const eternal_timestamp_t t);
		static bool has_milliseconds(const eternal_timestamp_t t);
		static bool has_microseconds(const eternal_timestamp_t t);
		static bool has_complete_date(const eternal_timestamp_t t);
		static bool has_time(const eternal_timestamp_t t);
		static bool has_hh_mm_ss(const eternal_timestamp_t t);

		static bool is_modern_format(const eternal_timestamp_t t);
		static bool is_prehistoric_format(const eternal_timestamp_t t);

		static int64_t calc_time_fast_delta(const eternal_timestamp_t t1, const eternal_timestamp_t t2);

		// return a improved attempt at producing the number of days between two values as a floating point
		// value.
		//
		// WARNING NOTE: we deliver an *approximate* time distance value, which will satisfy (almost all) LT/LE/EQ/GE/GT
		// checks as would `calc_time_fast_delta()`, but here we make an effort to approximate the distance in
		// time a little more accurately. However, we DO NOT go down and stickler (no typo!) to time and calendar
		// peculiarities here; for this rough estimate we're quite satisfied to reckon with these brutish time rules:
		// - a year has 12 months (so no Smart alec-ing with calendar adjustments throughout galactic history anywhere!)
		// - a month has 31 days (yes, that's an over-estimate about 50% of the time, but we don't want to burden ourselves
		//   with any lap year calculus, nor with any 30-vs-31 logic/lookup tables. After all, we're "more precise" than
		//   bankers and acturarians when they calculate with 400-day years for (some of) their financial modeling. ;-)
		static double calc_time_approx_delta(const eternal_timestamp_t t1, const eternal_timestamp_t t2);

		static int cvt_to_timeinfo_struct(eternal_time_tm &dst, const eternal_timestamp_t t);
		static int cvt_to_time_t(time_t &dst, const eternal_timestamp_t t);

#if defined(_WIN32) || defined(_WIN64)
		static int cvt_to_Win32FileTime(FILETIME &dst, const eternal_timestamp_t t);
#endif

		// *fast* conversion to a IEEE 754 floating point format we can use to store timestamps in databases and elsewhere, 
		// while we keep the characteristics of the `eternal_timestamp` alue intact as much as possible.
		static int cvt_to_etdb_real(double &dst, const eternal_timestamp_t t);

		// meanwhile, you MAY want to use this *slower* conversion to IEEE 754 floating point value, representing the
		// time since the Proleptic Georgian Calender Epoch, i.e. the 'zero' as used, f.e. in SQLite.
		// See also our design document and [SQLite's `REAL`-based timestamps, encoding "*Julian day numbers, the number of days since noon in Greenwich on November 24, 4714 B.C. according to the proleptic Gregorian calendar*"](https://www.sqlite.org/datatype3.html#date_and_time_datatype).
		static int cvt_to_proleptic_real(double &dst, const eternal_timestamp_t t);

		static int cvt_from_timeinfo_struct(eternal_timestamp_t &dst, const eternal_time_tm &t);
		static int cvt_from_time_t(eternal_timestamp_t &dst, const time_t t);
		static int cvt_from_tm(eternal_timestamp_t &dst, const struct tm &t);

#if defined(_WIN32) || defined(_WIN64)
		static int cvt_from_Win32FileTime(eternal_timestamp_t &dst, const FILETIME &t);
#endif

		// NOTE: our 'epoch' for the IEEEE 754 double type representation is 2000/jan/01@00:00:00.0000 UTC;
		// this way we expect most dates/times to have small values and carry highest risdual precision when
		// you perform calculations with/on these.
		static int cvt_from_etdb_real(eternal_timestamp_t &dst, const double t);

		static int cvt_from_proleptic_real(eternal_timestamp_t &dst, const double t);

		// and a helper method in case you need to process an epoch'ed IEEE 754 double value for your timestamp.
		static inline constexpr double cvt_epoch_to_etdb_real_offset(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute, const unsigned int second, const unsigned int subsecond);
	};
}

#endif
