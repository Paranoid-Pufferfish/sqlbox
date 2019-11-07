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
	size_t		 	 stmt1, stmt2;
	struct sqlbox		*p;
	struct sqlbox_cfg	 cfg;
	struct sqlbox_src	 srcs[] = {
		{ .fname = (char *)":memory:",
		  .mode = SQLBOX_SRC_RW }
	};
	struct sqlbox_pstmt	 pstmts[] = {
		{ .stmt = (char *)"CREATE TABLE foo (bar INTEGER)" },
		{ .stmt = (char *)"INSERT INTO foo (bar) VALUES (1)" },
		{ .stmt = (char *)"SELECT * FROM foo" },
		{ .stmt = (char *)"SELECT * FROM foo" },
	};

	memset(&cfg, 0, sizeof(struct sqlbox_cfg));
	cfg.msg.func_short = warnx;

	cfg.srcs.srcsz = nitems(srcs);
	cfg.srcs.srcs = srcs;
	cfg.stmts.stmtsz = nitems(pstmts);
	cfg.stmts.stmts = pstmts;

	if ((p = sqlbox_alloc(&cfg)) == NULL)
		errx(EXIT_FAILURE, "sqlbox_alloc");
	if (!sqlbox_open_async(p, 0))
		errx(EXIT_FAILURE, "sqlbox_open");
	if (!sqlbox_exec_async(p, 0, 0, 0, NULL, 0))
		errx(EXIT_FAILURE, "sqlbox_exec_async");
	if (!sqlbox_exec_async(p, 0, 1, 0, NULL, 0))
		errx(EXIT_FAILURE, "sqlbox_exec_async");

	if (!(stmt1 = sqlbox_prepare_bind(p, 0, 2, 0, NULL, 0)))
		errx(EXIT_FAILURE, "sqlbox_prepare_bind");
	if (!(stmt2 = sqlbox_prepare_bind(p, 0, 3, 0, NULL, 0)))
		errx(EXIT_FAILURE, "sqlbox_prepare_bind");

	/* Make sure we finalise first stmt2, then stmt1. */

	if (!sqlbox_finalise(p, 0))
		errx(EXIT_FAILURE, "sqlbox_finalise");
	if (!sqlbox_ping(p))
		errx(EXIT_FAILURE, "sqlbox_ping");
	if (!sqlbox_finalise(p, stmt1))
		errx(EXIT_FAILURE, "sqlbox_finalise");
	if (!sqlbox_ping(p))
		errx(EXIT_FAILURE, "sqlbox_ping");

	sqlbox_free(p);
	return EXIT_SUCCESS;
}
