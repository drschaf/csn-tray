#include <stdio.h>

#include "conf.h"

struct csn_api_config readconfig(const char* filename)
{
	struct csn_api_config myconfig;
	FILE *conffile;
	char line[81], key[16], value[64];

	myconfig.key = "asd";
	myconfig.refreshrate = 15;
	myconfig.warn = 1;
	myconfig.warnlevel = 80;

	conffile = fopen(filename, "r");
	if (!conffile) return myconfig;

	while (fgets(line, 80, conffile)) {
		sscanf(line, "%s %s\n", key, value);
		if (!strcmp("key", key)) myconfig.key = strdup(value);
		if (!strcmp("refresh", key)) myconfig.refreshrate = atoi(value);
		if (!strcmp("warn", key)) myconfig.warn = atoi(value);
		if (!strcmp("warnlevel", key)) myconfig.warnlevel = atoi(value);
	}

	myconfig.warn = !!myconfig.warn;
	myconfig.warnlevel = (myconfig.warnlevel > 100 ? 100 :
			(myconfig.warnlevel < 0 ? 0 :
				myconfig.warnlevel));
	return myconfig;
}

void writeconfig(const char* filename, struct csn_api_config myconfig)
{
	FILE *conffile;
	char line[81], key[16], value[64];

	conffile = fopen(filename, "w+");

	sprintf(line, "%s %s\n", "key", myconfig.key);
	fputs(line, conffile);

	sprintf(line, "%s %i\n", "refresh", myconfig.refreshrate);
	fputs(line, conffile);

	sprintf(line, "%s %i\n", "warn", myconfig.warn);
	fputs(line, conffile);

	sprintf(line, "%s %i\n", "warnlevel", myconfig.warnlevel);
	fputs(line, conffile);
}
