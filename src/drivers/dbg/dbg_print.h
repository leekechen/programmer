#ifndef __DBG_PRINT_H__
#define __DBG_PRINT_H__

#include <stdio.h>
#include "cmsis_os2.h"

void dbg_init(void);
void dbg_lock(void);
void dbg_unlock(void);

#define _DEBUG     1

#if (_DEBUG)

#if (MODULE_DEBUG)
static int ___module_log_debug_enable = 1;
#else
static int ___module_log_debug_enable = 0;
#endif

#if (MODULE_ERROR)
static int ___module_log_err_eanble   = 1;
#else
static int ___module_log_err_eanble   = 0;
#endif

#if (THREAD_DEBUG)
static int ___thread_log_err_eanble   = 1;
#else
static int ___thread_log_err_eanble   = 0;
#endif


#define print(...)                                                                              \
                                do {                                                            \
                                    if (___module_log_debug_enable) {                           \
                                        dbg_lock();                                             \
                                        printf(__VA_ARGS__);                                    \
                                        dbg_unlock();                                           \
                                    }                                                           \
                                } while (0)


#define dbg_print(...)                                                                          \
                                do {                                                            \
                                    if (___module_log_debug_enable) {                           \
                                        dbg_lock();                                             \
                                        if (___thread_log_err_eanble) {                         \
                                            printf("[Thread:%p] ", osThreadGetId());            \
                                        }                                                       \
                                        printf(__VA_ARGS__);                                    \
                                        dbg_unlock();                                           \
                                    }                                                           \
                                } while (0)

#define dbg_dump(msg, ptr, len)                                                                 \
                                do {                                                            \
                                    if (___module_log_debug_enable) {                           \
                                        dbg_lock();                                             \
                                        int32_t ___i;                                           \
                                        if (___thread_log_err_eanble) {                         \
                                            printf("[Thread:%p] ", osThreadGetId());            \
                                        }                                                       \
                                        printf("%s,DumP: %p[%d]", (msg), (ptr), (len));         \
                                        for (___i = 0; ___i < (len); ___i++) {                  \
                                            if ((___i % 16) == 0) {                             \
                                                printf("\n\t\t\t [0x%08x]\t",___i);             \
                                            }                                                   \
                                            printf("%02x,", (ptr)[___i]);                       \
                                        }                                                       \
                                        printf("\n");                                           \
                                        dbg_unlock();                                           \
                                    }                                                           \
                                } while (0)


#define err_print(...)                                                                          \
                                do {                                                            \
                                    if (___module_log_err_eanble) {                             \
                                        dbg_lock();                                             \
                                        fprintf(stderr, "[Thread:%p] %s(),%d,Error,", osThreadGetId(), __func__, __LINE__);  \
                                        fprintf(stderr, __VA_ARGS__);                           \
                                        dbg_unlock();                                           \
                                    }                                                           \
                                } while (0)

 #define handle_check(h,l,s,e)                                                                  \
                                do {                                                            \
                                   if (___module_log_err_eanble) {                              \
                                      dbg_lock();                                               \
                                      int32_t ___i;                                             \
                                      for (___i = 0; ___i < (sizeof(l)/sizeof(s)); ___i++ ) {   \
                                          if ((h) == (l)[___i]) {                               \
                                              break;                                            \
                                          }                                                     \
                                      }                                                         \
                                      if (___i == (sizeof(l)/sizeof(s))) {                      \
                                          printf("[Thread:%p] %s(),%dInvalid handle!handle=%p,valid[%p~0x%p]\n",   \
                                                         osThreadGetId(), __func__, __LINE__, h, &(l)[0], (l)[___i-1]);             \
                                          dbg_unlock();                                          \
                                          return e;                                             \
                                      }                                                         \
                                      dbg_unlock();                                              \
                                    }                                                           \
                                }while (0)

#else

#define print(...)
#define dbg_print(...)
#define dbg_dump(msg, ptr, len)
#define err_print(...)
#define handle_check(h,l,s,e)

#endif /*DEBUG == 0*/

void dbg_init(void);


#endif /* __DBG_PRINT_H__ */

