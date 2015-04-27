#include "dadvisor.h"

static int cmd_handler(CManager cm, void *vevent, void *client_data,
    attr_list attrs) {
  alloc_info_ptr event = (alloc_info_ptr)vevent;

  /* -------- @todo hard code --------- */
  if(strcmp(event->id, "begin") == 0) {
    assert(0==system("sudo iptables -t nat -D PREROUTING -d 192.168.111.128 -s 192.168.111.161 -j DNAT --to-destination 192.168.111.124"));
    assert(0==system("sudo iptables -t nat -D POSTROUTING -s 192.168.111.161 -j SNAT --to-source 192.168.111.128"));
    assert(0==system("sudo iptables -t nat -A PREROUTING -d 192.168.111.128 -s 192.168.111.161 -j DNAT --to-destination 192.168.111.125"));
    assert(0==system("sudo iptables -t nat -I POSTROUTING -s 192.168.111.161 -j SNAT --to-source 192.168.111.128"));
    assert(0==system("sudo iptables -A FORWARD -d 192.168.111.125 -i eth0 -j ACCEPT"));
  } else {
    assert(0==system("sudo iptables -t nat -D PREROUTING -d 192.168.111.128 -s 192.168.111.161 -j DNAT --to-destination 192.168.111.125"));
    assert(0==system("sudo iptables -t nat -D POSTROUTING -s 192.168.111.161 -j SNAT --to-source 192.168.111.128"));
    assert(0==system("sudo iptables -D FORWARD -d 192.168.111.125 -i eth0 -j ACCEPT"));
    assert(0==system("sudo iptables -t nat -A PREROUTING -d 192.168.111.128 -s 192.168.111.161 -j DNAT --to-destination 192.168.111.124"));
    assert(0==system("sudo iptables -t nat -I POSTROUTING -s 192.168.111.161 -j SNAT --to-source 192.168.111.128"));
  }
  /* ---------------------------------- */

  return 1;
}

int main(int argc, char **argv) {
  if(argc < 3) {
    printf("usage: %s <id> <master-contact-list>\n", argv[0]);
    return -1;
  }

  CManager cm = CManager_create();
  CMlisten(cm);

  // parsing the master contact list
  char string_list[2048];
  EVstone remote_stone;
  if(sscanf(argv[2], "%d:%s", &remote_stone, &string_list[0]) != 2) {
    printf("Bad arguments \"%s\"\n", argv[2]);
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

  // sending usage
  usage_info data;
  char *str = attr_list_to_string(CMget_contact_list(cm));
  data.contact_list = new char[strlen(str)+6];
  sprintf(data.contact_list, "%d:%s", rstone, str);
  data.id = argv[1];
  data.cpu_usage = -1;
  data.network_out_usage = -1;
  EVsubmit(source, &data, NULL);
  delete[] data.contact_list;

  CMrun_network(cm);
  return 0;
}
