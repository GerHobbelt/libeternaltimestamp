# lib::eternal_timestamp

small C/C++ header / library which provides (encodes/decodes) `int64_t` 64-bit integer timestamps which can encode any *partially known* or *wholly complete* date/time in the lifetime of our planet up to about 3000 AD in the future. (When you extend the format to use the sign bit (bit 63) you can encode any date/time until the End Of Days / Armageddon ... and beyond.)

Special attention is paid to the period where humanity has been writing: you can specify dates & times back to about 20,000BC in at least milisecond precision. Dates older than that are slightly less precise.

This timestamp is useful for any of these purposes (and more...):

- you are working with databases and are looking for a non-`time_t` / 2038AD catastrophic date/time field type ([the looming UNIX Y2K event](https://en.wikipedia.org/wiki/Year_2038_problem))
- prefer your date/timestamps encoded as a plain positive 64 bit integer so you can use them as a fast unique key too in arbitrary databases & tables (next to using it as a regular field value)[^1]
- you want to use high precision date/timestamps for any new or old written material, e.g. when working in a library or recording archeological data
- plan to use this type as an SQLite extension
- need to store dates/times before "*epoch*", i.e. before 1/jan/1970
- need to store dates/times reaching back into antiquity, as far as 5000 BC *at least*
- like to store dates/times reaching back as far as 20,000 BC at least (yep, that's *not a typo*: we took the estimated date for the first human writing and then added a copious safety margin to that in order to be safe when future discoveries inform us our predecessors have been writing *before*.
- like to store reasonably precise timestamps back to the estimated creation of our planet. The **scientific estimate**, that is. That's a couple *billion* years ago... Maybe you want/need to go back even further: this date/timestamp *carries the day*. Both *figuratively* and *literally*.
- need to compare many date/timestamps like that, and pretty fast too. For you, we've ensured this timestamp, encoded in a `int64_t` integer value, is nicely incremental over the entire date/time range: "*older*" is just a \< (less than) integer comparison away, "*equality*" is just applying the `==` integer operation.
- need a date/timestamp that's *comparable* (i.e. *which is older/newer?*) and *sortable* (so you can produce an *ordered list* of these dates (plus the other row data).
- are looking for a date/timestamp which can store both complete *and* **partial date/times**, e.g. "15 August @ 09:00" (no year given, no seconds, nor subseconds) -- and use those in any mix, with a sort order of your choice (several sort criteria functions are available off the shelf).


## History

First concept was written here: [Qiqqa timestamp design considerations](https://github.com/jimmejardine/qiqqa-open-source/blob/master/docs-src/Notes/Progress%20in%20Development/Considering%20the%20Way%20Forward/Database%20Design/Storing%20a%20wide%20range%20of%20date%2Btime-stamps%20of%20arbitrary%20precision%20in%20a%2064-bit%20number.md)

---

[^1]: by design, we encode *all date/timestamps* in the lower 63 bits, i.e. we DO NOT use the *negative values*, thus excluding any possible confusion *anywhere* in your software between `uint64_t` ("*unsigned* integer") and `int64_t` ("*signed* integer") types -- something that can easily happen in large(r) codebases and multi-component systems. It also means our date/timestamps can be used *directly* as PRIMARY/UNIQUE KEY in any database, delivering fast access due to the timestamp being a single (64 bit wide) INTEGER element. If you want to know more technical details about the timestamp design/layout, see the analysis in the "History" section.





## Building using CMake

Create the `build` directory and configure the build system:

```bash
cmake -B build # Or `cmake -D BUILD_TESTING=OFF -B build` to skip tests. 
```

Build the project:

```bash
cmake --build build
```

Inspect the `build` directory to find the application and the tests.

Optionally, run the tests with `ctest` by typing:

```bash
cmake -E chdir build ctest
```

Run the demo application by typing:

```bash
cmake -E chdir build demo/eternal-demo
```

-----

