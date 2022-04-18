#include "url_list.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *get_url_file(const char mode[static 1])
{
	const char *conf_p = get_config_path();
	const char *file_name = "/url_list.txt";
	size_t size = strlen(conf_p) + strlen(file_name) + 1;
	char *full_p = malloc(size);
	snprintf(full_p, size, "%s%s", conf_p, file_name);
	FILE *file = fopen(full_p, mode);
	free(conf_p);
	free(full_p);
	return file;
}
static int char_count(FILE *file)
{
	int count = 0;
	char c = 0;
	while (1) {
		c = fgetc(file);
		if (c == EOF || c == '\n')
			break;
		count++;
	}
	//Leaves the file's position intact.
	fseek(file, -count - 1, SEEK_CUR);
	return count;
}
char *get_next_url(FILE *file)
{
	if (!file)
		return 0;
	int size = char_count(file) + 1;
	char *url = malloc(size);
	fgets(url, size, file);
	return url;
}
bool add_url(FILE *file, const char *url)
{
	fseek(file, 0, SEEK_END);
	fputs(url, file);
}
bool remove_url(FILE *file, const char *url)
{
	if (!file)
		return false;
	const char *template = "/tmp/rss_subs_tmp.XXXXXX";
	int temp_fd = mkstemp(template);
	char *buf = malloc(1);
	size_t buf_size = 1;
	size_t line_size = 0;
	while (!feof(file)) {
		line_size = char_count(file);
		if (line_size > buf_size) {
			buf_size = line_size + 1;
			buf = realloc(buf, buf_size);
		}
		fgets(buf, buf_size, file);
		if (strcmp(buf, url))
			write(temp_fd, buf, buf_size);
	}
	file = freopen(0, "w", file);
	int read_num = 0;
	do {
		read_num = read(temp_fd, buf, buf_size - 1);
		buf[read_num] = '\0';
		fputs(buf, file);
	} while (read_num);
	free(buf);
	close(temp_fd);
	return true;
}
