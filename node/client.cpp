#include "dadvisor.h"

static LProcess *lp;

static int cmd_handler(CManager cm, void *vevent, void *client_data,
    attr_list attrs) {
  alloc_info_ptr event = (alloc_info_ptr)vevent;
  printf("New allocation received: cpu-%2f\tnet-%2f\n",
    event->cpu_alloc, event->network_out_alloc);

  if(event->cpu_alloc > 0) {
    lp->setHardCPUShares(event->cpu_alloc);
  }

  if(event->network_out_alloc > 0) {
    lp->setNetworkOutBW(event->network_out_alloc);
  }

  return 1;
}

int main(int argc, char **argv) {
  if(argc < 6) {
    printf("usage: %s <id> <cpu-usage> <net-usage> <master-contact-list> <pid>\n", argv[0]);
    return -1;
  }

  CManager cm = CManager_create();
  CMlisten(cm);

  // parsing the master contact list
  char string_list[2048];
  EVstone remote_stone;
  if(sscanf(argv[4], "%d:%s", &remote_stone, &string_list[0]) != 2) {
    printf("Bad arguments \"%s\"\n", argv[4]);
    exit(0);
  }

  // send stone
  EVstone sstone = EValloc_stone(cm);
  attr_list contact_list = attr_list_from_string(string_list);
  EVassoc_bridge_action(cm, sstone, contact_list, remote_stone);

  // receive command stone
  EVstone rstone = EValloc_stone(cm);
  EVassoc_terminal_action(cm, rstone, alloc_data_list, cmd_handler, NULL);

  // setting initial allocation
  EVsource source = EVcreate_submit_handle(cm, sstone, usage_data_list);
  lp = new LProcess(argv[1], atoi(argv[5]), "eth0");
  lp->setHardCPUShares(atoi(argv[2]));
  lp->setNetworkOutBW(atoi(argv[3]));

  // sending initial allocations
  usage_info data;
  char *str = attr_list_to_string(CMget_contact_list(cm));
  data.id = new char[strlen(str)+6];
  sprintf(data.id, "%d:%s", rstone, str);
  data.cpu_usage = atoi(argv[2]);
  data.network_out_usage = atoi(argv[3]);
  EVsubmit(source, &data, NULL);

  // sending usage
  struct timeval cpu_time;
  unsigned long cpu_usage = lp->getCPUCumUsage();
  gettimeofday(&cpu_time, NULL);
  struct timeval net_time;
  unsigned long net_usage = lp->getNetworkOutCumUsage();
  gettimeofday(&net_time, NULL);
  while(true) {
    CMsleep(cm, USAGE_CHECK_PERIOD);

    unsigned long old_usage = cpu_usage;
    struct timeval old_time = cpu_time;
    cpu_usage = lp->getCPUCumUsage();
    gettimeofday(&cpu_time, NULL);
    double elapsed_time = (cpu_time.tv_sec - old_time.tv_sec);
    elapsed_time += (cpu_time.tv_usec - old_time.tv_usec) / 1000000.0;
    data.cpu_usage = (cpu_usage-old_usage)/elapsed_time;

    old_usage = net_usage;
    old_time = net_time;
    net_usage = lp->getNetworkOutCumUsage();
    gettimeofday(&net_time, NULL);
    elapsed_time = (net_time.tv_sec - old_time.tv_sec);
    elapsed_time += (net_time.tv_usec - old_time.tv_usec) / 1000000.0;
    data.network_out_usage = (net_usage-old_usage)/elapsed_time;

    printf("Current usage: cpu-%2f\tnet-%2f\n",
      data.cpu_usage, data.network_out_usage);
    EVsubmit(source, &data, NULL);
  }

  return 0;
}
