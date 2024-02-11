
#ifndef __ETERNAL_TIMESTAMP_HPP__
#include "eternal_timestamp/eternal_timestamp.h"
#endif

#include <chrono>
#include <ctime>

#ifndef NDEBUG
#include <stdio.h>
#include <stdlib.h>

#define ETS_ASSERT(t)			((t) ? (void)0 : (void)fprintf(stderr, "ETTM assertion %s failed at %s:%d\n", #t, __FILE__, __LINE__))
#else
#define ETS_ASSERT(t)			(void)0
#endif


using namespace eternal_timestamp;


enum fieldsize : unsigned int
{
	ETMT_FIELDSIZE_CENTURY = 9,
	ETMT_FIELDSIZE_YEAR = 7,
	ETMT_FIELDSIZE_MONTH = 4,
	ETMT_FIELDSIZE_DAY = 5,
	ETMT_FIELDSIZE_HOUR = 5,
	ETMT_FIELDSIZE_MINUTE = 6,
	ETMT_FIELDSIZE_SECONDS = 6,
	ETMT_FIELDSIZE_MILLISECONDS = 10,
	ETMT_FIELDSIZE_MICROSECONDS = 10,

	ETPHT_FIELDSIZE_YEARS = 38,
	ETPHT_FIELDSIZE_MONTH = 4,
	ETPHT_FIELDSIZE_DAY = 5,
	ETPHT_FIELDSIZE_HOUR = 5,
	ETPHT_FIELDSIZE_MINUTE = 6,
	ETPHT_FIELDSIZE_PRECISION = 4,
};

constexpr inline unsigned int get_MaxInvalid(unsigned int field_size_in_bits)
{
	const unsigned int invalid = (1 << field_size_in_bits) - 1;
	return invalid;
}

constexpr inline unsigned int get_MinInvalid(unsigned int field_size_in_bits)
{
	const unsigned int invalid = 0;
	return invalid;
}


#if ETS_UNSPECIFIED_MARKER_SORTS_BEFORE_1ST_VALUE

constexpr inline unsigned int get_Invalid(unsigned int field_size_in_bits)
{
	return get_MinInvalid(field_size_in_bits);
}

// clip value to legal range or signal as 'invalid':
constexpr inline unsigned int clip_Invalid(int v, unsigned int field_size_in_bits, int value_range)
{
	const unsigned int invalid = get_Invalid(field_size_in_bits);
	v++;
	// clip
	if (v <= 0)
		return invalid;
	if (v > value_range)
		return invalid;
	return v;
}

static constexpr int FIELD_VAL_OFFSET = 1;

#else

constexpr inline unsigned int get_Invalid(unsigned int field_size_in_bits)
{
	return get_MaxInvalid(field_size_in_bits);
}

// clip value to legal range or signal as 'invalid':
constexpr inline unsigned int clip_Invalid(int v, unsigned int field_size_in_bits, int value_range)
{
	const unsigned int invalid = get_MaxInvalid(field_size_in_bits);
	// clip
	if (v < 0)
		return invalid;
	if (v >= value_range)
		return invalid;
	return v;
}

static constexpr int FIELD_VAL_OFFSET = 0;

#endif // ETS_UNSPECIFIED_MARKER_SORTS_BEFORE_1ST_VALUE


static constexpr const int MODERN_EPOCH = 10000;    // 10000 B.C.
static constexpr const int PREHISTORIC_EPOCH = 0;   // 0 A.D.


