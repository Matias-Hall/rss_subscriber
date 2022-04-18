#include "config.h"
#include "feed_gather.h"
#include "url_list.h"
#include "minunit.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>

int tests_run = 0;

static char *test_get_config(void)
{
	printf("Running test_get_config.\n");
	const char *path = get_config_path();
	mu_assert("Error config path incorrect.",
		  path && !strcmp(path,
			  "/home/matias/.config/rss_subscriber"));
	return 0;
}
//Used to ensure file is closed even if test fails.
static char *tfg_helper(FILE *file)
{
	mu_assert("Error gathering feed.", download_rss(file, "https://archlinux.org/feeds/news/"));
	return 0;
}
static char *test_feed_gather(void)
{
	printf("Running test_feed_gather.\n");
	FILE *file = fopen("archlinux.xml", "w");
	char *m = tfg_helper(file);
	fclose(file);
	return m;
}
static char *test_get_url_file(FILE **file)
{
	mu_assert("Error getting url file.", *file = get_url_file("r"));
	return 0;
}
static char *test_get_next_url(FILE *file)
{
	mu_assert("Error getting next url.", !strcmp(get_next_url(file), "https://archlinux.org/feeds/news/"));
	return 0;
}
static char *all(void)
{
	mu_run_test(test_get_config());
	mu_run_test(test_feed_gather());
	FILE *f;
	mu_run_test(test_get_url_file(&f));
	mu_run_test(test_get_next_url(f));
	return 0;
}

int main(void)
{
	char *res = all();
	if (res) {
		printf("%s\n", res);
	} else {
		printf("All (%d) tests passed.\n", tests_run);
	}
}
