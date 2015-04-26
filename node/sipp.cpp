#include "dadvisor.h"

static LProcess *lp;

#define AWK_COMMAND "awk -F ';' '\
BEGIN {\
  msize=2;\
  asize=6;\
  array[0]=7;\
  array[1]=15;\
  array[2]=17;\
  array[3]=49;\
  array[4]=57;\
  array[5]=59;\
}\
\
{\
  if(NR==1) {\
    for(i=0;i<asize;i++) {\
      final[i]=$array[i];\
    }\
  } else {\
    for(i=0;i<msize;i++) {\
      if(final[i]>$array[i]) {\
        final[i]=$array[i];\
      }\
    }\
\
    for(i=msize;i<asize;i++) {\
      if(final[i]<$array[i]) {\
        final[i]=$array[i];\
      }\
    }\
  }\
}\
\
END {\
  print NR;\
  print \"t\";\
  for(i=0;i<asize;i++) {\
    print final[i];\
    print \"t\";\
  }\
}\
'"

#define AWK_COMMAND_RTT "awk -F ';' '\
{\
  if(NR==1) {\
    final=$2;\
  } else {\
    if(final<$3) {\
      final=$3;\
    }\
  }\
}\
\
END {\
  print NR,final;\
}\
'"

void getAppMetrics(usage_info& data, char* prefix, char* pid) {
  static int last_line = 3;
  static int last_line_rtt = 3;
  int line;

  // init for Application specific metrics
  stringstream ss;
  ss<<"tail -n +"<<last_line<<" "<<prefix<<"_"<<pid<<"_.csv | ";
  ss<<AWK_COMMAND;
  string result;
  Utils::systemCmd(ss.str(), result);
  sscanf(result.c_str(), "%dt%ft%dt%dt%dt%dt%d", &line, &data.call_rate, &data.successful_calls,
    &data.failed_calls, &data.retransmissions, &data.watchdog_major,
    &data.watchdog_minor);
  last_line += line;

  // rtt file
  ss.str("");
  result.clear();
  ss<<"tail -n +"<<last_line_rtt<<" "<<prefix<<"_"<<pid<<"_rtt.csv | ";
  ss<<AWK_COMMAND_RTT;
  Utils::systemCmd(ss.str(), result);
  sscanf(result.c_str(), "%d %f", &line, &data.max_response_time);
  last_line_rtt += line;
}

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
    printf("usage: %s <master-contact-list> <pid> <cpu-usage> <net-usage> <scenario>\n", argv[0]);
    return -1;
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
  lp->setHardCPUShares(atoi(argv[3]));
  lp->setNetworkOutBW(atoi(argv[4]));

  // sending usage
  usage_info data;
  char *str = attr_list_to_string(CMget_contact_list(cm));
  data.id = new char[strlen(str)+6];
  sprintf(data.id, "%d:%s", rstone, str);

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

    // getting other application specific matrics
    getAppMetrics(data, argv[5], argv[2]);

    printf("Current usage: callrate-%2f\tcpu-%2f\tnet-%2f\n",
      data.call_rate, data.cpu_usage, data.network_out_usage);
    EVsubmit(source, &data, NULL);
  }

  return 0;
}
