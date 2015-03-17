/*
 * I did not write this, I modified it to fit kernel coding standards.
 * Original work by:
 *   Cloudef (website)[http://cloudef.pw] (github)[https://github.com/Cloudef]
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <pwd.h>

static char *strip_slash(char *str)
{
	size_t len = strlen(str);

	if (str[0] == '/')
		return str;

	if (str[len - 1] == '/')
		str[len - 1] = 0;

	return str;
}

static char *get_home(void)
{
	const char *env;
	struct passwd *pwd;

	if ((env = getenv("HOME")) && env[0] == '/')
		return strdup(env);

	if (!(pwd = getpwuid(getuid())) || !pwd->pw_dir || pwd->pw_dir[0] != '/')
		return NULL;

	return strdup(pwd->pw_dir);
}

static const char *xdg_get_path(const char *xdg_env, const char *default_path)
{
	const char *xdg_dir;
	char *home, *path;

	assert(default_path[0] != '/');

	if ((xdg_dir = getenv(xdg_env)) && xdg_dir[0] == '/')
		return strip_slash(strdup(xdg_dir));

	if (!(home = get_home()))
		return NULL; /* fatal! */

	asprintf(&path, "%s/%s", home, default_path);
	free(home);
	return path;
}
