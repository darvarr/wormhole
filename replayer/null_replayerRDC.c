/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 
/**
 * \file
 *         Definition of a null RDC driver to be used with the Replayer mote.
 *         The replayer is in promiscuos mode. When it receives a frame, if the
 *         macro to forge ACKs is set, the radio is on and the sender address is in the victims list,
 *         it sends an ACK for the incoming frame. We then drop everything.
 *         This code is specific for CC2650 Launchpad platform.
 *
 * \author
 *         Dario Varano - <dario.varano@ing.unipi.it>
 */
 
#include <stdio.h>
#include "net/mac/mac.h"
#include "net/mac/rdc.h"
#include "net/rime/rime.h"
#include "net/netstack.h"
#include "sys/rtimer.h"
#include "null_replayerRDC.h"
#include "replayer.h"

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifdef NULLREPLAYERRDC_CONF_FORGE_802154_ACK
#define NULLREPLAYERRDC_FORGE_802154_ACK NULLREPLAYERRDC_CONF_FORGE_802154_ACK
#else /* NULLREPLAYERRDC_CONF_FORGE_802154_ACK */
#define NULLREPLAYERRDC_FORGE_802154_ACK 0
#endif /* NULLREPLAYERRDC_CONF_FORGE_802154_ACK */

#define ACK_LEN 3

/* Victim nodes to ack */
static const linkaddr_t victim_node8 = { { 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08 } };
static const linkaddr_t victim_node12 = { { 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c } };
static const linkaddr_t victim_node7 = { { 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07 } };
static const linkaddr_t victim_node3 = { { 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 } };
static const linkaddr_t victim_node14 = { { 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e } };
static const linkaddr_t victim_node16 = { { 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10 } };
static const linkaddr_t victim_node19 = { { 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13 } };
int radio_on;

/*---------------------------------------------------------------------------*/
static void
send(mac_callback_t sent, void *ptr)
{
  if(sent) {
    sent(ptr, MAC_TX_OK, 1);
  }
}
/*---------------------------------------------------------------------------*/
static void
send_list(mac_callback_t sent, void *ptr, struct rdc_buf_list *list)
{
  if(sent) {
    sent(ptr, MAC_TX_OK, 1);
  }
}
/*---------------------------------------------------------------------------*/
static void
input(void)
{
/* If the following flag is set, the mote will ack messages coming from a specific node (victim_node) */ 
#if NULLREPLAYERRDC_FORGE_802154_ACK
	int original_datalen;
	uint8_t *original_dataptr;
	original_datalen = packetbuf_datalen();
	original_dataptr = packetbuf_dataptr();
	{
	frame802154_t info154;
	frame802154_parse(original_dataptr, original_datalen, &info154);

	if(info154.fcf.frame_type == FRAME802154_DATAFRAME &&
		info154.fcf.ack_required != 0 &&
		(linkaddr_cmp((linkaddr_t *)&info154.src_addr, &victim_node8) ||
		 linkaddr_cmp((linkaddr_t *)&info154.src_addr, &victim_node12) ||
		 linkaddr_cmp((linkaddr_t *)&info154.src_addr, &victim_node14) ||
		 linkaddr_cmp((linkaddr_t *)&info154.src_addr, &victim_node16) ||
		 linkaddr_cmp((linkaddr_t *)&info154.src_addr, &victim_node7) ||
		 linkaddr_cmp((linkaddr_t *)&info154.src_addr, &victim_node3) ||
		 linkaddr_cmp((linkaddr_t *)&info154.src_addr, &victim_node19)) &&
		(original_datalen > 3))
		{
			uint8_t ackdata[ACK_LEN] = {0, 0, 0};
			ackdata[0] = FRAME802154_ACKFRAME;
			ackdata[1] = 0;
			ackdata[2] = info154.seq;
			if (radio_on)
			{
				NETSTACK_RADIO.send(ackdata, ACK_LEN);
			}
		}
	}
#endif /* NULLREPLAYERRDC_FORGE_802154_ACK */
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  return NETSTACK_RADIO.on();
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_radio_on)
{
  if(keep_radio_on) {
    return NETSTACK_RADIO.on();
  } else {
    return NETSTACK_RADIO.off();
  }
}
/*---------------------------------------------------------------------------*/
static unsigned short
cca(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{

  on();

}
/*---------------------------------------------------------------------------*/
const struct rdc_driver nullreplayerrdc_driver = {
    "nullreplayerrdc",
    init,
    send,
    send_list,
    input,
    on,
    off,
    cca,
};
/*---------------------------------------------------------------------------*/
