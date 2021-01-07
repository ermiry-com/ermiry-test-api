#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <signal.h>

#include <cerver/cerver.h>
#include <cerver/version.h>

#include <cerver/http/http.h>

#include <cerver/utils/log.h>

#include "version.h"

static Cerver *test_api = NULL;

void end (int dummy) {
	
	if (test_api) {
		cerver_stats_print (test_api, false, false);
		cerver_log_msg ("\nHTTP Cerver stats:\n");
		http_cerver_all_stats_print ((HttpCerver *) test_api->cerver_data);
		cerver_log_line_break ();
		cerver_teardown (test_api);
	}

	cerver_end ();

	exit (0);

}

int main (int argc, char const **argv) {

	srand ((unsigned int) time (NULL));

	// register to the quit signal
	(void) signal (SIGINT, end);
	(void) signal (SIGTERM, end);

	// to prevent SIGPIPE when writting to socket
	(void) signal (SIGPIPE, SIG_IGN);

	cerver_init ();

	cerver_version_print_full ();

	cerver_log_line_break ();
	test_version_print_full ();
	cerver_log_line_break ();

	cerver_end ();

	return 0;

}