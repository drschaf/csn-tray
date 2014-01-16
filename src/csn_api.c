#include <stdio.h>
#include <curl/curl.h>
#include <string.h>

#include "../include/csn_api.h"

size_t csn_api_writecallback(void *b, size_t size, size_t rlen, void *p)
{
	char **resp = (char**)p;
	size_t mylen = size *rlen;

	*resp = strndup(b, mylen);

	return rlen;
}

struct csn_api_value csn_api(char* url)
{
	struct csn_api_value val = {0,0,0,0, 0,0,0,0};

	char *recv = NULL;

	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	curl = curl_easy_init();
	if(!curl) {
		curl_easy_cleanup(curl);
		curl_global_cleanup();
		val.error = CAE_NOCURL;
		return val;
	}

	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Muuh übelster schaf CSN Client");
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, csn_api_writecallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &recv);

	res = curl_easy_perform(curl);

	if(res != CURLE_OK) {
		val.error = CAE_NOTOK;
		curl_easy_cleanup(curl);
		curl_global_cleanup();

		return val;
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	// i don't like json.
	char sf[] = "%*s \"%i\", "
		"\"happyhour\": {"
			"\"download\": %i, "
			"\"total\": %i, "
			"\"upload\": %i"
		"}, \"normal\": {"
			"\"download\": %i, "
			"\"total\": %i, "
			"\"upload\": %i"
		"}}";
	sscanf(recv, sf,
	    &val.updated,
	    &val.h_down, &val.h_total, &val.h_up,
	    &val.n_down, &val.n_total, &val.n_up
	);

	if (!val.updated)
		val.error = CAE_NOAPI;
	return val;
}
