/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
 * All rights reserved.
 *
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
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Definition of a fake RDC driver to be used with passive
 *         examples. The sniffer will never send packets and it will never
 *         push incoming packets up the stack. Received packets are sent to the 
 *         serial port for the sniffer. We do this by defining this
 *         driver as our RDC. We then drop everything.
 *         This code is specific for CC2650 Launchpad platform.
 *
 * \author
 *         George Oikonomou - <oikonomou@users.sourceforge.net>
 *         Carlo Vallati - <c.vallati@iet.unipi.it>
 *         Dario Varano - <dario.varano@ing.unipi.it>
 */

#include <stdio.h>
#include "net/mac/mac.h"
#include "net/mac/rdc.h"
#include "net/rime/rime.h"
#include "net/netstack.h"

#include "dev/serial-line.h"
#include "dev/uart1.h"
#define UART_WRITEB(b) cc26xx_uart_write_byte(b)

static const uint8_t magic[] = { 0x53, 0x6E, 0x69, 0x66 }; /* Snif */

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
  int original_datalen, i;
  uint8_t *original_dataptr;
  signed char last_rssi;
  uint8_t last_correlation;

  last_rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  last_correlation = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);

  original_datalen = packetbuf_datalen();
  original_dataptr = packetbuf_dataptr();

  // Write magic sequence
  UART_WRITEB(magic[0]);
  UART_WRITEB(magic[1]);
  UART_WRITEB(magic[2]);
  UART_WRITEB(magic[3]);
  UART_WRITEB(original_datalen);

  // Write paket
  for( i = 0 ; i < original_datalen ; ++i )
  {
	UART_WRITEB(((unsigned char*)(original_dataptr))[i]);
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
const struct rdc_driver stubrdc_driver = {
    "stubrdc",
    init,
    send,
    send_list,
    input,
    on,
    off,
    cca,
};
/*---------------------------------------------------------------------------*/
