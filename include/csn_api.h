#ifndef API_H_INCLUDED
#define API_H_INCLUDED

#include <stdio.h>

int csn_api_value[7];

struct csn_api_value
{
	int updated;
	int h_down, h_total, h_up;
	int n_down, n_total, n_up;
	int t_down, t_total, t_up;
	int error;
};

enum CA_ERROR {
	CAE_NOCURL, CAE_NOTOK,	CAE_NOAPI
};

size_t csn_api_writecallback(void *b, size_t size, size_t rlen, void *p);
struct csn_api_value csn_api(char* url);

#endif // API_H_INCLUDED
