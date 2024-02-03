
#pragma once

#ifndef __ETERNAL_TIMESTAMP_H__
#define __ETERNAL_TIMESTAMP_H__

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

// configuration setting 0/1: 1 means '0' signals 'unspecified' as that will sort before day/month/hour 1, etc.
#define ETS_UNSPECIFIED_MARKER_SORTS_BEFORE_1ST_VALUE   1


// available range: 3000 AD .. 48200 BC

struct eternal_modern_timestamp // (64 bits)
{
	unsigned sign: 1;			// MUST be ZERO.
	unsigned mode: 1;			// MUST be ZERO.

	// legal values for each field are 1..N; ZERO(0) value for the bitfield is the 'magic value' indicating 'not specified'.

	unsigned century: 9;		/// legal century values are 1..2^9-1(=511), year 0 signals "unpecified", while the other values are offsets towards the future from 10000 BC (epoch) onwards
	unsigned year: 7;           /// legal years are 1..100, NOT 0..99 as you might assume!
	unsigned month: 4;          /// legal months are 1..12, NOT 0..23 as you might assume!
	unsigned day: 5;            /// legal days are 1..31, NOT 0..30 as you might assume!
	unsigned hour: 5;			/// legal hours are 1..24, NOT 0..23 as you might assume!
	unsigned minute: 6;         /// legal minute values are 1..60, NOT 0..59 as you might assume!
	unsigned seconds: 6;        /// legal second values are 1..60, NOT 0..59 as you might assume!
	unsigned milliseconds: 10;  /// legal millisecond values are 1..1000, NOT 0..999 as you might assume!
	unsigned microseconds: 10;  /// ditto!
};
typedef struct eternal_modern_timestamp eternal_modern_timestamp_t;

// available range: 48200 BC .. Big Bang & before.

struct eternal_prehistoric_timestamp // (64 bits)
{
	unsigned sign: 1;			// MUST be ZERO.
	unsigned mode: 1;			// MUST be ONE.

	// legal values for each field are 1..N; ZERO(0) value for the bitfield is the 'magic value' indicating 'not specified'.

	uint64_t years: 38;		    /// legal year values are 1..2^38-1, year 0 signals "unpecified", while the other values are offsets towards history from 0 AD (epoch)
	unsigned month: 4;          /// legal months are 1..12, NOT 0..23 as you might assume!
	unsigned day: 5;            /// legal days are 1..31, NOT 0..30 as you might assume!
	unsigned hour: 5;			/// legal hours are 1..24, NOT 0..23 as you might assume!
	unsigned minute: 6;			/// legal minute values are 1..60, NOT 0..59 as you might assume!
	unsigned precision: 4;      /// the power of 10 for the years; e.g. 5 --> 10^5 --> "approx. 100000 years ago"; power 0 implies that the month/day/... fields may carry additional date info precision.
};
typedef struct eternal_prehistoric_timestamp eternal_prehistoric_timestamp_t;

union eternal_timestamp_u
{
	uint64_t t;
	struct eternal_modern_timestamp modern;
	struct eternal_prehistoric_timestamp prehistoric;
};
typedef union eternal_timestamp_u eternal_timestamp_t;


// the timestamp as a bunch of integer fields for general easy access, akin to `struct tm`:
struct eternal_time_tm
{
	int64_t large_year;		        // this value overides the `year` field value below. When we deliver a `tm` back to you, both will be set to the same value as long as the value is within legal `signed int` range; otherwise we'll set `year` to zero(0). 0 is 0 A.D., negative values are B.C.
	signed int year;       	        // year+century combined for ease of use. 0 is 0 A.D., negative values are B.C.

	unsigned int month;             // legal month numbers are 1..12 as 0(zero) signals 'undefined/unspecified'. However, 'unspecified' is *officially* signalled by the appropriate bit in thw `unspecified` field.
	unsigned int day;		        // legal day numbers are 1..31 as 0(zero) signals 'undefined/unspecified'. However, 'unspecified' is *officially* signalled by the appropriate bit in thw `unspecified` field.
	unsigned int hour;		        // legal hour numbers are 0..23
	unsigned int minute;		    // legal minute numbers are 0..59
	unsigned int seconds;		    // legal seconds numbers are 0..59 (we don't deal with epoch seconds)
	unsigned int milliseconds;		// legal millisecond numbers are 0..999
	unsigned int microseconds;	    // legal microsecond numbers are 0..999

