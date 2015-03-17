#define _GNU_SOURCE
#include "err.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

void err_msg(enum loglevel level, int errnum, char *filename, unsigned lineno,
	char *fmt, ...)
{
	va_list args;
	char *message;
	char *prefix;

	if (loglevel < level)
		return;

	va_start(args, fmt);
	if (vasprintf(&message, fmt, args) == -1) {
		fprintf(stderr, "fatal - %s:%d - error handler error: %d, %d, %s, %d, %s", __FILE__, __LINE__, level, errnum, filename, lineno, fmt);
		exit(EXIT_FAILURE); /* If this happens, we're having bad luck. */
	}
	va_end(args);

	switch (level) {
	case fatal: prefix = "fatal"; break;
	case warning: prefix = "warning"; break;
	case info: prefix = "info"; break;
	case debug: prefix = "debug"; break;
	default: prefix = "unknown";
	}

	if (level <= warning && errnum)
		fprintf(stderr, "%s - %s:%d - %s - %s\n", prefix, filename,
			lineno, message, strerror(errnum));
	else
		fprintf(stderr, "%s - %s:%d - %s\n", prefix, filename, lineno, message);

	free(message);
}
