#ifndef _URL_L_INC_GUARD
#define _URL_L_INC_GUARD
#include <stdbool.h>
#include <stdio.h>

FILE *get_url_file(const char mode[static 1]);
char *get_next_url(FILE *file);
bool add_url(FILE *file, const char *url);
bool remove_url(FILE *file, const char *url);
#endif /*_URL_L_INC_GUARD*/
