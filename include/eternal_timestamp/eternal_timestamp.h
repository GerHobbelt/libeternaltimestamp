
#pragma once

#ifndef __ETERNAL_TIMESTAMP_H__
#define __ETERNAL_TIMESTAMP_H__

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

// available range: 3000 AD .. 48200 BC

struct eternal_modern_timestamp 
{
	unsigned sign: 1;			// MUST be ZERO.
	unsigned mode: 1;			// MUST be ZERO.

	// legal values for each field are 1..N; ZERO(0) value for the bitfield is the 'magic value' indicating 'not specified'.

	unsigned century: 9;		/// legal century values are 1..2^9-1(=511), year 0 signals "unpecified", while the other values are offsets towards history from 3000 AD (epoch)
	unsigned year: 7;
	unsigned month: 4;
	unsigned day: 5;
	unsigned hour: 5;			/// legal hours are 1..24, NOT 0..23 as you might assume!
	unsigned minute: 6;         /// legal minute values are 1..60, NOT 0..59 as you might assume!
	unsigned seconds: 6;        /// legal second values are 1..60, NOT 0..59 as you might assume!
	unsigned milliseconds: 10;  /// legal millisecond values are 1..1000, NOT 0..999 as you might assume!
	unsigned microseconds: 10;  /// ditto!
};

// available range: 48200 BC .. Big Bang & before.

struct eternal_prehistoric_timestamp 
{
	unsigned sign: 1;			// MUST be ZERO.
	unsigned mode: 1;			// MUST be ONE.

	// legal values for each field are 1..N; ZERO(0) value for the bitfield is the 'magic value' indicating 'not specified'.

	uint64_t years: 38;		    /// legal century values are 1..2^38-1, year 0 signals "unpecified", while the other values are offsets towards history from 0 AD (epoch)
	unsigned month: 4;
	unsigned day: 5;
	unsigned hour: 5;			/// legal hours are 1..24, NOT 0..23 as you might assume!
	unsigned minute: 6;			/// legal minute values are 1..60, NOT 0..59 as you might assume!
	unsigned precision: 4;      /// the power of 10 for the years; e.g. "approx. 100000
};

typedef union eternal_timestamp_u 
{
	uint64_t t;
	struct eternal_modern_timestamp modern;
	struct eternal_prehistoric_timestamp prehistoric;
} eternal_timestamp_t;


// the timestamp as a bunch of integer fields for general easy access, akin to `struct tm`:
struct eternal_time_tm
{
	uint64_t large_year;		// when non-zero, this value overides the `year` field value. When we deliver a `tm` back to you, both will be set to the same value as long as the value is within legal `unsigned int` range; otherwise we'll set `year` to zero(0).
	unsigned int century;			// century
	unsigned int year_sans_epoch;	// year
	unsigned int year;       	// year+century combined, for ease of use, but might be a bit confusing as the same info can be had at `year_sans_epoch + 100*century`

	unsigned int month;         // legal day numbers are 1..12 as 0(zero) signals 'undefined/unspecified'. However, 'unspecified' is *officially* signalled by the appropriate bit in thw `unspecified` field.
	unsigned int day;		    // legal day numbers are 1..31 as 0(zero) signals 'undefined/unspecified'. However, 'unspecified' is *officially* signalled by the appropriate bit in thw `unspecified` field.
	unsigned int hour;
	unsigned int minute;
	unsigned int seconds;
	unsigned int milliseconds;
	unsigned int microseconds;

	uint32_t unspecified;		// bitfield: 1 signals 'unspecified' state for the given field. `microseconds` at bit position 0; the others' bit position numbers upwards from that, hence `day` is at bit position 5. See `enum eternal_unspecified_time_field_bit`.
};

enum eternal_unspecified_time_field_bit
{
	ETTS_UNSPECIDIED_MICROSECONDS = 0,
	ETTS_UNSPECIDIED_MILLISECONDS = 1,
	ETTS_UNSPECIDIED_SECONDS = 2,
	ETTS_UNSPECIDIED_MINUTES = 3,
	ETTS_UNSPECIDIED_HOURS = 4,
	ETTS_UNSPECIDIED_DAYS = 5,
	ETTS_UNSPECIDIED_MONTHS = 6,
	ETTS_UNSPECIDIED_YEARS = 7,
	ETTS_UNSPECIDIED_EPOCHS = 8,
};

#if defined(__cplusplus)
}
#endif

#endif
