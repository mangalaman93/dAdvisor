#ifndef DADVISOR_H
#define DADVISOR_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include <sys/time.h>
#include "config.h"
#include "evpath.h"
#include "lprocess.h"

/* general usage info struct */
typedef struct _usage_info {
  char* id;
  double cpu_usage;
  double network_out_usage;

  // sipp related info
  double call_rate;
  int retransmissions;
  int failed_calls;
  double max_response_time;
  int watchdog_major;
  int watchdog_minor;

  _usage_info() {
    id = NULL;
    cpu_usage = 0.0;
    network_out_usage = 0.0;
    call_rate = 0.0;
    retransmissions = -1;
    failed_calls = -1;
    max_response_time = 0.0;
    watchdog_major = -1;
    watchdog_minor = -1;
  }
} usage_info, *usage_info_ptr;

static FMField usage_field_list[] = {
  {"id", "string", sizeof(char*), FMOffset(usage_info_ptr, id)},
  {"cpu_usage", "double", sizeof(double), FMOffset(usage_info_ptr, cpu_usage)},
  {"network_out_usage", "double", sizeof(double), FMOffset(usage_info_ptr, network_out_usage)},
  {"call_rate", "double", sizeof(double), FMOffset(usage_info_ptr, call_rate)},
  {"retransmissions", "integer", sizeof(int), FMOffset(usage_info_ptr, retransmissions)},
  {"failed_calls", "integer", sizeof(int), FMOffset(usage_info_ptr, failed_calls)},
  {"max_response_time", "double", sizeof(double), FMOffset(usage_info_ptr, max_response_time)},
  {"watchdog_major", "integer", sizeof(int), FMOffset(usage_info_ptr, watchdog_major)},
  {"watchdog_minor", "integer", sizeof(int), FMOffset(usage_info_ptr, watchdog_minor)},
  {NULL, NULL, 0, 0}
};

static FMStructDescRec usage_data_list[] = {
    {"usage_info", usage_field_list, sizeof(usage_info), NULL},
    {NULL, NULL}
};

/* allocation information related struct */
typedef struct _alloc_info {
    char* id;
    double cpu_alloc;
    double network_out_alloc;
} alloc_info, *alloc_info_ptr;

// allocations are set to -1 if unchanged
static FMField alloc_field_list[] =
{
    {"id", "string", sizeof(char*), FMOffset(alloc_info_ptr, id)},
    {"cpu_alloc", "double", sizeof(double), FMOffset(alloc_info_ptr, cpu_alloc)},
    {"network_out_alloc", "double", sizeof(double), FMOffset(alloc_info_ptr, network_out_alloc)},
    {NULL, NULL, 0, 0}
};

static FMStructDescRec alloc_data_list[] =
{
    {"alloc_info", alloc_field_list, sizeof(alloc_info), NULL},
    {NULL, NULL}
};

#endif
