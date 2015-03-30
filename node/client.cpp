#include "dadvisor.h"

static int
cmd_handler(CManager cm, void *vevent, void *client_data, attr_list attrs)
{
    alloc_info_ptr event = (alloc_info_ptr)vevent;
    printf("Alloc received from: %s => cpu:%f, netin:%f, netout:%f\n",
           event->id, event->cpu_alloc, event->network_in_alloc, event->network_out_alloc);
    return 1;
}

/* this file is evpath/examples/net_send.c */
int main(int argc, char **argv)
{
    CManager cm;
    char string_list[2048];
    attr_list contact_list;
    EVstone remote_stone;

    if(sscanf(argv[1], "%d:%s", &remote_stone, &string_list[0]) != 2) {
	   printf("Bad arguments \"%s\"\n", argv[1]);
	   exit(0);
    }

    cm = CManager_create();
    CMlisten(cm);

    // send stone
    EVstone sstone = EValloc_stone(cm);
    contact_list = attr_list_from_string(string_list);
    EVassoc_bridge_action(cm, sstone, contact_list, remote_stone);

    // receive command stone
    EVstone rstone = EValloc_stone(cm);
    EVassoc_terminal_action(cm, rstone, command_list, cmd_handler, NULL);

    // sending data
    EVsource source = EVcreate_submit_handle(cm, sstone, data_list);
    LProcess lp("client", "eth0", "", atoi(argv[2]));
    lp.setHardCpuShares(100);
    lp.setNetworkInBW(1000);
    lp.setNetworkOutBW(100);

    // sending allocation
    usage_info data;
    data.id = "client";
    data.cpu_usage = 100;
    data.network_in_usage = 1000;
    data.network_out_usage = 100;
    EVsubmit(source, &data, NULL);

    // sending usage
    while(true) {
        sleep(TRIGGER_PERIOD);
        data.cpu_usage = lp.getCpuUsage();
        data.network_in_usage = lp.getNetworkInUsage();
        data.network_out_usage = lp.getNetworkOutUsage();
        EVsubmit(source, &data, NULL);
    }

    return 0;
}
