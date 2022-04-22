#include "url_list.h"
#include <argp.h>
#include <stdio.h>

static const char args_doc[] = "rss_subscriber args_doc";
static const char doc[] = "rss_subscriber doc";
static struct argp_option opts[] = {
	{ "subscribe", 's', "url", 0,
	  "Adds a url to the list of xml sources to download from.", 0 },
	{ "unsubscribe", 'u', "url", 0,
	  "Removes a url to the list of xml sources to download from.", 0 },
	{ "get-subs", 'g', 0, 0,
	  "Gets the list of urls of xml sources to download from.", 0 },
	{ 0 }
};

static error_t parse_args(int key, char *arg, struct argp_state *state)
{
	if (!arg) {
		printf("arg is empty.\n");
		return 0;
	}
	FILE *f = 0;
	// TODO: Check if arg is a valid url.
	switch (key) {
	case 's':
		f = get_url_file("a");
		if (!add_url(f, arg))
			fprintf(stdout, "%s is not a valid rss url.\n", arg);
		break;
	case 'u':
		f = get_url_file("r+");
		remove_url(f, arg);
		break;
	case 'g':
		break;
	}
	if (f)
		fclose(f);
	return 0;
}
static struct argp argp = { opts, parse_args, args_doc, doc, 0, 0, 0 };

int main(int argc, char **argv)
{
	if (!argp_parse(&argp, argc, argv, 0, 0, 0))
		printf("Successfully processsed arguments.\n");
	return 0;
}