eternal_timestamp_t EternalTimestamp::now()
{
#if defined(_WIN32)
	SYSTEMTIME st;
	FILETIME ft;
	eternal_modern_timestamp t{0};

	GetSystemTime(&st);
//	for (int i = 0; i < 20000000; i++) {
	GetSystemTimePreciseAsFileTime(&ft);  // Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC).

	int y = st.wYear;
	y += MODERN_EPOCH;
	t.century = y / 100;
	t.year = FIELD_VAL_OFFSET + y % 100;
	t.month = FIELD_VAL_OFFSET - 1 + st.wMonth;
	t.day = FIELD_VAL_OFFSET - 1 + st.wDay;
	t.hour = FIELD_VAL_OFFSET + st.wHour;
	t.minute = FIELD_VAL_OFFSET + st.wMinute;
	t.seconds = FIELD_VAL_OFFSET + st.wSecond;
	t.milliseconds = FIELD_VAL_OFFSET + st.wMilliseconds;

	uint64_t tt = ft.dwHighDateTime;
	tt <<= 32;
	tt += ft.dwLowDateTime;

	// little extra feature: we never produce the same timestamp for 'now' by artificially "bumping" it a microsecond or more if needed.
	static uint64_t last_real_tt{0};
	static uint64_t last_tt{0};
	uint32_t offset = 0;
	if (tt == last_real_tt) {
		tt = ++last_tt;
		offset = tt - last_real_tt;
		ft.dwLowDateTime += offset;
	} else {
		//fprintf(stderr, "tt: %llu vs %llu\n", (long long int)last_real_tt, (long long int)tt);
		last_real_tt = tt;
		last_tt = tt;
	}

	const uint64_t FREQ = (static_cast<uint64_t>(1e9) / 100);
	int32_t ss = tt % FREQ;
	int32_t ms = ss / 10000;
	int32_t us = ss % 10000;
	us /= 10;

	ETS_ASSERT(us >= 0);
	ETS_ASSERT(us < 1000);
	ETS_ASSERT(ms >= 0);
	ETS_ASSERT(ms < 1000);
	t.microseconds = FIELD_VAL_OFFSET + us;
	t.milliseconds = FIELD_VAL_OFFSET + ms;

	if (FileTimeToSystemTime(&ft, &st)) {
		y = st.wYear;
		y += MODERN_EPOCH;
		t.century = y / 100;
		t.year = FIELD_VAL_OFFSET + y % 100;
		t.month = FIELD_VAL_OFFSET - 1 + st.wMonth;
		t.day = FIELD_VAL_OFFSET - 1 + st.wDay;
		t.hour = FIELD_VAL_OFFSET + st.wHour;
		t.minute = FIELD_VAL_OFFSET + st.wMinute;
		t.seconds = FIELD_VAL_OFFSET + st.wSecond;
		t.milliseconds = FIELD_VAL_OFFSET + st.wMilliseconds;
	}
	else {
		ETS_ASSERT(!"Should not get here!");
	}
//	}

	// t value is now a positive offset value from 10000 BC.
	eternal_timestamp_t rv;
	rv.modern = t;
	return rv;
#else
	return {0};
#endif
}

eternal_timestamp_t EternalTimestamp::today()
{
#if defined(_WIN32)
	SYSTEMTIME st;
	eternal_modern_timestamp t{0};

	GetSystemTime(&st);

	int y = st.wYear;
	y += MODERN_EPOCH;
	t.century = y / 100;
	t.year = FIELD_VAL_OFFSET + y % 100;
	t.month = FIELD_VAL_OFFSET - 1 + st.wMonth;
	t.day = FIELD_VAL_OFFSET - 1 + st.wDay;

	// t value is now a positive offset value from 10000 BC.
	eternal_timestamp_t rv;
	rv.modern = t;
	return rv;
#else
	return {0};
#endif
}

eternal_timestamp_t EternalTimestamp::today_at(int hour, int minute, int second)
{
	eternal_timestamp_t t = today();

	ETS_ASSERT(is_modern_format(t));
	ETS_ASSERT(!is_partial_timestamp(t));

	t.modern.hour = clip_Invalid(hour, ETMT_FIELDSIZE_HOUR, 24);
	t.modern.minute = clip_Invalid(minute, ETMT_FIELDSIZE_MINUTE, 60);
	t.modern.seconds = clip_Invalid(second, ETMT_FIELDSIZE_SECONDS, 60);
	t.modern.milliseconds = 0;
	t.modern.microseconds = 0;

	return t;
}

// produce a timestamp that has all fields set to 'not specified':
constexpr inline eternal_timestamp_t EternalTimestamp::unknown()
{
#if ETS_UNSPECIFIED_MARKER_SORTS_BEFORE_1ST_VALUE
	return {0};
#else
	return {0x3FFFFFFFFFFFFFFFULL};
#endif
}

