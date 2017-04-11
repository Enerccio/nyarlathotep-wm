#ifndef MACROS_H_
#define MACROS_H_

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#ifdef FAIL_FAST
#define ASSERT(test, msg)
#else
#define ASSERT(test, msg) do { \
	if (test == 0) { \
		fprintf(stderr, msg " - at " __FILE__ ": " STRINGIZE(__LINE__) "\n"); \
		abort(); \
	} \
} while (0)
#endif
#define ASSERT_MEM(ptr) ASSERT(ptr, "memory allocation failure")
#define GENERAL_FAILURE(msg) ASSERT(0, msg)

#define NO_RENDER_MASK (0)
#define RENDER_MASK (1)

#define RENDER_SELECTED_WINDOW_LIST_OFFSET (5)

#endif /* MACROS_H_ */
