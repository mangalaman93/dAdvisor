#include "dadvisor.h"
#include "machine.h"

bool cpu_flag;
bool network_flag;
map<string, alloc_info> cinfo;
map<string, EVsource> clients;

static int alloc_handler(CManager cm, void *vevent, void *client_data,
    attr_list attrs) {
  usage_info_ptr event = (usage_info_ptr)vevent;
  map<string, alloc_info>::iterator iter = cinfo.find(event->id);

  if(iter != cinfo.end()) {
    // we know the client and the bridge exists
    alloc_info new_alloc;
    new_alloc.id = "master";

    new_alloc.cpu_alloc = -1;
    if(cpu_flag && (iter->second.cpu_alloc*SAFETY_FACTOR < event->cpu_usage)) {
      new_alloc.cpu_alloc = iter->second.cpu_alloc*(1+INCREMENT_FACTOR);
      iter->second.cpu_alloc = new_alloc.cpu_alloc;
    }

    new_alloc.network_out_alloc = -1;
    if(network_flag && (iter->second.network_out_alloc*SAFETY_FACTOR < event->network_out_usage)) {
      new_alloc.network_out_alloc = iter->second.network_out_alloc*(1+INCREMENT_FACTOR);
      iter->second.network_out_alloc = new_alloc.network_out_alloc;
    }

    EVsubmit(clients[event->id], &new_alloc, NULL);
  } else {
    char string_list[2048];
    attr_list contact_list;
    EVstone remote_stone;

    if(sscanf(event->id, "%d:%s", &remote_stone, &string_list[0]) != 2) {
      printf("Bad arguments \"%s\"\n", event->id);
      return -1;
    }

    // send stone
    EVstone sstone = EValloc_stone(cm);
    contact_list = attr_list_from_string(string_list);
    EVassoc_bridge_action(cm, sstone, contact_list, remote_stone);
    clients[event->id] = EVcreate_submit_handle(cm, sstone, alloc_data_list);

    // new client
    alloc_info client;
    client.id = new char[strlen(event->id)];
    strcpy(client.id, event->id);
    client.cpu_alloc = event->cpu_usage;
    cinfo[event->id] = client;
  }

  return 1;
}

int main(int argc, char **argv) {
  if(argc < 3) {
    printf("usage: %s <cpu flag> <network flag>\n", argv[0]);
    return -1;
  }

  // parsing the cpu flag
  cpu_flag = false;
  if(*argv[1] == '1') {
    cpu_flag = true;
  }

  // parsing the network flag
  network_flag = false;
  if(*argv[2] == '1') {
    network_flag = true;
  }

  // EVpath manager
  CManager cm = CManager_create();
  CMlisten(cm);

  // creating date receiving stone
  EVstone rstone = EValloc_stone(cm);
  EVassoc_terminal_action(cm, rstone, usage_data_list, alloc_handler, NULL);

  // printing master node contact list
  char *string_list = attr_list_to_string(CMget_contact_list(cm));
  fprintf(stderr, "Contact list \"%d:%s\"\n", rstone, string_list);

  // loop
  CMrun_network(cm);
  return 0;
}
