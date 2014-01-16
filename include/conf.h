#ifndef CONF_H_INCLUDED
#define CONF_H_INCLUDED

#define QUOTA 10000.0


struct csn_api_config {
	const char *key;
	int refreshrate;
	int warnlevel;
	int warn;
} conf;

struct csn_api_config readconfig(const char* filename);
void writeconfig(const char* filename, struct csn_api_config myconfig);

#endif // CONF_H_INCLUDED
