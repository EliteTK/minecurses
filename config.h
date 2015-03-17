#ifndef __CONFIG_H

#include <stdbool.h>

struct option {
	char *key;
	char *value;
};

struct option *readconf(char *);
void delconf(struct option *);
bool writeconf(struct option *, char *);

#define __CONFIG_H
#endif /* __CONFIG_H */
