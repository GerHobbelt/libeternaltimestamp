
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
	unsigned dummy: 1;
	unsigned mode: 1;     // MUST be ZERO.

	// legal values for each field are 0..N-1; highest possible value for the bitfield is the 'magic value' indicating 'not specified'.

	unsigned century: 9;
	unsigned year: 7;
	unsigned month: 4;
	unsigned day: 5;     // NOTE: legal day numbers are 0..30, NOT 1..31 as you might assume!
	unsigned hour: 5;
	unsigned minute: 6;
	unsigned seconds: 6;
	unsigned milliseconds: 10;
	unsigned microseconds: 10;
};

// available range: 48200 BC .. Big Bang & before.

struct eternal_prehistoric_timestamp 
{
	unsigned dummy: 1;
	unsigned mode: 1;     // MUST be ONE.

	// legal values for each field are 0..N-1; highest possible value for the bitfield is the 'magic value' indicating 'not specified'.

	uint64_t years: 38;
	unsigned month: 4;
	unsigned day: 5;     // NOTE: legal day numbers are 0..30, NOT 1..31 as you might assume!
	unsigned hour: 5;
	unsigned minute: 6;
	unsigned precision: 4;    // power of 10 for the years
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
