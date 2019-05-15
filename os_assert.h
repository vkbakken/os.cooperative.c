/*
 * os_assert.h
 *
 *  Created on: May 9, 2019
 *      Author: liemlb
 */

#ifndef OS_ASSERT_H_
#define OS_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void __assert(const char *__expression, const char *__filename, int __line);

#if (ASSERT_LEVEL == 2)
#define __ASSERT(e) ((e) ? (void)0 : __assert(#e, __FILE__, __LINE__))

#define __ASSERT_MSG(e, fmt, ...)             \
	do {                                      \
		if (!(e)) {                           \
			__assert(#e, __FILE__, __LINE__); \
			printf(fmt, __VA_ARGS__);         \
		}                                     \
	} while (false)
#elif (ASSERT_LEVEL == 1)
#define __ASSERT(e) ((e) ? (void)0 : __assert(#e, __FILE__, __LINE__))
#define __ASSERT_MSG(e, fmt, ...) ((void)(e))
#else
#define __ASSERT(e) ((void)(e))
#define __ASSERT_MSG(e, fmt, ...) ((void)(e))
#endif

#ifdef __cplusplus
}
#endif
#endif /* OS_ASSERT_H_ */
