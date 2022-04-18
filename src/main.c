#include <argp.h>
#include <stdio.h>

static const char args_doc[] = "rss_subscriber args_doc";
static const char doc[] = "rss_subscriber doc";
static struct argp_option opts[] = {
	{"subscribe", 's', "url", 0, "Adds a url to the list of xml sources to download from."},
	{"unsubscribe", 'u', "url", 0, "Removes a url to the list of xml sources to download from."},
	{"get-subs", 'g', 0, 0, "Gets the list of urls of xml sources to download from."},
	{ 0 }};
struct argument {
	enum {ADD_URL, REM_URL, GET_URL,} mode;
	char *url;
};
static error_t parse_args(int key, char *arg, struct argp_state *state)
{
	struct argument *args = state->input;
	switch (key) {
	case 's':
		args->mode = ADD_URL;
		args->url = arg;
		break;
	case 'u':
		args->mode = REM_URL;
		args->url = arg;
		break;
	case 'g':
		args->mode = GET_URL;
		break;
	}
	return 0;
}
static struct argp argp = {opts, parse_args, args_doc, doc, 0, 0, 0};

int main(int argc, char **argv)
{
	struct argument args;
	if (!argp_parse(&argp, argc, argv, 0, 0, &args))
		printf("Successfully processsed arguments.\n"); 
	return 0;
}
