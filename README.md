# comm

![](https://github.com/knilch0r/comm/workflows/C%20CI/badge.svg)

Based on FreeBSD comm, ported to Linux and ```mmap()```, and demolished on the way.

## Why?

Imagine doing _big data_ on ascii text files. Say, for example, you want to find all lines in file A that
are also present in file B, where file B is around 100 GB in size, both files are sorted.

There's a unix tool for this job: __comm__. Unfortunately, it performs badly:

```bash
$ time LANG=C comm -12 fileA fileB >/dev/null

real	4m55.178s
user	4m25.494s
sys	0m29.418s
```
The default version from GNU coreutils uses getc() to read in the data, which seems kind of a bottleneck.
Also, it's coding style made my brain hurt.

Next, I tried a BSD version, the one from FreeBSD; there, the code looked way more nice.
```bash
$ time LANG=C ./comm-freebsd -12 fileA fileB >/dev/null

real  25m44.049s
user  24m58.376s
sys  0m45.434s
```
Unfortunately, the FreeBSD version choses to fiddle eacht input line through some wide char conversion
functions, ```malloc()```ing and ```free()```ing all along, which makes it way slower.

Fortunately, you can remove that conversion stuff quite easily.
```bash
$ time ./comm-freebsd-noconv -12 fileA fileB >/dev/null

real  3m6.182s
user  2m30.566s
sys  0m35.487s
```

Then, while removing stuff, I wondered - why not remove all that stdio FILE API and use good ol'
unportable mmap() instead? Some shortcuts during implementing reduced functionality significantly
(basically, only the options giving one column output still do what they're expected to, and
we're only handling ASCII, no i18n). But the result is quite nice:

```bash
$ time ./comm -12 fileA fileB >/dev/null

real	1m56.946s
user	1m34.344s
sys	0m21.870s
```

## To Do

* add error handling
* check if output buffering will increase performance (most likely it will)
* be as fast as ```grep``` (searching for a single line takes around 45 seconds)
* add multi-threading
* add a project logo
