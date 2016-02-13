#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include <stdio.h>
#include <string.h>
#include "drone.h"

PROCESS(drone_process, "Drone process");
AUTOSTART_PROCESSES(&drone_process);

static struct broadcast_conn bc;
static struct unicast_conn uc;

static char data_from_sensors[NODE_NUM][MSG_LEN];
static int received_ids[NODE_NUM];
static int receive_cnt;

static void
drone_init()
{
  int i;
  for(i=0; i<NODE_NUM; i++)
  {
      strcpy(data_from_sensors[i], "Not received.");
      received_ids[i] = -1;
  }

  receive_cnt = 0;
}

static void
recv_bc(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d\n", from->u8[0]);
}

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
	printf("unicast message %s received from %d\n", packetbuf_dataptr(), from->u8[0]);

	if(receive_cnt == NODE_NUM)
		return;

  int i;
  for(i=0; i<NODE_NUM; i++)
  {
  	if(received_ids[i] == from->u8[0])
  		return;
  }

  strcpy(data_from_sensors[receive_cnt], packetbuf_dataptr());
  received_ids[receive_cnt] = from->u8[0];
  receive_cnt++;
}

static const struct broadcast_callbacks broadcast_callbacks = {recv_bc};
static const struct unicast_callbacks unicast_callbacks = {recv_uc};


PROCESS_THREAD(drone_process, ev, data)
{
  static struct etimer et;
  int i;

  PROCESS_BEGIN();

  broadcast_open(&bc, 129, &broadcast_callbacks);
  unicast_open(&uc, 146, &unicast_callbacks);

  drone_init();

  while(1)
  {
  	if(receive_cnt == NODE_NUM)
  		break;

    etimer_set(&et, CLOCK_SECOND + random_rand() % (CLOCK_SECOND));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    struct drone_msg drone_msg;
    strcpy(drone_msg.msg, "hello");
    for(i=0; i<NODE_NUM; i++)
    {
    	drone_msg.received_ids[i] = received_ids[i];
    }

    packetbuf_copyfrom(&drone_msg, sizeof(struct drone_msg));
    broadcast_send(&bc);
    printf("broadcast message sent.\n");
  }

  for(i=0; i<NODE_NUM; i++)
  {
  	printf("%s\n", data_from_sensors[i]);
  }

  PROCESS_END();
}
