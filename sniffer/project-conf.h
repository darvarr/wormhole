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
 *         Project specific configuration defines for the sniffer example
 *         for TI Launchpad CC2650.
 *
 * \author
 *         Dario Varano - <dario.varano@ing.unipi.it>
 */

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_


#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC      stubrdc_driver
#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC      nullmac_driver

#undef IEEE_MODE_CONF_AUTOACK
#define IEEE_MODE_CONF_AUTOACK 0
#undef IEEE_MODE_CONF_PROMISCOUS
#define IEEE_MODE_CONF_PROMISCOUS 1

/*---------------------------------------------------------------------------*/
/* Disable button shutdown functionality */
#define BUTTON_SENSOR_CONF_ENABLE_SHUTDOWN    0
/*---------------------------------------------------------------------------*/
/* Enable the ROM bootloader */
#define ROM_BOOTLOADER_ENABLE                 1
/*---------------------------------------------------------------------------*/
/* Change to match your configuration */
#define RF_CORE_CONF_CHANNEL                 26
/*---------------------------------------------------------------------------*/

#endif /* PROJECT_CONF_H_ */