bool EternalTimestamp::is_valid(const eternal_timestamp_t t)
{
	return 0;
}

int EternalTimestamp::validate(const eternal_timestamp_t t)
{
	return 0;
}

int EternalTimestamp::validate(const struct eternal_time_tm &ts)
{
	return 0;
}

bool EternalTimestamp::is_partial_timestamp(const eternal_timestamp_t t)
{
	return 0;
}

// convert a partial timestamp by rebasing it against the given base timestamp
eternal_timestamp_t EternalTimestamp::normalize(const eternal_timestamp_t t, const eternal_timestamp_t base)
{
	return {0};
}


bool EternalTimestamp::has_century(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.century != get_Invalid(ETMT_FIELDSIZE_CENTURY);
	}
	else
	{
		// Okay, this is a bit arbitrary, but we define 'has_century' here as 'known to within a century precise',
		// hence this is equivalent to a 'precison' = 2, i.e. a precision of 10^2 == 100.
		// Otherwise we'ld be talking about *milennia* or *aeons*.
		return t.prehistoric.years != get_Invalid(ETPHT_FIELDSIZE_YEARS) 
			&& t.prehistoric.precision < 3;   // we need precision=2 (i.e. 10^2 ==> 100) or *better* as precision indicator or we won't know the year within the millenium.
	}
}

bool EternalTimestamp::has_year(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.year != get_Invalid(ETMT_FIELDSIZE_YEAR);
	}
	else
	{
		return t.prehistoric.years != get_Invalid(ETPHT_FIELDSIZE_YEARS) 
			&& t.prehistoric.precision < 2;   // we need precision=0 (i.e. 10^0 ==> 1) or precision=1 (i.e. 10^1==>10) as precision indicator or we won't know the year within the century.
	}
}

bool EternalTimestamp::has_century_and_year(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return has_century(t) && has_year(t);
	}
	else
	{
		return t.prehistoric.years != get_Invalid(ETPHT_FIELDSIZE_YEARS)
			&& t.prehistoric.precision < 2;   // we need precision=0 (i.e. 10^0 ==> 1) or precision=1 (i.e. 10^1==>10) as precision indicator or we won't know the year within the century.
	}
}

// and probably more appropriate for *prehistoric* dates to ask: do we know the year to *any* precision at all?
bool EternalTimestamp::has_age(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return has_century(t) && has_year(t);
	}
	else
	{
		return t.prehistoric.years != get_Invalid(ETPHT_FIELDSIZE_YEARS);
	}
}

bool EternalTimestamp::has_month(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.month != get_Invalid(ETMT_FIELDSIZE_MONTH);
	}
	else
	{
		return t.prehistoric.month != get_Invalid(ETPHT_FIELDSIZE_MONTH);
	}
}

bool EternalTimestamp::has_day(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.day != get_Invalid(ETMT_FIELDSIZE_DAY);
	}
	else
	{
		return t.prehistoric.day != get_Invalid(ETPHT_FIELDSIZE_DAY);
	}
}

bool EternalTimestamp::has_hour(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.hour != get_Invalid(ETMT_FIELDSIZE_HOUR);
	}
	else
	{
		return t.prehistoric.hour != get_Invalid(ETPHT_FIELDSIZE_HOUR);
	}
}

bool EternalTimestamp::has_minute(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.minute != get_Invalid(ETMT_FIELDSIZE_MINUTE);
	}
	else
	{
		return t.prehistoric.minute != get_Invalid(ETPHT_FIELDSIZE_MINUTE);
	}
}

bool EternalTimestamp::has_seconds(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.seconds != get_Invalid(ETMT_FIELDSIZE_SECONDS);
	}
	else
	{
		return false;
	}
}

bool EternalTimestamp::has_milliseconds(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.milliseconds != get_Invalid(10);
	}
	else
	{
		return false;
	}
}

bool EternalTimestamp::has_microseconds(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return t.modern.microseconds != get_Invalid(10);
	}
	else
	{
		return false;
	}
}

