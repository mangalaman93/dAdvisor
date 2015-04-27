#include "dadvisor.h"

static int cmd_handler(CManager cm, void *vevent, void *client_data,
    attr_list attrs) {
  alloc_info_ptr event = (alloc_info_ptr)vevent;

  if(strcmp(event->id, "begin") == 0) {
    /* -------- @todo hard code --------- */
    /* ---------------------------------- */
  }

  return 1;
}

int main(int argc, char **argv) {
  if(argc < 2) {
    printf("usage: %s <master-contact-list>\n", argv[0]);
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

  // sending usage
  usage_info data;
  char *str = attr_list_to_string(CMget_contact_list(cm));
  data.id = new char[strlen(str)+6];
  sprintf(data.id, "%d:%s", rstone, str);
  data.cpu_usage = -1;
  data.network_out_usage = -1;
  EVsubmit(source, &data, NULL);
  delete[] data.id;

  CMrun_network(cm);
  return 0;
}
