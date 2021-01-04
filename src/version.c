#include <cerver/utils/log.h>

#include "version.h"

// print full pocket version information
void pocket_version_print_full (void) {

	cerver_log_msg ("Test API Version: %s", TEST_VERSION_NAME);
	cerver_log_msg ("Release Date & time: %s - %s", TEST_VERSION_DATE, TEST_VERSION_TIME);
	cerver_log_msg ("Author: %s", TEST_VERSION_AUTHOR);

}

// print the version id
void pocket_version_print_version_id (void) {

	cerver_log_msg ("Test API Version ID: %s", TEST_VERSION);

}

// print the version name
void pocket_version_print_version_name (void) {

	cerver_log_msg ("Test API Version: %s", TEST_VERSION_NAME);

}