#ifndef DADVISOR_H
#define DADVISOR_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "evpath.h"

typedef struct _usage_info {
    string id;
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

static FMStructDescRec simple_format_list[] =
{
    {"usage_info", usage_field_list, sizeof(usage_info), NULL},
    {NULL, NULL}
};

#endif
