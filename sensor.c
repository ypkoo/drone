#include "contiki.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include <string.h>

PROCESS(sensor_process, "Sensor process");
AUTOSTART_PROCESSES(&sensor_process);

static struct broadcast_conn bc;
static struct unicast_conn uc;

static struct ctimer ct;
static linkaddr_t drone_addr;

static void
send_data_to_drone(const linkaddr_t *from)
{ 
  char data[16];
  sprintf(data, "hello back");

  packetbuf_copyfrom(data, strlen(data));
  unicast_send(&uc, &drone_addr);
}

static void
recv_bc(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d\n", 
           from->u8[0]);
  drone_addr = *from;

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

  while(1)
  {
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_END();
}