bool EternalTimestamp::has_complete_date(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return has_century(t) && has_year(t) && has_month(t) && has_day(t);
	}
	else
	{
		// Well... in *prehistory* we would accept *anything* really, 
		// but THIS is a *technical* question: do we have all the date elements
		// so we can use them without fear of using invalid values for *any* part?
		//
		// As such, this question is more geared towards MODERN tinmestamps,
		// where we may expect the date to be known to the day precise...
		return t.prehistoric.years != get_Invalid(ETPHT_FIELDSIZE_YEARS)
			&& t.prehistoric.precision == 0
			&& has_month(t) && has_day(t);
	}
}

bool EternalTimestamp::has_time(const eternal_timestamp_t t)
{
	// Again, this is another one of those *technical* questions: do we have ALL time fields
	// so we don't have to fear one or more of them has an invalid value or 'unspecified' marker
	// that may throw a spanner in the works when we start doing our processing/calculations
	// on this timestamp?
	if (is_modern_format(t))
	{
		return has_hour(t) && has_minute(t) && has_seconds(t) && has_milliseconds(t) && has_microseconds(t);
	}
	else
	{
		return has_hour(t) && has_minute(t);
	}
}

bool EternalTimestamp::has_hh_mm_ss(const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		return has_hour(t) && has_minute(t) && has_seconds(t);
	}
	else
	{
		// We accept that prehistoric timestamps don't come with *seconds*...
		return has_hour(t) && has_minute(t);
	}
}

bool EternalTimestamp::is_modern_format(const eternal_timestamp_t t)
{
	return !t.modern.mode;
}

bool EternalTimestamp::is_prehistoric_format(const eternal_timestamp_t t)
{
	return !!t.modern.mode;
}


