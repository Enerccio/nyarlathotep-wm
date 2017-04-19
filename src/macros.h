#ifndef MACROS_H_
#define MACROS_H_

enum logging_type {
	FATAL, ERROR, WARN, DEBUG, TRACE, INFO
};

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x

#define CAPTURE() "<" __FILE__ ":" STRINGIZE(__LINE__) ">"

extern void platform_dependent_logging(enum logging_type, const char* message, const char* placement);

#ifndef LOG_LEVEL
#define LOG_LEVEL 2
#endif

#if LOG_LEVEL == 0
#define LOG_FATAL(msg) platform_dependent_logging(FATAL, msg, CAPTURE())
#define LOG_ERROR(msg) platform_dependent_logging(ERROR, msg, CAPTURE())
#define LOG_WARN(msg) platform_dependent_logging(WARN, msg, CAPTURE())
#define LOG_DEBUG(msg) platform_dependent_logging(DEBUG, msg, CAPTURE())
#define LOG_TRACE(msg) platform_dependent_logging(TRACE, msg, CAPTURE())
#define LOG_INFO(msg) platform_dependent_logging(INFO, msg, CAPTURE())
#elif LOG_LEVEL == 1
#define LOG_FATAL(msg) platform_dependent_logging(FATAL, msg, CAPTURE())
#define LOG_ERROR(msg) platform_dependent_logging(ERROR, msg, CAPTURE())
#define LOG_WARN(msg) platform_dependent_logging(WARN, msg, CAPTURE())
#define LOG_DEBUG(msg) platform_dependent_logging(DEBUG, msg, CAPTURE())
#define LOG_TRACE(msg)
#define LOG_INFO(msg) platform_dependent_logging(INFO, msg, CAPTURE())
#elif LOG_LEVEL == 2
#define LOG_FATAL(msg) platform_dependent_logging(FATAL, msg, CAPTURE())
#define LOG_ERROR(msg) platform_dependent_logging(ERROR, msg, CAPTURE())
#define LOG_WARN(msg) platform_dependent_logging(WARN, msg, CAPTURE())
#define LOG_DEBUG(msg)
#define LOG_TRACE(msg)
#define LOG_INFO(msg) platform_dependent_logging(INFO, msg, CAPTURE())
#elif LOG_LEVEL == 3
#define LOG_FATAL(msg) platform_dependent_logging(FATAL, msg, CAPTURE())
#define LOG_ERROR(msg) platform_dependent_logging(ERROR, msg, CAPTURE())
#define LOG_WARN(msg) platform_dependent_logging(WARN, msg, CAPTURE())
#define LOG_DEBUG(msg)
#define LOG_TRACE(msg)
#define LOG_INFO(msg)
#else
#define LOG_FATAL(msg) platform_dependent_logging(FATAL, msg, CAPTURE())
#define LOG_ERROR(msg) platform_dependent_logging(ERROR, msg, CAPTURE())
#define LOG_WARN(msg)
#define LOG_DEBUG(msg)
#define LOG_TRACE(msg)
#define LOG_INFO(msg)
#endif

#ifdef FAIL_FAST
#define ASSERT(test, msg)
#else
#define ASSERT(test, msg) do { \
	if (test == 0) { \
		LOG_FATAL(msg " - at " CAPTURE()); \
		abort(); \
	} \
} while (0)
#endif
#define ASSERT_MEM(ptr) ASSERT(ptr, "memory allocation failure")
#define GENERAL_FAILURE(msg) ASSERT(0, msg)

#define NO_RENDER_MASK (0)
#define RENDER_MASK (1)

#define CLAMP(x, y) (x > y ? y : x)

#define RENDER_SELECTED_WINDOW_LIST_OFFSET (5)

#endif /* MACROS_H_ */
