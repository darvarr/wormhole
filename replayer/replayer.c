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
 *         This program turns the mote into a Replayer.
 *         It waits continuously for a new byte coming from the serial port.
 *         When a new byte arrives, it checks if it is part of a known sequence:
 *            - Magic Sequence 'Snif' : a frame to be replayed follows:
 *                magic_sequence + frame_length + frame
 *            - Control Sequence: 'ctrl' : a new radio command follows:
 *                control_sequence + 1 --> RADIO ON
 *                control_sequence + 0 --> RADIO OFF
 *         This code is specific for TI Launchpad CC2650 platform
 *         
 * \author
 *         Dario Varano - <dario.varano@ing.unipi.it>
 */
 
#include "contiki.h"
#include "dev/serial-line.h"
#include "dev/cc26xx-uart.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "lib/memb.h"
#include "null_replayerRDC.h"
#include "replayer.h"
#include <stdio.h> /* For printf() */
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 150
#define SEQ_BUFF_SIZE 4

/*---------------------------------------------------------------------------*/
PROCESS(replayer_process, "Replay process");
AUTOSTART_PROCESSES(&replayer_process);
/*---------------------------------------------------------------------------*/

/* Global variables */
static unsigned char buffer[BUFF_SIZE];
static unsigned char seq_buf[SEQ_BUFF_SIZE];
static unsigned int buffer_length;
static int n_frame_recv_bytes = 0;
static const uint8_t magic[] = { 0x53, 0x6E, 0x69, 0x66 }; /* Magic Sequence: Snif */
static const uint8_t control[] = { 0x63, 0x74, 0x72, 0x6C }; /* Control Sequence: ctrl */
static int n_seq = 0;
static int control_flag = 0;
static int magic_flag = 0;
int radio_on = 1;

/* this function just send the frame contained in the buffer */
static void replay(void)
{
	NETSTACK_RADIO.send(buffer, buffer_length);
}

/* New byte available on the serial port */
static int uart_rx_callback(unsigned char c) 
{
	/* Check if the control flag is set */
	if (control_flag == 1)
	{
		/* The next byte will be the command to enable/disable the radio */
		if (c == 0x31)
		{
			NETSTACK_RADIO.on();
			printf("Radio ON!\n");
			radio_on = 1;
		}
		else if (c == 0x30)
		{
			NETSTACK_RADIO.off();
			printf("Radio OFF!\n");
			radio_on = 0;
		}
		control_flag = 0;
		n_seq = 0;
		n_frame_recv_bytes = 0;
	}
	else
	{
		int ret;
		
		/* shift left seq_buf by 1 and put the new char in the last position */
		memcpy(seq_buf, seq_buf+1, SEQ_BUFF_SIZE-1);
		seq_buf[SEQ_BUFF_SIZE-1] = c;
		
		/* check if it's a magic sequence */
		ret = memcmp(seq_buf, magic, SEQ_BUFF_SIZE);
		if ((ret == 0) && (magic_flag == 0))
		{
			/* magic sequence found, set the magic flag */
			magic_flag = 1;
			control_flag = 0;
			return 0;
		}
		else
		{
			/* check if it's a control sequence */
			ret = memcmp(seq_buf, control, SEQ_BUFF_SIZE);
			if (ret == 0)
			{
				/* control sequence found, set the control flag */
				control_flag = 1;
				magic_flag = 0;
			}
		}
		/* Check if the magic flag is set */
		if (magic_flag == 1)
		{
			buffer_length = c;
			magic_flag = 2;
		}
		else if (magic_flag == 2)
		{
			buffer[n_frame_recv_bytes] = c;
			n_frame_recv_bytes++;
			if (n_frame_recv_bytes == buffer_length)
			{
				if (radio_on)
				{
					/* Replay ONLY if the radio is on */
					replay();
				}
				n_frame_recv_bytes = 0;
				magic_flag = 0;
				n_seq = 0;
			}
		}
	}
	return 0;
}

PROCESS_THREAD(replayer_process, ev, data)
{
	PROCESS_BEGIN();
	cc26xx_uart_set_input(uart_rx_callback);

	memset(seq_buf, 0, SEQ_BUFF_SIZE);
	
	while(1) {
		
		PROCESS_YIELD();  
	}
	PROCESS_END();
}
