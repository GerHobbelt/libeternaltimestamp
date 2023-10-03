
#ifndef __ETERNAL_TIMESTAMP_HPP__
#include "eternal_timestamp/eternal_timestamp.hpp"
#endif

#ifndef NDEBUG
#include <stdio.h>
#include <stdlib.h>

#define ETTM_ASSERT(t)			((t) ? (void)0 : (void)fprintf(stderr, "ETTM assertion %s failed at %s:%d\n", #t, __FILE__, __LINE__))
#else
#define ETTM_ASSERT(t)			(void)0
#endif

namespace eternal_timestamp 
{
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
		unsigned int invalid = (1 << field_size_in_bits) - 1;
		return invalid;
	}

	// clip alie with 'max' as invalid value signal:
	constexpr inline unsigned int clip_MaxInvalid(int v, unsigned int field_size_in_bits)
	{
		unsigned int invalid = get_MaxInvalid(field_size_in_bits);
		// clip
		if (v < 0)
			return invalid;
		if (v >= invalid)
			return invalid;
		return v;
	}

	eternal_timestamp_t EternalTimestamp::now()
		{
			return {0};
		}

		eternal_timestamp_t EternalTimestamp::today()
		{
			return {0};
		}

		eternal_timestamp_t EternalTimestamp::today_at(int hour, int minute, int second)
		{
			eternal_timestamp_t t = today();

			ETTM_ASSERT(is_modern_format(t));
			ETTM_ASSERT(!is_partial_timestamp(t));

			t.modern.hour = clip_MaxInvalid(hour, ETMT_FIELDSIZE_HOUR);
			t.modern.minute = clip_MaxInvalid(minute, ETMT_FIELDSIZE_MINUTE);
			t.modern.seconds = clip_MaxInvalid(second, ETMT_FIELDSIZE_SECONDS);
			t.modern.milliseconds = 0;
			t.modern.microseconds = 0;

			return t;
		}

		constexpr inline eternal_timestamp_t EternalTimestamp::epoch()
		{
			return {0};
		}

		// produce a timestamp that has all fields set to 'not specified':
		constexpr inline eternal_timestamp_t EternalTimestamp::unknown()
		{
			return {0x3FFFFFFFFFFFFFFFULL}; 
		}

		bool EternalTimestamp::is_valid(const eternal_timestamp_t t)
		{
			return 0;
		}

		int EternalTimestamp::validate(const eternal_timestamp_t t)
		{
			return 0;
		}

		bool EternalTimestamp::is_partial_timestamp(const eternal_timestamp_t t)
		{
			return 0;
		}

		// convert a partial timestamp by rebasing it against the given base timestamp (using epoch() as default)
		eternal_timestamp_t EternalTimestamp::normalize(const eternal_timestamp_t t, const eternal_timestamp_t base)
		{
			return {0};
		}


		bool EternalTimestamp::has_century(const eternal_timestamp_t t)
		{
			if (is_modern_format(t))
			{
				return t.modern.century == get_MaxInvalid(ETMT_FIELDSIZE_CENTURY);
			}
			else
			{
				// Okay, this is a bit arbitrary, but we define 'has_century' here as 'known to within a century precise',
				// hence this is equivalent to a 'precison' = 2, i.e. a precision of 10^2 == 100.
				// Otherwise we'ld be talking about *milennia* or *aeons*.
				return t.prehistoric.years == get_MaxInvalid(ETPHT_FIELDSIZE_YEARS) 
					&& t.prehistoric.precision < 3;   // we need precision=2 (i.e. 10^2 ==> 100) or *better* as precision indicator or we won't know the year within the millenium.
			}
		}

		bool EternalTimestamp::has_year(const eternal_timestamp_t t)
		{
			if (is_modern_format(t))
			{
				return t.modern.year == get_MaxInvalid(ETMT_FIELDSIZE_YEAR);
			}
			else
			{
				return t.prehistoric.years == get_MaxInvalid(ETPHT_FIELDSIZE_YEARS) 
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
				return t.prehistoric.years == get_MaxInvalid(ETPHT_FIELDSIZE_YEARS)
					&& t.prehistoric.precision < 3;
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
				return t.prehistoric.years == get_MaxInvalid(ETPHT_FIELDSIZE_YEARS);
			}
		}

		bool EternalTimestamp::has_month(const eternal_timestamp_t t)
		{
			if (is_modern_format(t))
			{
				return t.modern.month == get_MaxInvalid(ETMT_FIELDSIZE_MONTH);
			}
			else
			{
				return t.prehistoric.month == get_MaxInvalid(ETPHT_FIELDSIZE_MONTH);
			}
		}

		bool EternalTimestamp::has_day(const eternal_timestamp_t t)
		{
			if (is_modern_format(t))
			{
				return t.modern.day == get_MaxInvalid(ETMT_FIELDSIZE_DAY);
			}
			else
			{
				return t.prehistoric.day == get_MaxInvalid(ETPHT_FIELDSIZE_DAY);
			}
		}

		bool EternalTimestamp::has_hour(const eternal_timestamp_t t)
		{
			if (is_modern_format(t))
			{
				return t.modern.hour == get_MaxInvalid(ETMT_FIELDSIZE_HOUR);
			}
			else
			{
				return t.prehistoric.hour == get_MaxInvalid(ETPHT_FIELDSIZE_HOUR);
			}
		}

		bool EternalTimestamp::has_minute(const eternal_timestamp_t t)
		{
			if (is_modern_format(t))
			{
				return t.modern.minute == get_MaxInvalid(ETMT_FIELDSIZE_MINUTE);
			}
			else
			{
				return t.prehistoric.minute == get_MaxInvalid(ETPHT_FIELDSIZE_MINUTE);
			}
		}

		bool EternalTimestamp::has_seconds(const eternal_timestamp_t t)
		{
			if (is_modern_format(t))
			{
				return t.modern.seconds == get_MaxInvalid(ETMT_FIELDSIZE_SECONDS);
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
				return t.modern.milliseconds == get_MaxInvalid(10);
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
				return t.modern.microseconds == get_MaxInvalid(10);
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
				return t.prehistoric.years == get_MaxInvalid(ETPHT_FIELDSIZE_YEARS)
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
				// We accept that prehistoric timestamps on't come with *seconds*...
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
		int64_t EternalTimestamp::calc_time_fast_delta(const eternal_timestamp_t t1, const eternal_timestamp_t t2)
		{
			// Here's why we did not bother to use bit63 in our format spec: now we have no problem turning uint64_t into int64_t 
			// and compare the two!
			int64_t a = t1.t;
			int64_t b = t2.t;
			return a - b;
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
		//   bankers and acturarians when they calculate with 400-day years for (some of) their financial modeling. ;-)
		double EternalTimestamp::calc_time_approx_delta(const eternal_timestamp_t t1, const eternal_timestamp_t t2)
		{
			eternal_time_tm d1, d2;
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


		int EternalTimestamp::cvt_to_timeinfo_struct(eternal_time_tm &dst, const eternal_timestamp_t t)
		{
			if (is_modern_format(t))
			{
				return has_hour(t) && has_minute(t) && has_seconds(t);
			}
			else
			{
				// We accept that prehistoric timestamps on't come with *seconds*...
				return has_hour(t) && has_minute(t);
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
		// floating poin's inherent *precision loss* when calculating with disparate numbers,
		// we encode each 'unspecified' field as THE MIDDLE between TWO 'out-of-range' values:
		// where we use a *single* 'unspecified' value per field in the regular, integer-based
		// `eternal_timestamp` format, we MUST use *two* adjacent values so we can RECOVER
		// the field state even when loating point precision loss has occurred in the lower
		// priority fields due to mantissa overflow (i.e. trying to encode more bits that can
		// be fit into  floating point's limited mantissa size).
		// When we wouldn't do this and simply encode the 'unspecified' value as, say, 
		// just the next number up, we'll have an impossible task whn we ant to *recover* 
		// that flagged value after we've done some minimal calculus or transport of the 
		// floating point size produced. By aimin for the middle between two values, we can
		// always reconstruct the flagged state for the field as long as the field still
		// fitted within the mantissa. While this would be a sane reason to mark the zero(0)
		// as 'unspecified' and thus encode it as the middle between +/- 0.5 for the field,
		// this will change the behaviour of floating point valeu comparisons compared to
		// our regular `eternal_timestamp` integer based comparisons, where 'unspecified' 
		// fields are flagged by having a MAX_VALUE and thus landing AFTER instead of BEFORE
		// in any sort order employin these comparisons.
		//
		// for now, we consider this a FEATURE of the etd_float value: 'unspecified' equals 
		// zero(0) has the added benefit that any accuracy loss due to *external* operations
		// on the floating point value will, upon back-conversion, automagically pop up the
		// lower signiicant fields as 'unspecified' thanks to them having dropped from the
		// lower significant digits of the mantissa; all of which I consider a boon!
		int EternalTimestamp::cvt_to_etdb_real(double &dst, const eternal_timestamp_t t)
		{
			return 0;
		}

		int EternalTimestamp::cvt_to_proleptic_real(double &dst, const eternal_timestamp_t t)
		{
			return 0;
		}


		int EternalTimestamp::cvt_from_timeinfo_struct(eternal_timestamp_t &dst, const eternal_time_tm &t)
		{
			return 0;
		}

		int EternalTimestamp::cvt_from_time_t(eternal_timestamp_t &dst, const time_t t)
		{
			return 0;
		}

		int EternalTimestamp::cvt_from_tm(eternal_timestamp_t &dst, const struct tm &t)
		{
			return 0;
		}


#if defined(_WIN32) || defined(_WIN64)
		int EternalTimestamp::cvt_from_Win32FileTime(eternal_timestamp_t &dst, const FILETIME &t)
		{
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


		// and a helper method in case you need to process an epoch'ed IEEE 754 double value for your timestamp.
		inline constexpr double EternalTimestamp::cvt_epoch_to_etdb_real_offset(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute, const unsigned int second, const unsigned int subsecond)
		{
			return 0;
		}

}

