#include "dadvisor.h"

static LProcess *lp;

static int cmd_handler(CManager cm, void *vevent, void *client_data,
    attr_list attrs) {
  alloc_info_ptr event = (alloc_info_ptr)vevent;
  printf("%2f\t%2f\n", event->cpu_alloc, event->network_out_alloc);

  // if(event->cpu_alloc > 0) {
  //     lp->setHardCPUShares(event->cpu_alloc);
  // }

  // if(event->network_out_alloc > 0) {
  //   lp->setNetworkOutBW(event->network_out_alloc);
  // }

  return 1;
}

int main(int argc, char **argv) {
  if(argc < 3) {
    printf("usage: %s <master contact list> <pid>\n", argv[0]);
  }

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
  EVassoc_terminal_action(cm, rstone, alloc_data_list, cmd_handler, NULL);

  // sending allocation
  EVsource source = EVcreate_submit_handle(cm, sstone, usage_data_list);
  lp = new LProcess("client", atoi(argv[2]), "eth0");
  // lp->setHardCPUShares(100);
  // lp->setNetworkOutBW(1000);

  // sending usage
  usage_info data;
  char *str = attr_list_to_string(CMget_contact_list(cm));
  data.id = new char[strlen(str)+6];
  sprintf(data.id, "%d:%s", rstone, str);

  // sending usage
  while(true) {
    CMsleep(cm, USAGE_CHECK_PERIOD);
    data.cpu_usage = lp->getCPUCumUsage();
    data.network_out_usage = lp->getNetworkOutCumUsage();
    printf("%2f\t%2f\n", data.cpu_usage, data.network_out_usage);
    EVsubmit(source, &data, NULL);
  }

  return 0;
}
