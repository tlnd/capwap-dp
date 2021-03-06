/*
 * Copyright (C) 2014-2017, Travelping GmbH <info@travelping.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __LOG_H
#define __LOG_H

#include <string.h>
#define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define PRIsMAC "%02x:%02x:%02x:%02x:%02x:%02x"
#define ARGsMAC(m) (m)[0], (m)[1], (m)[2], (m)[3], (m)[4], (m)[5]

#ifdef USE_SYSTEMD_JOURNAL

#include <systemd/sd-journal.h>

#define _log_XSTRINGIFY(x) #x
#define _log_STRINGIFY(x) _log_XSTRINGIFY(x)

#define ALLOCA_CODE_FILE(f, file)                 \
	do {                                      \
		size_t _fl;                       \
		const char *_file = (file);       \
		char **_f = &(f);                 \
		_fl = strlen(_file) + 1;          \
		*_f = alloca(_fl + 10);           \
		memcpy(*_f, "CODE_FILE=", 10);    \
		memcpy(*_f + 10, _file, _fl);     \
	} while(0)

#define log(priority, ...)						\
	do {								\
		char *f;						\
		ALLOCA_CODE_FILE(f, __FILE_NAME__);			\
		sd_journal_print_with_location(priority, f, "CODE_LINE=" _log_STRINGIFY(__LINE__), __func__, __VA_ARGS__); \
	} while (0)

#else

#include <syslog.h>

#define log(priority, ...)	syslog(priority, __VA_ARGS__)

#endif

#if defined(DEBUG)

#define debug(...)						\
	_debug(__FILE_NAME__, __LINE__, __func__,__VA_ARGS__)
#define debug_head(tv)						\
	_debug_head(__FILE_NAME__, __LINE__, __func__, tv)

void debug_log(const char *fmt, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
void debug_flush(void);

void _debug(const char *filename, int line, const char *func, const char *fmt, ...)
	__attribute__ ((__format__ (__printf__, 4, 5)));
void _debug_head(const char *filename, int line, const char *func, struct timeval *);

#define hexdump(buf, len)					\
	_hexdump(__FILE_NAME__, __LINE__, __func__, buf, len);

void _hexdump(const char *filename, int line, const char *func,
	      const unsigned char *buf, ssize_t len) __attribute__((unused));

#define debug_ei(x)							\
	do {								\
		int type;						\
		int size;						\
		int index = (x)->index;					\
		if (ei_get_type((x)->buff, &index, &type, &size) == 0)	\
			debug("ei type: %d, size: %d", type, size);	\
	} while (0)

#define debug_sockaddr(addr)						\
	do {								\
		char ipaddr[INET6_ADDRSTRLEN];				\
									\
		inet_ntop(((struct sockaddr *)(addr))->sa_family, SIN_ADDR_PTR((struct sockaddr *)((addr))), ipaddr, sizeof(ipaddr)); \
		debug("IP: %s:%d", ipaddr, ntohs(SIN_PORT(((struct sockaddr *)(addr))))); \
									\
	} while (0)

#else

#define debug(format, ...) do {} while (0)

#define debug_head() do {} while (0)
#define debug_log(format, ...) do {} while (0)
#define debug_flush() do {} while (0)
#define debug_ei(x) do {} while (0)
#define debug_sockaddr(addr) do {} while (0)
#define hexdump(buf, len) do {} while (0)

#endif

#endif /* __LOG_H */