	uint32_t unspecified;		    // bitfield: a SET bit signals 'unspecified' state for the given field. `microseconds` at bit position 0; the others' bit position numbers upwards from that, hence `day` is at bit position 5. See `enum eternal_unspecified_time_field_bit`.
};

enum eternal_unspecified_time_field_bit
{
	ETTS_UNSPECIFIED_MICROSECONDS = 0,
	ETTS_UNSPECIFIED_MILLISECONDS = 1,
	ETTS_UNSPECIFIED_SECONDS = 2,
	ETTS_UNSPECIFIED_MINUTES = 3,
	ETTS_UNSPECIFIED_HOURS = 4,
	ETTS_UNSPECIFIED_DAYS = 5,
	ETTS_UNSPECIFIED_MONTHS = 6,
	ETTS_UNSPECIFIED_YEARS = 7,
	ETTS_UNSPECIFIED_EPOCHS = 8,
};
typedef struct eternal_time_tm eternal_time_tm_t;

#if defined(__cplusplus)
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C++ interface definitions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <time.h>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#if defined(__cplusplus)

namespace eternal_timestamp
{
	class EternalTimestamp
	{
	public:
		static eternal_timestamp_t now();

		static eternal_timestamp_t today();
		static eternal_timestamp_t today_at(int hour = 0, int minute = 0, int second = 0);

		// produce a timestamp that has all fields set to 'not specified':
		static constexpr inline eternal_timestamp_t unknown();

		// return `true` when the given timestamp is *probably* legal/valid: this is a fast check
		// which does not expend the additional effort to precisely check the validity of the year/month/day combo,
		// but does a fast validation against the basic rules re the Proleptic Georgian Calender.
		static bool is_valid(const eternal_timestamp_t t);

		// Sam as `is_valid()` but returns a bitmask identifying the offending fields.
		// A negative return value indicates an invalid date/timestamp, while a non-zero positive
		// return value indicates a valid yet partial timestamp, whee the bitmask identifies the
		// timestamp fields which signal 'unspecified'.
		//
		// The return bits are (addressable with some help from enum eternal_unspecified_time_field_bit):
		// bit 0: microseconds unspecified
		// bit 1: milliseconds unspecified
		// bit 2: seconds unspecified
		// bit 3: minutes unspecified
		// bit 4: hours unspecified
		// bit 5: days unspecified
		// bit 6: months unspecified
		// bit 7: years unspecified
		// bit 8: epochs unspecified
		// 
		// bit 16: microseconds invalid
		// bit 17: milliseconds invalid
		// bit 18: seconds invalid
		// bit 19: minutes invalid
		// bit 20: hours invalid
		// bit 21: days invalid
		// bit 22: months invalid
		// bit 23: years invalid
		// bit 24: epochs invalid
		//
		// sign bit set when any of the 'invalid' bits are set, i.e. signal an invalid date/timestamp.
		//
		// Note: when validating, month has priority over day, hence February the 31st will get the *day*
		// marked as invalid as we won't dispute the month, like should it perhaps have been March or some such?
		static int validate(const eternal_timestamp_t t);
		static int validate(const struct eternal_time_tm &ts);

		static bool is_partial_timestamp(const eternal_timestamp_t t);

		// convert a partial timestamp by rebasing it against the given base timestamp
		static eternal_timestamp_t normalize(const eternal_timestamp_t t, const eternal_timestamp_t base);

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

		static int cvt_to_timeinfo_struct(struct eternal_time_tm &dst, const eternal_timestamp_t t);
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

		static int cvt_from_timeinfo_struct(eternal_timestamp_t &dst, const struct eternal_time_tm &t);
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

