#include "url_list.h"
#include "config.h"
#include "feed_gather.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *get_url_file(const char mode[static 1])
{
	char *conf_p = get_config_path();
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
	// Stores the file position to restore later.
	long int pos = ftell(file);
	while (1) {
		c = fgetc(file);
		if (c == EOF || c == '\n')
			break;
		count++;
	}
	// Restores the file position.
	fseek(file, pos, SEEK_SET);
	return count;
}
char *get_next_url(FILE *file)
{
	if (!file)
		return 0;
	//+1 accounts for null-terminating character.
	int size = char_count(file) + 1;
	//Checks for a size of 1, which means there are no characters to read.
	if (size <= 1)
		return 0;
	char *url = malloc(size);
	fgets(url, size, file);
	//Skips newline character.
	fseek(file, 1, SEEK_CUR);
	return url;
}
bool add_url(FILE *file, const char *url)
{
	if (!validate_rss(url))
		return false;
	fseek(file, 0, SEEK_END);
	// If fputs and fputc return non-negative value, the write was succesful.
	if (fputs(url, file) >= 0 && fputc('\n', file) >= 0)
		return true;
	return false;
}
bool remove_url(FILE *file, const char *url)
{
	if (!file)
		return false;
	char template[] = "/tmp/rss_subs_tmp.XXXXXX";
	int temp_fd = mkstemp(template);
	if (temp_fd == -1) {
		if (errno == EINVAL)
			printf("EINVAL error\n");
		exit(errno);
	}
	printf("%s\n", template);
	char *buf = malloc(1);
	size_t buf_size = 1;
	size_t line_size = 0;
	while (!feof(file)) {
		line_size = char_count(file);
		if (line_size + 2 > buf_size) {
			//+2 accounts for newline and null-terminating character.
			buf_size = line_size + 2;
			buf = realloc(buf, buf_size);
		}
		fgets(buf, buf_size, file);
		// line_size is passed so newline character doesn't mess up comparison +
		// safety.
		if (strncmp(buf, url, line_size)) {
			write(temp_fd, buf, strlen(buf));
		}
	}
	file = freopen(0, "w", file);
	int read_num = 0;
	// Resets the file offset from the tmp file.
	lseek(temp_fd, 0, SEEK_SET);
	do {
		read_num = read(temp_fd, buf, buf_size - 1);
		buf[read_num] = '\0';
		printf("%s\n", buf);
		fputs(buf, file);
	} while (read_num);
	free(buf);
	// By unlinking, the file will be deleted after closing.
	unlink(template);
	close(temp_fd);
	return true;
}