// NOTE: this performs a FAST time diff calculation, which will satisfy any LT/LE/EQ/GE/GT check on the calculated delta
// but DOES NOT produce a time-accurate *distance* per se. You should use Proleptic REAL calcualus for that on, or if that
// doesn't suit your needs, apply different rules to the conversion of these timestamps to 'time since' values that you want.
//
// For our purposes, the (complex!) date/time conversions involved are deemed overkill and thus we stick to a very fast and
// very basic delta calculus.
//
// Returns equivalent of (B - A)
int64_t EternalTimestamp::calc_time_fast_delta(const eternal_timestamp_t t1, const eternal_timestamp_t t2)
{
	// Notes:
	// - t.modern.mode is at the same bit location as t.prehistoric.mode
	// - we can run a simple integer comparison when both timestamps are of the same 'mode' , i.e. the same subformat.
	//   + NOT REALLY: modern subformat increases into the future while prehistoric subformat increases into history,
	//     hence we MUST handle this subtly different for both formats
	// - check the most common scenario (modern vs. modern) first.
	if (is_modern_format(t1)) {
		if (is_modern_format(t2)) {
			// Here's why we did not bother to use bit63 in our format spec: now we have no problem turning uint64_t into int64_t 
			// and compare the two!
			int64_t a = static_cast<int64_t>(t1.t);
			int64_t b = static_cast<int64_t>(t2.t);
			return b - a;
		} else {
			// t1 is increasing towards the future; t2 is increasing towards history.
			//
			// check if they're somewhat normalized, i.e. whether the prehistoric one is pointing at an earlier century/year than modern t2:
			int64_t sa = t1.modern.century * 100 + t1.modern.year + MODERN_EPOCH;
			int64_t sb = t2.prehistoric.years + PREHISTORIC_EPOCH;
			int64_t d = sb - sa;
			if (d) {
				return d;
			}

			// Whoa! Non-Normalized prehistoric timestamp 't2' as it shares the same year with modern 't1'.
			// hence we can 'normalize' t2 to a modern timestamp.
			eternal_timestamp_t tn{0};
			int y = t2.prehistoric.years - MODERN_EPOCH;
			tn.modern.century = y / 100;
			tn.modern.year = y % 100;
			tn.modern.month = t2.prehistoric.month;
			tn.modern.day = t2.prehistoric.day;
			tn.modern.hour = t2.prehistoric.hour;
			tn.modern.minute = t2.prehistoric.minute;

			int64_t a = static_cast<int64_t>(t1.t);
			int64_t b = static_cast<int64_t>(tn.t);

			return b - a;
		}
	}
	else {
		if (is_prehistoric_format(t2)) {
			// Here's why we did not bother to use bit63 in our format spec: now we have no problem turning uint64_t into int64_t 
			// and compare the two!
			int64_t a = static_cast<int64_t>(t1.t);
			int64_t b = static_cast<int64_t>(t2.t);
			return a - b;  // to keep the comparison result in line with the modern vs. modern comparison, we MUST reverse the delta here.
		} else {
			// t2 is increasing towards the future; t1 is increasing towards history.
			//
			// check if they're somewhat normalized, i.e. whether the prehistoric one is pointing at an earlier century/year than modern t2:
			int64_t sb = t2.modern.century * 100 + t2.modern.year + MODERN_EPOCH;
			int64_t sa = t1.prehistoric.years + PREHISTORIC_EPOCH;
			int64_t d = sb - sa;
			if (d) {
				return d;
			}

			// Whoa! Non-Normalized prehistoric timestamp 't1' as it shares the same year with modern 't2'.
			// hence we can 'normalize' t1 to a modern timestamp.
			eternal_timestamp_t tn{0};
			int y = t1.prehistoric.years - MODERN_EPOCH;
			tn.modern.century = y / 100;
			tn.modern.year = y % 100;
			tn.modern.month = t1.prehistoric.month;
			tn.modern.day = t1.prehistoric.day;
			tn.modern.hour = t1.prehistoric.hour;
			tn.modern.minute = t1.prehistoric.minute;

			int64_t a = static_cast<int64_t>(tn.t);
			int64_t b = static_cast<int64_t>(t2.t);

			return b - a;
		}
	}
}

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
//   bankers and actuarians when they calculate with 400-day years for (some of) their financial modeling. ;-)
double EternalTimestamp::calc_time_approx_delta(const eternal_timestamp_t t1, const eternal_timestamp_t t2)
{
	struct eternal_time_tm d1, d2;
	cvt_to_timeinfo_struct(d1, t1);
	cvt_to_timeinfo_struct(d2, t2);
	double v1, v2;
	double m1, m2;

	v1 = d1.day + 31 * d1.month + 12 * 31 * d1.large_year;
	m1 = d1.milliseconds + 1000 * d1.milliseconds + 1000 * 1000 * d1.seconds
		+ 1E6 * 60 * d1.minute + 1E6 * 60 * 60 * d1.hour;
	v1 += m1 / (1E6 * 60 * 60 * 24);

	v2 = d2.day + 31 * d2.month + 12 * 31 * d2.large_year;
	m2 = d2.milliseconds + 1000 * d2.milliseconds + 1000 * 1000 * d2.seconds
		+ 1E6 * 60 * d2.minute + 1E6 * 60 * 60 * d2.hour;
	v2 += m2 / (1E6 * 60 * 60 * 24);

	return v1 - v2;
}


