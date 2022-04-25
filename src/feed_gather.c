#include "feed_gather.h"
#include <ctype.h>
#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/xmlstring.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

//Case insensitive (ci) strstr. Works by converting both strings to lowercase versions.
static char *ci_strstr(const char str[static 1], const char substr[static 1])
{
	char *low_str = malloc(strlen(str) * sizeof(*low_str));
	low_str[strlen(str) - 1] = '\0';
	for (size_t i = 0; i < strlen(str); i++)
		low_str[i] = tolower(str[i]);
	char *low_substr = malloc(strlen(substr) * sizeof(*low_substr));
	for (size_t i = 0; i < strlen(substr); i++)
		low_substr[i] = tolower(substr[i]);
	char *occ = strstr(low_str, low_substr);
	free(low_str);
	free(low_substr);
	if (!occ)
		return 0;
	//(occ - low_str) gets the index of the found substring, which is then added to the original str pointer. 
	return ((occ - low_str) + str);
}

//Returns month number (Jan == 0), given a three character code.
static int month_to_int(char month[static 4])
{
	if (!strncmp(month, "Jan", 3))
		return 0;
	if (!strncmp(month, "Feb", 3))
		return 1;
	if (!strncmp(month, "Mar", 3))
		return 2;
	if (!strncmp(month, "Apr", 3))
		return 3;
	if (!strncmp(month, "May", 3))
		return 4;
	if (!strncmp(month, "Jun", 3))
		return 5;
	if (!strncmp(month, "Jul", 3))
		return 6;
	if (!strncmp(month, "Aug", 3))
		return 7;
	if (!strncmp(month, "Sep", 3))
		return 8;
	if (!strncmp(month, "Oct", 3))
		return 9;
	if (!strncmp(month, "Nov", 3))
		return 10;
	if (!strncmp(month, "Dec", 3))
		return 11;
	return -1;
}
// TODO: Use ETAGs maybe.
static size_t internal_rss_updated(char *buffer,
                       size_t size,
                       size_t nitems,
                       void *userdata)
{
	struct tm **time = (struct tm **)userdata;
	if (*time)
		return size * nitems;
	buffer[nitems - 1] = '\0';
	const char key[] = "last-modified:";
	if (ci_strstr(buffer, key)) {
		struct tm *tm_p = calloc(1, sizeof(*tm_p));
		*time = tm_p;
		char smonth[4];
		sscanf((buffer + strlen(key)), " %*3s, %2d %3s %d %2d:%2d:%2d",
				&tm_p->tm_mday,
				smonth,
				&tm_p->tm_year,
				&tm_p->tm_hour,
				&tm_p->tm_min,
				&tm_p->tm_sec);
		//sscanf will assign actual year, tm_year is years since 1900.
		//Therefore, 1900 must be subtracted to get the correct interpretation.
		tm_p->tm_year -= 1900;
		tm_p->tm_mon = month_to_int(smonth);
	}
	return size * nitems;
}
static struct tm *fetch_updated_date(const char url[static 1])
{
	if (!url)
		return 0;
	CURL *curl = curl_easy_init();
	CURLcode code = 0;
	struct tm *tm = 0;
	code = curl_easy_setopt(curl, CURLOPT_URL, url);
	if (code)
		fprintf(stderr, "libcurl error: %d", code);
	code = curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
	if (code)
		fprintf(stderr, "libcurl error: %d", code);
	code = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, internal_rss_updated);
	if (code)
		fprintf(stderr, "libcurl error: %d", code);
	code = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &tm);
	if (code)
		fprintf(stderr, "libcurl error: %d", code);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return tm;
}
bool rss_updated(const char url[static 1])
{
	FILE *f = fopen("/var/lib/rss_subscriber/rss_feeds_update", "r");
	const size_t buf_size = strlen(url) + 1;
	char *buf = malloc(buf_size * sizeof(*buf));
	struct tm tm = { 0 };
	bool tm_set = false;
	while (!feof(f)) {
		fgets(buf, buf_size, f);
		char smonth[4];
		if (!strncmp(buf, url, strlen(url))) {
			fscanf(f, " %*3s, %2d %3s %d %2d:%2d:%2d",
					&tm.tm_mday,
					smonth,
					&tm.tm_year,
					&tm.tm_hour,
					&tm.tm_min,
					&tm.tm_sec);
			//fscanf will assign actual year, tm_year is years since 1900.
			//Therefore, 1900 must be subtracted to get the correct interpretation.
			tm.tm_year -= 1900;
			tm.tm_mon = month_to_int(smonth);
			tm_set = true;
			break;
		}
	}
	fclose(f);
	free(buf);
	//If the feed has never been uploaded, it is always "updated".
	if (!tm_set)
		return true;
	struct tm *new_tm = fetch_updated_date(url);
	bool res = mktime(new_tm) > mktime(&tm);
	free(new_tm);
	return res;
}

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
