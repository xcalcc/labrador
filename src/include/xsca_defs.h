/*
 * Copyright (C) 2020-2021 Xcalibyte Limited, Inc.  All Rights Reserved.
 */

//
// ====================================================================
// xsca_defs.h
// ====================================================================
//
// XSCA basic definitions
//

#ifndef XSCA_DEFS_INCLUDED
#define XSCA_DEFS_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#ifdef Is_True_On
// TRACE0: trace without message
# define TRACE0() \
  do { \
    printf("%s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__); \
  } while (0)

// TRACE: trace with message
# define TRACE(fmt, ...) \
  do { \
    printf("%s:%d:%s\n  ", __FILE__, __LINE__, __FUNCTION__); \
    printf(fmt, ##__VA_ARGS__); \
  } while (0)

// DBG_ASSERT: output a warn if cond is false
# define DBG_WARN(cond, fmt, ...) \
  do { \
    if (!(cond)) { \
      printf("warning: %s:%d:%s\n  ", __FILE__, __LINE__, __FUNCTION__); \
      printf(fmt, ##__VA_ARGS__); \
    } \
  } while (0)

// DBG_ASSERT: output a warn and abort if cond is false
# define DBG_ASSERT(cond, fmt, ...) \
  do { \
    if (!(cond)) { \
      printf("error: %s:%d:%s\n  ", __FILE__, __LINE__, __FUNCTION__); \
      printf(fmt, ##__VA_ARGS__); \
      abort(); \
    } \
  } while (0)

#else  // Is_True_On
# define TRACE0()
# define TRACE(fmt, ...)
# define DBG_WARN(cond, fmt, ...)
# define DBG_ASSERT(cond, fmt, ...)
#endif // Is_True_On

#define REL_ASSERT(cond, fmt, ...) \
  do { \
    if (!(cond)) { \
      printf("error: %s:%d: ", __FILE__, __LINE__); \
      printf((fmt), __VA_ARGS); \
      abort(); \
    } \
  } while (0)


#endif  // XSCA_DEFS_INCLUDED
