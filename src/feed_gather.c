#include "feed_gather.h"
#include <curl/curl.h>
#include <stddef.h>
//TODO: Change to using multi curl, which is non-blocking.
FILE *download_rss(FILE *file, const char url[static 1])
{
	CURL *curl = curl_easy_init();
	if (curl) {
		//code used throughout function to catch errors.
		CURLcode code = 0;
		code = curl_easy_setopt(curl, CURLOPT_URL, url);
		if (code != CURLE_OK)
			return 0;
		//By default (NULL), the data will be written to the FILE* passed as WRITEDATA.
		code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		if (code != CURLE_OK)
			return 0;
		code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
		if (code != CURLE_OK)
			return 0;
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return file;
	}
	return 0;
}
