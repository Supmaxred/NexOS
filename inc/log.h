#include <stdio.h>
#include "serialport.h"

#define LOG_TYPE 2

#define LOG_0 (void)
#define LOG_1 sp_printf
#define LOG_2 printf

#if LOG_TYPE == 0
#define LOG     LOG_0
#define LOGDBG  LOG_0
#endif

#if LOG_TYPE == 1
#define LOG     LOG_1
#define LOGDBG  LOG_1
#endif

#if LOG_TYPE == 2
#define LOG     LOG_2
#define LOGDBG  LOG_1
#endif

#define LOG_STEP_START(str) LOG("- %s ", str)
#define LOG_STEP_END() LOG("[ OK ]\n")