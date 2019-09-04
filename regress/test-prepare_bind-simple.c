/*	$Id$ */
/*
 * Copyright (c) 2019 Kristaps Dzonsons <kristaps@bsd.lv>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHORS DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "../config.h"

#if HAVE_ERR
# include <err.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "../sqlbox.h"
#include "regress.h"

int
main(int argc, char *argv[])
{
	size_t		 	 dbid;
	struct sqlbox		*p;
	struct sqlbox_cfg	 cfg;
	struct sqlbox_src	 srcs[] = {
		{ .fname = (char *)":memory:" }
	};
	struct sqlbox_pstmt	 pstmts[] = {
		{ .stmt = (char *)"INSERT INTO foo "
			"(bar, baz) VALUES (?, ?)" }
	};
	struct sqlbox_bound	 parms[] = {
		{ .iparm = 10,
		  .type = SQLBOX_BOUND_INT },
		{ .iparm = 20,
		  .type = SQLBOX_BOUND_INT }
	};

	memset(&cfg, 0, sizeof(struct sqlbox_cfg));
	cfg.msg.func_short = warnx;

	cfg.srcs.srcsz = nitems(srcs);
	cfg.srcs.srcs = srcs;
	cfg.stmts.stmtsz = nitems(pstmts);
	cfg.stmts.stmts = pstmts;

	if ((p = sqlbox_alloc(&cfg)) == NULL)
		return EXIT_FAILURE;

	if (!(dbid = sqlbox_open(p, 0)))
		return EXIT_FAILURE;
	if (!sqlbox_ping(p))
		return EXIT_FAILURE;
	if (!sqlbox_prepare_bind(p, dbid, 0, nitems(parms), parms))
		return EXIT_FAILURE;
	if (!sqlbox_ping(p))
		return EXIT_FAILURE;
	sqlbox_free(p);
	return EXIT_SUCCESS;
}
