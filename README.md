# lib::eternal_timestamp

small C/C++ header / library which provides (encodes/decodes) `int64_t` 64-bit integer timestamps which can encode any date/time in the lifetime of our planet up to about 3000 AD in the future.

Special attention is paid to the period where humanity has been writing: you can specify dates & times back to about 50,000BC in at least millisecond precision. Dates older than that are slightly less precise.

This timestamp is useful for any of these purposes:

- you are working with databases and are looking for a non-time_t / 2038AD catastrophic date/time field type
- prefer your date/timestamps encoded as a plain 64 bit integer so you can use them as a fast unique key too in arbitrary databases & tables (next to using it as a regular field value)
- you want to use high precision date/timestamps for any new or old written material, e.g. when working in a library or recording archeological data
- plan to use this type as an Sqlite extension
- need to store dates/times before "*epoch*", i.e. before 1/jan/1970
- need to store dates/times reaching back as far as 5000 BC at least
- like to store dates/times reaching back as far as 50,000 BC at least (yep, that's *not a typo*: we took the estimated date for the first human writing and then added a copious safety margin to that in order to be safe when future discoveries inform us our predecessors have been writing *before*.
- like to store reasonably precise timestamps back to the estimated creation of our planet (the **scientific estimate**, that is. That's a couple *billion* years ago...)
- need to compare many date/timestamps like that, and pretty fast too. For you, we've ensured this timestamp, encoded in a `int64_t` integer value, is nicely incremental over the entire date/time range: "*older*" is just a \< (less than) integer comparison away, "*equality*" is just applying the `==` integer operation.


## History

First concept was written here:



