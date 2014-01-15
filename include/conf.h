#ifndef CONF_H_INCLUDED
#define CONF_H_INCLUDED

#define QUOTA 10000.0


struct csn_api_config {
	char *key;
	int refreshrate;
	int warnlevel;
	int warn;
} conf;

struct csn_api_config readconfig(const char* filename);

#endif // CONF_H_INCLUDED
