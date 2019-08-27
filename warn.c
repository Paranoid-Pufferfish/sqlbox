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
#include "config.h"

#if HAVE_SYS_QUEUE
# include <sys/queue.h>
#endif 

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sqlite3.h>

#include "sqlbox.h"
#include "extern.h"

void
sqlbox_warnx(const struct sqlbox_cfg *cfg, const char *fmt, ...)
{
	va_list	 ap;
	char	 buf[BUFSIZ]; /* FIXME */

	if (cfg == NULL)
		return;
	if (cfg->msg.func == NULL &&
	    cfg->msg.func_short == NULL)
		return;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	if (cfg->msg.func != NULL)
		cfg->msg.func(buf, cfg->msg.dat);
	else
		cfg->msg.func_short(buf);
}

void
sqlbox_warn(const struct sqlbox_cfg *cfg, const char *fmt, ...)
{
	int	 er = errno;
	va_list	 ap;
	char	 buf[BUFSIZ]; /* FIXME */
	int	 sz;

	if (cfg == NULL)
		return;
	if (cfg->msg.func == NULL &&
   	    cfg->msg.func_short == NULL)
		return;
	va_start(ap, fmt);
	sz = vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	if (sz < 0)
		return;
	if ((size_t)sz < sizeof(buf)) 
		strlcat(buf, strerror(er), sizeof(buf));
	if (cfg->msg.func != NULL)
		cfg->msg.func(buf, cfg->msg.dat);
	else
		cfg->msg.func_short(buf);
}

