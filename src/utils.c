#include "utils.h"

bool is_parent_view(wlc_handle p, wlc_handle view) {
	if (p == 0 || view == 0)
		return false;
	wlc_handle parent = wlc_view_get_parent(view);
	if (parent == 0)
		return false;
	return parent == p || is_parent_view(p, parent);
}

static const char* lfatal = "FATAL";
static const char* lerror = "ERROR";
static const char* lwarn = "WARN";
static const char* ldebug = "DEBUG";
static const char* ltrace = "TRACE";
static const char* linfo = "INFO";

const char* logging_type_str(enum logging_type type) {
	switch (type) {
	case FATAL: return lfatal;
	case ERROR: return lerror;
	case WARN: return lwarn;
	case DEBUG: return ldebug;
	case TRACE: return ltrace;
	default:
		break;
	}
	return linfo;
}

void logger_callback(enum wlc_log_type type, const char *str) {
	if (type == WLC_LOG_INFO)
		LOG_INFO(str);
	else if (type == WLC_LOG_WARN)
		LOG_WARN(str);
	else if (type == WLC_LOG_ERROR)
		LOG_ERROR(str);
	else if (type == WLC_LOG_WAYLAND)
		LOG_INFO(str);
	else {
		LOG_DEBUG(str);
	}
}
