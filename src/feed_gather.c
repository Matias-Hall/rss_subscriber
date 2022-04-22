#include "feed_gather.h"
#include <ctype.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xmlstring.h>
#include <stddef.h>
#include <string.h>

// TODO: Change to using multi curl, which is non-blocking.
FILE *download_rss(FILE *file, const char url[static 1])
{
	CURL *curl = curl_easy_init();
	if (curl) {
		// code used throughout function to catch errors.
		CURLcode code = 0;
		code = curl_easy_setopt(curl, CURLOPT_URL, url);
		if (code != CURLE_OK)
			return 0;
		// By default (NULL), the data will be written to the FILE* passed as
		// WRITEDATA.
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
//Case insensitive (ci) strstr. Works by converting both strings to lowercase versions.
static char *ci_strstr(const char str[static 1], const char substr[static 1])
{
	char *low_str = strdup(str);
	for (size_t i = 0; i < strlen(low_str); i++)
		low_str[i] = tolower(low_str[i]);
	char *low_substr = strdup(str);
	for (size_t i = 0; i < strlen(low_substr); i++)
		low_substr[i] = tolower(low_substr[i]);
	char *occ = strstr(low_str, low_substr);
	free(low_str);
	free(low_substr);
	if (!occ)
		return 0;
	//(occ - low_str) gets the index of the found substring, which is then added to the original str pointer. 
	return ((occ - low_str) + str);
}
static size_t internal_validate_rss(char *buffer,
                       size_t size,
                       size_t nitems,
                       void *userdata)
{
	bool *is_xml = (bool *)userdata;
	if (*is_xml)
		return size * nitems;
	buffer[nitems - 1] = '\0';
	const char key[] = "content-type:";
	if (ci_strstr(buffer, key)) {
		char *possible_types[] = {"application/rss+xml", "application/rdf+xml", "application/atom+xml", "application/xml", "text/xml"};
		for (size_t i = 0; i < sizeof(possible_types)/sizeof(possible_types[0]); i++) {
			if (strstr(buffer, possible_types[i])) {
				*is_xml = true;
				break;
			}
		}
	}
	return size * nitems;
}
// TODO: More advanced validation.
bool validate_rss(const char url[static 1])
{
	if (!url)
		return false;
	CURL *curl = curl_easy_init();
	CURLcode code = 0;
	bool is_xml = false;
	code = curl_easy_setopt(curl, CURLOPT_URL, url);
	if (code)
		fprintf(stderr, "libcurl error: %d", code);
	code = curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	if (code)
		fprintf(stderr, "libcurl error: %d", code);
	code = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, internal_validate_rss);
	if (code)
		fprintf(stderr, "libcurl error: %d", code);
	code = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &is_xml);
	if (code)
		fprintf(stderr, "libcurl error: %d", code);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return is_xml;
}
