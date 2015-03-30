#include "dadvisor.h"

static int
simple_handler(CManager cm, void *vevent, void *client_data, attr_list attrs)
{
    usage_info_ptr event = vevent;
    printf("Usage received from: %s => cpu:%f, netin:%f, netout:%f\n",
           event->id, event->cpu_usage, event->network_in_usage, event->network_out_usage);
    return 1;
}

/* this file is evpath/examples/net_recv.c */
int main(int argc, char **argv)
{
    CManager cm;
    EVstone stone;
    char *string_list;

    cm = CManager_create();
    CMlisten(cm);

    stone = EValloc_stone(cm);
    EVassoc_terminal_action(cm, stone, usage_field_list, simple_handler, NULL);
    string_list = attr_list_to_string(CMget_contact_list(cm));
    printf("Contact list \"%d:%s\"\n", stone, string_list);
    CMrun_network(cm);
    return 0;
}
