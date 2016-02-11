#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include <stdio.h>
#include <string.h>

PROCESS(drone_process, "Drone process");
AUTOSTART_PROCESSES(&drone_process);

static struct broadcast_conn bc;
static struct unicast_conn uc;

static void
recv_bc(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d\n", from->u8[0]);
}

static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from)
{
  printf("unicast message %s received from %d\n", 
         (char *)packetbuf_dataptr(), from->u8[0]);
}

static const struct broadcast_callbacks broadcast_callbacks = {recv_bc};
static const struct unicast_callbacks unicast_callbacks = {recv_uc};


PROCESS_THREAD(drone_process, ev, data)
{
  static struct etimer et;
  static char *msg = "Hello";
  static int cnt = 1;

	PROCESS_BEGIN();

  broadcast_open(&bc, 129, &broadcast_callbacks);
  unicast_open(&uc, 146, &unicast_callbacks);

  while(1)
  {
    etimer_set(&et, CLOCK_SECOND + random_rand() % (CLOCK_SECOND));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    packetbuf_copyfrom((char *)&cnt, strlen(msg));
    broadcast_send(&bc);
    printf("broadcast message %d sent.\n", cnt);

    cnt++;
  }

	PROCESS_END();
}
