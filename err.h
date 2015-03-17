#ifndef __ERR_H

#include <stdlib.h>
#include <errno.h>

enum loglevel {
	fatal,
	error,
	warning,
	info,
	debug
};

enum loglevel loglevel = fatal;

void err_msg(enum loglevel, int, char *, unsigned, char *, ...);

#define _err_msg(level, fmt, ...) err_msg(level, __FILE__, __LINE__, fmt ,##__VA_ARGS__)

#define err_fatal(fmt, ...) _err_msg(fatal, errno, fmt ,##__VA_ARGS__); exit(EXIT_FAILURE)
#define err_error(fmt, ...) _err_msg(error, errno, fmt ,##__VA_ARGS__)
#define err_warning(fmt, ...) _err_msg(warning, errno, fmt ,##__VA_ARGS__)
#define err_info(fmt, ...) _err_msg(info, errno, fmt ,##__VA_ARGS__)
#define err_debug(fmt, ...) _err_msg(debug, errno, fmt ,##__VA_ARGS__)

#define __ERR_H
#endif /* __ERR_H */
