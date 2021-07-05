#ifndef _TEST_ERRORS_H_
#define _TEST_ERRORS_H_

struct _HttpReceive;

#define TEST_ERROR_MAP(XX)							\
	XX(0,	NONE, 				None)				\
	XX(1,	EXISTING_VALUE, 	Existing Value)		\
	XX(2,	BAD_REQUEST, 		Bad Request)		\
	XX(3,	MISSING_VALUES, 	Missing Values)		\
	XX(4,	BAD_USER, 			Bad User)			\
	XX(5,	NOT_FOUND, 			Not Found)			\
	XX(6,	SERVER_ERROR, 		Server Error)

typedef enum TestError {

	#define XX(num, name, string) TEST_ERROR_##name = num,
	TEST_ERROR_MAP (XX)
	#undef XX

} TestError;

extern const char *test_error_to_string (
	const TestError type
);

extern void test_error_send_response (
	const TestError error,
	const struct _HttpReceive *http_receive
);

#endif