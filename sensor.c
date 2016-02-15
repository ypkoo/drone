#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>
#include "sys/node-id.h"
#include "drone.h"

PROCESS(sensor_process, "Sensor process");
AUTOSTART_PROCESSES(&sensor_process);

static struct broadcast_conn bc;
static struct unicast_conn uc;

static struct ctimer ct;
static linkaddr_t drone_addr;

static char msg_to_send[MSG_LEN];

static void
sensor_init()
{
	sprintf(msg_to_send, "Hello. I'm %d", node_id);
	drone_addr.u8[0] = 1;
	drone_addr.u8[1] = 0;
	srand(node_id);
}

static void
send_data_to_drone(const linkaddr_t *from)
{ 
  packetbuf_copyfrom(msg_to_send, strlen(msg_to_send));
  unicast_send(&uc, &drone_addr);
}

static void
recv_bc(struct broadcast_conn *c, const linkaddr_t *from)
{
	struct drone_msg *msg = (struct drone_msg *) packetbuf_dataptr();
	int i;

  for(i=0; i<NODE_NUM; i++)
    printf("%d ", msg->received_ids[i]);
  printf("\n");

	for(i=0; i<NODE_NUM; i++)
	{
		if(msg->received_ids[i] == node_id)
			return;
	}

  printf("broadcast message received from %d\n", 
           from->u8[0]);
  //drone_addr = *from;

  ctimer_set(&ct, (random_rand() & CLOCK_SECOND) * 0.1, send_data_to_drone, from);
}

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{

}

static const struct broadcast_callbacks broadcast_call = {recv_bc};
static const struct unicast_callbacks unicast_call = {recv_uc};

PROCESS_THREAD(sensor_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();

  broadcast_open(&bc, 129, &broadcast_call);
  unicast_open(&uc, 146, &unicast_call);

  sensor_init();

  while(1)
  {
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_END();
}

