#include "config.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

//Checks for the existence of path.
static bool check_dir_exists(const char *path)
{
	struct stat s;
	int err = stat(path, &s);
	if (err == -1) {
		if (errno == ENOENT) {
			//Resets errno
			errno = 0;
			return false;
		} else {
			perror("stat");
			exit(1);
		}
	} else if (S_ISDIR(s.st_mode)) {
		return true;
	}
	return false;
}
const char *get_config_path(void)
{
	const char *pkg_name = "/rss_subscriber";
	const char *config_path;
	//Full config path.
	char *f_config_path;
	if (config_path = getenv("XDG_CONFIG_HOME")) {
		size_t size = strlen(config_path) + strlen(pkg_name) + 1;
		f_config_path = malloc(size);
		snprintf(f_config_path, size, "%s%s", config_path, pkg_name);
		if (check_dir_exists(f_config_path))
			return f_config_path;
	}
	if (config_path = getenv("HOME")) {
		const char *conf = "/.config";
		size_t size = strlen(config_path) + strlen(conf) + strlen(pkg_name) + 1;
		f_config_path = malloc(size);
		snprintf(f_config_path, size, "%s%s%s", config_path, conf, pkg_name);
		if (check_dir_exists(f_config_path))
			return f_config_path;
	}
	return 0;
}
const char *get_data_path(void)
{
	const char *pkg_name = "/rss_subscriber";
	const char *config_path;
	//Full config path.
	char *f_config_path;
	if (config_path = getenv("XDG_DATA_HOME")) {
		size_t size = strlen(config_path) + strlen(pkg_name) + 1;
		f_config_path = malloc(size);
		snprintf(f_config_path, size, "%s%s", config_path, pkg_name);
		if (check_dir_exists(f_config_path))
			return f_config_path;
	}
	if (config_path = getenv("HOME")) {
		const char *conf = "/.local/share";
		size_t size = strlen(config_path) + strlen(conf) + strlen(pkg_name) + 1;
		f_config_path = malloc(size);
		snprintf(f_config_path, size, "%s%s%s", config_path, conf, pkg_name);
		if (check_dir_exists(f_config_path))
			return f_config_path;
	}
	return 0;
}