int EternalTimestamp::cvt_to_timeinfo_struct(struct eternal_time_tm &dst, const eternal_timestamp_t t)
{
	if (is_modern_format(t))
	{
		auto ts = t.modern;
		dst.unspecified = 0;
		int y = 0;
		if (has_century(t))
			y = ts.century * 100;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_EPOCHS;
		if (has_year(t))
			y += ts.year - FIELD_VAL_OFFSET;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_YEARS;
		ETS_ASSERT(y - MODERN_EPOCH > INT_MIN);
		ETS_ASSERT(y - MODERN_EPOCH < INT_MAX);
		dst.large_year = dst.year = y - MODERN_EPOCH;

		int v = 0;
		if (has_month(t))
			v = ts.month;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_MONTHS;
		dst.month = v;

		v = 0;
		if (has_day(t))
			v = ts.day;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_DAYS;
		dst.day = v;

		v = 0;
		if (has_hour(t))
			v = ts.hour - FIELD_VAL_OFFSET;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_HOURS;
		dst.hour = v;

		v = 0;
		if (has_minute(t))
			v = ts.minute - FIELD_VAL_OFFSET;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_MINUTES;
		dst.minute = v;

		v = 0;
		if (has_seconds(t))
			v = ts.seconds;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_SECONDS;
		dst.seconds = v;

		v = 0;
		if (has_milliseconds(t))
			v = ts.milliseconds - FIELD_VAL_OFFSET;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_MILLISECONDS;
		dst.milliseconds = v;

		v = 0;
		if (has_microseconds(t))
			v = ts.microseconds;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_MICROSECONDS;
		dst.microseconds = v;

		return validate(dst);
	}
	else
	{
		auto ts = t.prehistoric;
		dst.unspecified = 0;
		int64_t y = 0;
		if (has_age(t))
			y = -1 * ts.years;   // the years aare to be negative to signal they're dates B.C.
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_EPOCHS;
		int prec = ts.precision;
		if (ts.precision < 2) {   // we need precision=0 (i.e. 10^0 ==> 1) or precision=1 (i.e. 10^1==>10) as precision indicator or we won't know the year within the century.
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_YEARS;
		}
		dst.large_year = y;
		dst.year = 0;

		int v = 0;
		if (has_month(t))
			v = ts.month;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_MONTHS;
		dst.month = v;

		v = 0;
		if (has_day(t))
			v = ts.day;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_DAYS;
		dst.day = v;

		v = 0;
		if (has_hour(t))
			v = ts.hour - FIELD_VAL_OFFSET;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_HOURS;
		dst.hour = v;

		v = 0;
		if (has_minute(t))
			v = ts.minute - FIELD_VAL_OFFSET;
		else
			dst.unspecified |= 1 << ETTS_UNSPECIFIED_MINUTES;
		dst.minute = v;

		dst.unspecified |= 1 << ETTS_UNSPECIFIED_SECONDS;
		dst.seconds = 0;

		dst.unspecified |= 1 << ETTS_UNSPECIFIED_MILLISECONDS;
		dst.milliseconds = 0;

		dst.unspecified |= 1 << ETTS_UNSPECIFIED_MICROSECONDS;
		dst.microseconds = 0;

		return validate(dst);
	}
}

int EternalTimestamp::cvt_to_time_t(time_t &dst, const eternal_timestamp_t t)
{
	return 0;
}


#if defined(_WIN32) || defined(_WIN64)
int EternalTimestamp::cvt_to_Win32FileTime(FILETIME &dst, const eternal_timestamp_t t)
{
	return 0;
}

#endif

// NOTE: in order to encode the 'undefined' fields into the REAL while reckoning with 
// floating point's inherent *precision loss* when calculating with disparate numbers,
// we encode each 'unspecified' field as THE MIDDLE between TWO 'out-of-range' values:
// where we use a *single* 'unspecified' value per field in the regular, integer-based
// `eternal_timestamp` format, we MUST use *two* adjacent values so we can RECOVER
// the field state even when floating point precision loss has occurred in the lower
// priority fields due to mantissa overflow (i.e. trying to encode more bits that can
// be fit into floating point's limited mantissa size).
// When we wouldn't do this and simply encode the 'unspecified' value as, say, 
// just the next number up, we'll have an impossible task when we want to *recover* 
// that flagged value after we've done some minimal calculus or transport of the 
// floating point size produced. By aiming for the middle between two values, we can
// always reconstruct the flagged state for the field as long as the field still
// fitted within the mantissa. While this would be a sane reason to mark the zero(0)
// as 'unspecified' and thus encode it as the middle between +/- 0.5 for the field,
// this will change the behaviour of floating point value comparisons compared to
// our regular `eternal_timestamp` integer based comparisons, where 'unspecified' 
// fields are flagged by having a MAX_VALUE and thus landing AFTER instead of BEFORE
// in any sort order employing these comparisons (*iff* ETS_UNSPECIFIED_MARKER_SORTS_BEFORE_1ST_VALUE==0).
//
// For now, we consider this a FEATURE of the etd_float value: 'unspecified' equals 
// zero(0) has the added benefit that any accuracy loss due to *external* operations
// on the floating point value will, upon back-conversion, automagically pop up the
// lower significant fields as 'unspecified' thanks to them having dropped from the
// lower significant digits of the mantissa; all of which I consider a boon!
int EternalTimestamp::cvt_to_etdb_real(double &dst, const eternal_timestamp_t t)
{
	return 0;
}

