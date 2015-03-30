#include "dadvisor.h"

static LProcess *lp;

static int
cmd_handler(CManager cm, void *vevent, void *client_data, attr_list attrs)
{
    alloc_info_ptr event = (alloc_info_ptr)vevent;
    printf("New Alloc: %s => %2f, %2f, %2f\n",
           event->id, event->cpu_alloc, event->network_in_alloc, event->network_out_alloc);

    if(event->cpu_alloc > 0) {
        lp->setHardCpuShares(event->cpu_alloc);
    }

    if(event->network_in_alloc > 0) {
        lp->setNetworkInBW(event->network_in_alloc);
    }

    if(event->network_out_alloc > 0) {
        lp->setNetworkOutBW(event->network_out_alloc);
    }

    return 1;
}

/* this file is evpath/examples/net_send.c */
int main(int argc, char **argv)
{
    CManager cm = CManager_create();
    CMlisten(cm);

    // parsing the master contact list
    char string_list[2048];
    EVstone remote_stone;
    if(sscanf(argv[1], "%d:%s", &remote_stone, &string_list[0]) != 2) {
	   printf("Bad arguments \"%s\"\n", argv[1]);
	   exit(0);
    }

    // send stone
    EVstone sstone = EValloc_stone(cm);
    attr_list contact_list = attr_list_from_string(string_list);
    EVassoc_bridge_action(cm, sstone, contact_list, remote_stone);

    // receive command stone
    EVstone rstone = EValloc_stone(cm);
    EVassoc_terminal_action(cm, rstone, command_list, cmd_handler, NULL);

    // sending allocation
    EVsource source = EVcreate_submit_handle(cm, sstone, data_list);
    lp = new LProcess("client", "eth0", "", atoi(argv[2]));
    lp->setHardCpuShares(100);
    lp->setNetworkInBW(1000);
    lp->setNetworkOutBW(100);

    // sending usage
    usage_info data;
    char *str = attr_list_to_string(CMget_contact_list(cm));
    data.id = new char[strlen(str)+2];
    sprintf(data.id, "%d:%s", rstone, string_list);
    data.cpu_usage = 100;
    data.network_in_usage = 1000;
    data.network_out_usage = 100;
    EVsubmit(source, &data, NULL);

    // sending usage
    while(true) {
        sleep(TRIGGER_PERIOD);
        data.cpu_usage = lp->getCpuUsage();
        data.network_in_usage = lp->getNetworkInUsage();
        data.network_out_usage = lp->getNetworkOutUsage();
        printf("Current Usage: %s => %2f, %2f, %2f\n",
           data.id, data.cpu_usage, data.network_in_usage, data.network_out_usage);
        EVsubmit(source, &data, NULL);
    }

    return 0;
}
