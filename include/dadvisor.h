#ifndef DADVISOR_H
#define DADVISOR_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <map>
#include "config.h"
#include "evpath.h"
#include "lprocess.h"

typedef struct _usage_info {
    char* id;
    double cpu_usage;
    double network_in_usage;
    double network_out_usage;
} usage_info, *usage_info_ptr;

static FMField usage_field_list[] =
{
    {"id", "string", sizeof(char*), FMOffset(usage_info_ptr, id)},
    {"cpu_usage", "double", sizeof(double), FMOffset(usage_info_ptr, cpu_usage)},
    {"network_in_usage", "double", sizeof(double), FMOffset(usage_info_ptr, network_in_usage)},
    {"network_out_usage", "double", sizeof(double), FMOffset(usage_info_ptr, network_out_usage)},
    {NULL, NULL, 0, 0}
};

static FMStructDescRec data_list[] =
{
    {"usage_info", usage_field_list, sizeof(usage_info), NULL},
    {NULL, NULL}
};

typedef struct _alloc_info {
    char* id;
    double cpu_alloc;
    double network_in_alloc;
    double network_out_alloc;
} alloc_info, *alloc_info_ptr;

// allocations are set to -1 if unchanged
static FMField alloc_field_list[] =
{
    {"id", "string", sizeof(char*), FMOffset(alloc_info_ptr, id)},
    {"cpu_alloc", "double", sizeof(double), FMOffset(alloc_info_ptr, cpu_alloc)},
    {"network_in_alloc", "double", sizeof(double), FMOffset(alloc_info_ptr, network_in_alloc)},
    {"network_out_alloc", "double", sizeof(double), FMOffset(alloc_info_ptr, network_out_alloc)},
    {NULL, NULL, 0, 0}
};

static FMStructDescRec command_list[] =
{
    {"alloc_info", alloc_field_list, sizeof(alloc_info), NULL},
    {NULL, NULL}
};

#endif
