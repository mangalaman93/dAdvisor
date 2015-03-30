#include "dadvisor.h"

static CManager cm;
static map<string, alloc_info> cinfo;
static map<string, EVsource> clients;

static int
alloc_handler(CManager cm, void *vevent, void *client_data, attr_list attrs)
{
    usage_info_ptr event = (usage_info_ptr)vevent;
    printf("%s => %f, %f, %f\n",
        event->id, event->cpu_usage, event->network_in_usage, event->network_out_usage);

    if(cinfo.count(event->id) > 0)
    {
        // we know the client and the bridge exists
        alloc_info client = cinfo[event->id];
        alloc_info new_alloc;
        new_alloc.id = "master";

        new_alloc.cpu_alloc = -1;
        if(client.cpu_alloc*SAFETY_FACTOR < event->cpu_usage) {
            new_alloc.cpu_alloc = client.cpu_alloc*(1+INCREMENT_FACTOR);
        }

        if(client.network_in_alloc*SAFETY_FACTOR < event->network_in_usage) {
            new_alloc.network_in_alloc = client.network_in_alloc*(1+INCREMENT_FACTOR);
        }

        if(client.network_out_alloc*SAFETY_FACTOR < event->network_out_usage) {
            new_alloc.network_out_alloc = client.network_out_alloc*(1+INCREMENT_FACTOR);
        }

        EVsubmit(clients[event->id], &new_alloc, NULL);
    } else
    {
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
        printf("trying to connect to %d:%s\n", remote_stone, string_list);
        clients[event->id] = EVcreate_submit_handle(cm, sstone, command_list);

        // new client
        alloc_info client;
        client.id = new char[strlen(event->id)];
        strcpy(client.id, event->id);
        client.cpu_alloc = event->cpu_usage;
        client.network_in_alloc = event->network_in_usage;
        client.network_out_alloc = event->network_in_usage;
        cinfo[event->id] = client;
    }

    return 1;
}

int main(int argc, char **argv)
{
    cm = CManager_create();
    CMlisten(cm);

    EVstone rstone = EValloc_stone(cm);
    EVassoc_terminal_action(cm, rstone, data_list, alloc_handler, NULL);
    char *string_list = attr_list_to_string(CMget_contact_list(cm));
    printf("Contact list \"%d:%s\"\n", rstone, string_list);
    CMrun_network(cm);
    return 0;
}