		// return `true` when 'host' machine-native format is identical to the 'network' database format (Little Endian 64bit integer)
		//
		// when `false`, every timestamp loaded from external storage or transmitted through
		// a network connection across machine boundaries must be converted to/from native
		// layout by invoking the ntoh() and hton() methods before use. (This will be needed
		// on Big Endian machines.)
		static bool ntoh_is_an_empty_op(const eternal_timestamp_t t);
		// convert timestamp as loaded from external storage or network message to native layout.
		static eternal_timestamp_t ntoh(const eternal_timestamp_t t);
		// convert timestamp in native layout to a layout suitable for external storage or network message travelling abroad.
		static eternal_timestamp_t hton(const eternal_timestamp_t t);
	};
}

#endif // __cplusplus

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// C interface definitions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif

// prefix: `ets_` for EternalTimestamp
//
// See the C++ declarations above for the description of what each function does.

#ifndef BASETYPES
typedef int                 BOOL;
#endif

eternal_timestamp_t ets_now();

eternal_timestamp_t ets_today();
eternal_timestamp_t ets_today_at(int hour, int minute, int second);

eternal_timestamp_t ets_unknown();

BOOL ets_is_valid(const eternal_timestamp_t t);
int ets_validate(const eternal_timestamp_t t);
int ets_validate_tm(const struct eternal_time_tm *ts);
BOOL ets_is_partial_timestamp(const eternal_timestamp_t t);

eternal_timestamp_t ets_normalize(const eternal_timestamp_t t, const eternal_timestamp_t base);

BOOL ets_has_century(const eternal_timestamp_t t);
BOOL ets_has_year(const eternal_timestamp_t t);
BOOL ets_has_century_and_year(const eternal_timestamp_t t);
BOOL ets_has_age(const eternal_timestamp_t t);
BOOL ets_has_month(const eternal_timestamp_t t);
BOOL ets_has_day(const eternal_timestamp_t t);
BOOL ets_has_hour(const eternal_timestamp_t t);
BOOL ets_has_minute(const eternal_timestamp_t t);
BOOL ets_has_seconds(const eternal_timestamp_t t);
BOOL ets_has_milliseconds(const eternal_timestamp_t t);
BOOL ets_has_microseconds(const eternal_timestamp_t t);
BOOL ets_has_complete_date(const eternal_timestamp_t t);
BOOL ets_has_time(const eternal_timestamp_t t);
BOOL has_hh_mm_ss(const eternal_timestamp_t t);

BOOL ets_is_modern_format(const eternal_timestamp_t t);
BOOL ets_is_prehistoric_format(const eternal_timestamp_t t);

int64_t ets_calc_time_fast_delta(const eternal_timestamp_t t1, const eternal_timestamp_t t2);
double ets_calc_time_approx_delta(const eternal_timestamp_t t1, const eternal_timestamp_t t2);

int ets_cvt_to_timeinfo_struct(struct eternal_time_tm *dst, const eternal_timestamp_t t);
int ets_cvt_to_time_t(time_t *dst, const eternal_timestamp_t t);

#if defined(_WIN32) || defined(_WIN64)
int ets_cvt_to_Win32FileTime(FILETIME *dst, const eternal_timestamp_t t);
#endif

int ets_cvt_to_etdb_real(double *dst, const eternal_timestamp_t t);

int ets_cvt_to_proleptic_real(double *dst, const eternal_timestamp_t t);

int ets_cvt_from_timeinfo_struct(eternal_timestamp_t *dst, const struct eternal_time_tm *t);
int ets_cvt_from_time_t(eternal_timestamp_t *dst, const time_t t);
int ets_cvt_from_tm(eternal_timestamp_t *dst, const struct tm *t);

#if defined(_WIN32) || defined(_WIN64)
int ets_cvt_from_Win32FileTime(eternal_timestamp_t *dst, const FILETIME *t);
#endif

int ets_cvt_from_etdb_real(eternal_timestamp_t *dst, const double t);

int ets_cvt_from_proleptic_real(eternal_timestamp_t *dst, const double t);

double ets_cvt_epoch_to_etdb_real_offset(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute, const unsigned int second, const unsigned int subsecond);

BOOL ets_ntoh_is_an_empty_op(const eternal_timestamp_t t);

eternal_timestamp_t ets_ntoh(const eternal_timestamp_t t);
eternal_timestamp_t ets_hton(const eternal_timestamp_t t);

#if defined(__cplusplus)
}
#endif

#endif // __ETERNAL_TIMESTAMP_H__