int EternalTimestamp::cvt_to_proleptic_real(double &dst, const eternal_timestamp_t t)
{
	return 0;
}


int EternalTimestamp::cvt_from_timeinfo_struct(eternal_timestamp_t &dst, const struct eternal_time_tm &ts)
{
	int64_t y = ts.large_year;
	if (y == 0)
		y = ts.year;

	bool is_modern = (y <= -MODERN_EPOCH);
	bool has_epoch = true;
	if (ts.unspecified & (1 << ETTS_UNSPECIFIED_EPOCHS)) {
		is_modern = true;
		has_epoch = false;
	}

	if (is_modern) {
		eternal_modern_timestamp t{0};

		y += MODERN_EPOCH;
		ETS_ASSERT(y > 0);

		if (!has_epoch) {
			t.century = get_Invalid(ETMT_FIELDSIZE_CENTURY);
		}
		else {
			t.century = y / 100;
		}
		y %= 100;

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_YEARS)) {
			t.year = get_Invalid(ETMT_FIELDSIZE_YEAR);
		}
		else {
			t.year = FIELD_VAL_OFFSET + y;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_MONTHS)) {
			t.month = get_Invalid(ETMT_FIELDSIZE_MONTH);
		} else {
			t.month = FIELD_VAL_OFFSET - 1 + ts.month;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_DAYS)) {
			t.day = get_Invalid(ETMT_FIELDSIZE_DAY);
		} else {
			t.day = FIELD_VAL_OFFSET - 1 + ts.day;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_HOURS)) {
			t.hour = get_Invalid(ETMT_FIELDSIZE_HOUR);
		} else {
			t.hour = FIELD_VAL_OFFSET + ts.hour;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_MINUTES)) {
			t.minute = get_Invalid(ETMT_FIELDSIZE_MINUTE);
		} else {
			t.minute = FIELD_VAL_OFFSET + ts.minute;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_SECONDS)) {
			t.seconds = get_Invalid(ETMT_FIELDSIZE_SECONDS);
		} else {
			t.seconds = FIELD_VAL_OFFSET + ts.seconds;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_MILLISECONDS)) {
			t.milliseconds = get_Invalid(ETMT_FIELDSIZE_MILLISECONDS);
		} else {
			t.milliseconds = FIELD_VAL_OFFSET + ts.milliseconds;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_MICROSECONDS)) {
			t.microseconds = get_Invalid(ETMT_FIELDSIZE_MICROSECONDS);
		} else {
			t.microseconds = FIELD_VAL_OFFSET + ts.microseconds;
		}

		// t value is now a positive offset value from 10000 BC.
		dst.modern = t;
		return 0;
	} else {
		eternal_prehistoric_timestamp t{0};

		t.mode = 1;

		y = PREHISTORIC_EPOCH - y;
		ETS_ASSERT(y > 0);

		ETS_ASSERT(has_epoch);

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_YEARS)) {
			t.precision = 2;
		} else {
			t.precision = 0;
		}
		t.years = y;

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_MONTHS)) {
			t.month = get_Invalid(ETMT_FIELDSIZE_MONTH);
		} else {
			t.month = FIELD_VAL_OFFSET - 1 + ts.month;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_DAYS)) {
			t.day = get_Invalid(ETMT_FIELDSIZE_DAY);
		} else {
			t.day = FIELD_VAL_OFFSET - 1 + ts.day;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_HOURS)) {
			t.hour = get_Invalid(ETMT_FIELDSIZE_HOUR);
		} else {
			t.hour = FIELD_VAL_OFFSET + ts.hour;
		}

		if (ts.unspecified & (1 << ETTS_UNSPECIFIED_MINUTES)) {
			t.minute = get_Invalid(ETMT_FIELDSIZE_MINUTE);
		} else {
			t.minute = FIELD_VAL_OFFSET + ts.minute;
		}

		// t value is now a positive offset value from 0 AD back into history.
		dst.prehistoric = t;
		return 0;
	}
}

