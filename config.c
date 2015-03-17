#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFSIZE 2048

struct option *readconf(char *filename)
{
	FILE *config;
	struct option *options;
	unsigned optcount;
	char buffer[BUFSIZE];

	if (!(config = fopen(filename, "r")))
		return NULL;

	optcount = 0;
	options = NULL;

	while (!feof(config)) {
		if (!fgets(buffer, BUFSIZE, config))
			break;

		if (strlen(buffer) == 0)
			break;

		optcount++;

		if (!(options = realloc(options, sizeof(struct option) * optcount)))
			return NULL;

		strtok(buffer, "\n");
		if (!(options[optcount - 1].key = strdup(strtok(buffer, ":"))))
			return NULL;
		if (!(options[optcount - 1].value = strdup(strtok(NULL, ":"))))
			return NULL;
	}

	if (!(options = realloc(options, sizeof(struct option) * optcount + 1)))
		return NULL;

	fclose(config);

	return options;
}

#undef BUFSIZE

void delconf(struct option *options)
{
	unsigned i;

	i = 0;
	while (options[i].key != NULL) {
		free(options[i].key);
		free(options[i].value);
		i++;
	}

	free(options);
}

bool writeconf(struct option *options, char *filename)
{
	FILE *config;
	unsigned i;

	if (!(config = fopen(filename, "w")))
		return true;

	i = 0;
	while (options[i].key != NULL) {
		if (fprintf(config, "%s:%s\n", options[i].key, options[i].value) < 0)
			return true;

		i++;
	}

	return false;
}
