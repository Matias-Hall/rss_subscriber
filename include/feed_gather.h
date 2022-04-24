#ifndef _GATHER_INC_GUARD
#define _GATHER_INC_GUARD
#include <stdbool.h>
#include <stdio.h>
bool rss_updated(const char url[static 1]);
FILE *download_rss(FILE *file, const char url[static 1]);
bool validate_rss(const char url[static 1]);
#endif /*_GATHER_INC_GUARD*/
