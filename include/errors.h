#ifndef _TEST_ERRORS_H_
#define _TEST_ERRORS_H_

#define TEST_ERROR_MAP(XX)							\
	XX(0,	NONE, 				None)				\
	XX(1,	BAD_REQUEST, 		Bad Request)		\
	XX(2,	MISSING_VALUES, 	Missing Values)		\
	XX(3,	BAD_USER, 			Bad User)			\
	XX(4,	NOT_FOUND, 			Not found)			\
	XX(5,	SERVER_ERROR, 		Server Error)

typedef enum TestError {

	#define XX(num, name, string) TEST_ERROR_##name = num,
	TEST_ERROR_MAP (XX)
	#undef XX

} TestError;

extern const char *test_error_to_string (const TestError type);

extern void test_error_send_response (
	const TestError error,
	const struct _HttpReceive *http_receive
);

#endif