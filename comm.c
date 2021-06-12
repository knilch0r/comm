/*-
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2021 knilch0r. Based on and derived from code that's
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Case Larsen.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#if 0
#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 1989, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif
#endif
#if 0
#ifndef lint
static char sccsid[] = "From: @(#)comm.c	8.4 (Berkeley) 5/4/95";
#endif
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <err.h>
#include <limits.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __GNUC__
#define likely(x)     __builtin_expect((x),1)
#define unlikely(x)   __builtin_expect((x),0)
#else
#define likely(x)     x
#define unlikely(x)   x
#endif

static void file(const char *name, const char **fp, size_t *len);
static void show(const char *p, int offset, size_t len);
static void usage(void);

static inline int lncompare(const char *sa, const char *sb)
{
	int ca, cb;
	do {
		ca = *sa++;
		cb = *sb++;
		if (unlikely(ca == '\n')) return ca - cb;
		if (unlikely(!ca)) return ca - cb;
	} while (ca == cb);
	return ca - cb;
}

static inline void nextline(const char **fp, size_t *fl)
{
	const char *old = *fp, *new = *fp;
	int c = *new++;
	while (likely(c != '\n' && c)) c = *new++;
	*fl -= (new - old);
	*fp = new;
}

int main(int argc, char *argv[])
{
	int comp;
	int ch, flag1, flag2, flag3;
	const char *fp1, *fp2;
	size_t fl1, fl2;
	int col1, col2, col3;
	int p;
	const char *old;

	flag1 = flag2 = flag3 = 1;

	while ((ch = getopt(argc, argv, "123i")) != -1)
		switch(ch) {
		case '1':
			flag1 = 0;
			break;
		case '2':
			flag2 = 0;
			break;
		case '3':
			flag3 = 0;
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc != 2)
		usage();

	file(argv[0], &fp1, &fl1);
	file(argv[1], &fp2, &fl2);

	/* for each column printed, remember its number */
	p = 1;
	col1 = col2 = col3 = 0;
	if (flag1)
		col1 = p++;
	if (flag2)
		col2 = p++;
	if (flag3)
		col3 = p;

	while (1) {
		/* if one file done, display the rest of the other file */
		if (!fl1) {
			if (*fp2 && col2)
				show(fp2, col2, fl2);
			break;
		}
		if (!fl2) {
			if (*fp1 && col1)
				show(fp1, col1, fl1);
			break;
		}

		comp = lncompare(fp1, fp2);

		/* lines are the same */
		if (!comp) {
			old = fp1;
			nextline(&fp1, &fl1);
			nextline(&fp2, &fl2);
			if (col3)
				/* FIXME prepend col3 */
				write(STDOUT_FILENO, old, fp1 - old);
				/* FIXME handle errors */
			continue;
		}

		/* lines are different */
		if (comp < 0) {
			old = fp1;
			nextline(&fp1, &fl1);
			if (col1)
				/* FIXME prepend col1 */
				write(STDOUT_FILENO, old, fp1 - old);
				/* FIXME handle errors */
		} else {
			old = fp2;
			nextline(&fp2, &fl2);
			if (col2)
				/* FIXME prepend col2 */
				write(STDOUT_FILENO, old, fp2 - old);
				/* FIXME handle errors */
		}
	}
	exit(0);
}

static void show(const char *p, int offset, size_t len)
{
	/* FIXME: do it line-by-line and prepend offset */
	write(STDOUT_FILENO, p, len);
	/* FIXME handle errors / EINTR */
}

static void file(const char *name, const char **fp, size_t *len)
{
	int fd;
	struct stat s;

	if (!strcmp(name, "-")) errx(1, "stdin not supported");

	if (   ((fd = open(name, O_RDONLY)) < 0)
	    || (fstat(fd, &s) < 0)
	   ) err(1, "%s", name);

	*len = s.st_size;
	if ((*fp = mmap(NULL, *len, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) err(1, "%s (mmap)", name);
}

static void usage(void)
{
	(void)fprintf(stderr, "usage: comm [-123] file1 file2\n");
	exit(1);
}