int EternalTimestamp::cvt_from_time_t(eternal_timestamp_t &dst, const time_t t)
{
	// as per https://stackoverflow.com/questions/68548288/convert-utc-time-t-to-utc-tm
	using namespace std::chrono;

	std::chrono::sys_seconds tp = sys_seconds{seconds{t}};

	auto td = floor<days>(tp);
	year_month_day ymd = td;
	hh_mm_ss<seconds> tod{tp - td};  // <seconds> can be omitted in C++17
	tm ts{};
	ts.tm_sec  = tod.seconds().count();
	ts.tm_min  = tod.minutes().count();
	ts.tm_hour = tod.hours().count();
	ts.tm_mday = unsigned{ymd.day()};
	ts.tm_mon  = (ymd.month() - January).count();
	ts.tm_year = (ymd.year() - 1900y).count();
	//ts.tm_wday = weekday{td}.c_encoding();
	//ts.tm_yday = (td - sys_days{ymd.year()/January/1}).count();
	ts.tm_isdst = 0;

	return cvt_from_tm(dst, ts);
}

int EternalTimestamp::cvt_from_tm(eternal_timestamp_t &dst, const struct tm &t)
{
	struct eternal_time_tm ts{0};

	ts.large_year = ts.year = t.tm_year + 1900;
	ts.month = t.tm_mon + 1;
	ts.day = t.tm_mday;
	ts.hour = t.tm_hour;
	ts.minute = t.tm_min;
	ts.seconds = t.tm_sec;

	return cvt_from_timeinfo_struct(dst, ts);
}

#if defined(_WIN32) || defined(_WIN64)
int EternalTimestamp::cvt_from_Win32FileTime(eternal_timestamp_t &dst, const FILETIME &ft)
{
	SYSTEMTIME st;
	eternal_modern_timestamp t{0};

	uint64_t tt = ft.dwHighDateTime;
	tt <<= 32;
	tt += ft.dwLowDateTime;

	const uint64_t FREQ = (static_cast<uint64_t>(1e9) / 100);
	int32_t ss = tt % FREQ;
	int32_t ms = ss / 10000;
	int32_t us = ss % 10000;
	us /= 10;

	ETS_ASSERT(us >= 0);
	ETS_ASSERT(us < 1000);
	ETS_ASSERT(ms >= 0);
	ETS_ASSERT(ms < 1000);
	t.microseconds = FIELD_VAL_OFFSET + us;
	t.milliseconds = FIELD_VAL_OFFSET + ms;

	if (FileTimeToSystemTime(&ft, &st)) {
		int y = st.wYear;
		y += MODERN_EPOCH;
		t.century = y / 100;
		t.year = FIELD_VAL_OFFSET + y % 100;
		t.month = FIELD_VAL_OFFSET - 1 + st.wMonth;
		t.day = FIELD_VAL_OFFSET - 1 + st.wDay;
		t.hour = FIELD_VAL_OFFSET + st.wHour;
		t.minute = FIELD_VAL_OFFSET + st.wMinute;
		t.seconds = FIELD_VAL_OFFSET + st.wSecond;
		t.milliseconds = FIELD_VAL_OFFSET + st.wMilliseconds;
	} else {
		ETS_ASSERT(!"Should not get here!");
	}

	// t value is now a positive offset value from 10000 BC.
	dst.modern = t;

	return 0;
}
#endif

// NOTE: our 'epoch' for the IEEEE 754 double type representation is 2000/jan/01@00:00:00.0000 UTC;
// this way we expect most dates/times to have small values and carry highest risdual precision when
// you perform calculations with/on these.
int EternalTimestamp::cvt_from_etdb_real(eternal_timestamp_t &dst, const double t)
{
	return 0;
}

int EternalTimestamp::cvt_from_proleptic_real(eternal_timestamp_t &dst, const double t)
{
	return 0;
}


bool EternalTimestamp::ntoh_is_an_empty_op(const eternal_timestamp_t t)
{
	return true;
}

// convert timestamp as loaded from external storage or network message to native layout.
eternal_timestamp_t EternalTimestamp::ntoh(const eternal_timestamp_t t)
{
	return t;
}
// convert timestamp in native layout to a layout suitable for external storage or network message travelling abroad.
eternal_timestamp_t EternalTimestamp::hton(const eternal_timestamp_t t)
{
	return t;
}
