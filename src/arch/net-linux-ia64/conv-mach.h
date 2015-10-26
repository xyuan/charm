#ifndef _CONV_MACH_H
#define _CONV_MACH_H


#define CMK_IA64                                           1

#define CMK_ASYNC_NOT_NEEDED                               0
#define CMK_ASYNC_USE_FIOASYNC_AND_FIOSETOWN               0
#define CMK_ASYNC_USE_FIOASYNC_AND_SIOCSPGRP               0
#define CMK_ASYNC_USE_FIOSSAIOSTAT_AND_FIOSSAIOOWN         0
#define CMK_ASYNC_USE_F_SETFL_AND_F_SETOWN                 1

#define CMK_GETPAGESIZE_AVAILABLE                          1

#define CMK_MALLOC_USE_GNU_MALLOC                          0
#define MORECORE_CANNOT_TRIM
#define CMK_MALLOC_USE_OS_BUILTIN                          1
#define CMK_MALLOC_USE_GNUOLD_MALLOC                       0

#define CMK_MEMORY_PAGESIZE                                16384
#define CMK_MEMORY_PROTECTABLE                             1


#define CMK_SSH_IS_A_COMMAND                               1
#define CMK_SSH_NOT_NEEDED                                 0

#define CMK_SHARED_VARS_UNAVAILABLE                        1
#define CMK_SHARED_VARS_UNIPROCESSOR                       0

#define CMK_THREADS_USE_CONTEXT                            1
#define CMK_THREADS_USE_PTHREADS                           0
#define CMK_THREADS_ARE_WIN32_FIBERS                       0

#define CMK_SIGNAL_NOT_NEEDED                              0
#define CMK_SIGNAL_USE_SIGACTION                           0
#define CMK_SIGNAL_USE_SIGACTION_WITH_RESTART              1

#define CMK_THREADS_REQUIRE_NO_CPV                         0
#define CMK_THREADS_COPY_STACK                             0

#define CMK_TIMER_USE_RDTSC                                1
#define CMK_TIMER_USE_GETRUSAGE                            0
#define CMK_TIMER_USE_SPECIAL                              0
#define CMK_TIMER_USE_TIMES                                0

#define CMK_64BIT    1

#define CMK_NETPOLL                                        1

#define CMK_WHEN_PROCESSOR_IDLE_BUSYWAIT                   1
#define CMK_WHEN_PROCESSOR_IDLE_USLEEP                     0


#define CMK_DEBUG_MODE					   0 
#define CMK_WEB_MODE                                       1

#define CMK_LBDB_ON					   1


#endif

