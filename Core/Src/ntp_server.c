/*
 * ntp_server.c
 *
 *  Created on: Jul 16, 2026
 *      Author: katev
 */

#include "ntp_server.h"

#include <string.h>

#include "socket.h"
#include "time.h"
#include "w5500.h"
#include "wizchip_conf.h"

#define NTP_SOCKET             0
#define NTP_PORT               123

#define NTP_PACKET_SIZE        48

#define NTP_LI_NO_WARNING      0
#define NTP_VERSION            4
#define NTP_MODE_SERVER        4

#define NTP_STRATUM_PRIMARY    1
#define NTP_POLL_DEFAULT       4
#define NTP_PRECISION_DEFAULT  (-20)

enum
{
    NTP_ROOT_DELAY     = 4,
    NTP_ROOT_DISP      = 8,
    NTP_REFERENCE_ID   = 12,

    NTP_REF_TS         = 16,
    NTP_ORIG_TS        = 24,
    NTP_RECV_TS        = 32,
    NTP_TX_TS          = 40
};

static uint8_t rx_buffer[NTP_PACKET_SIZE];
static uint8_t tx_buffer[NTP_PACKET_SIZE];

static wiz_NetInfo net_info =
{
    .mac  = {0x00,0x08,0xDC,0x11,0x22,0x33},
    .ip   = {192,168,0,123},
    .sn   = {255,255,255,0},
    .gw   = {192,168,1,1},
    .dns  = {8,8,8,8},
    .dhcp = NETINFO_STATIC
};

static inline void put_u32(uint8_t *buf, uint32_t value)
{
    buf[0] = (uint8_t)(value >> 24);
    buf[1] = (uint8_t)(value >> 16);
    buf[2] = (uint8_t)(value >> 8);
    buf[3] = (uint8_t)value;
}

static void build_ntp_response(void)
{
    uint32_t sec;
    uint32_t frac;

    memset(tx_buffer, 0, sizeof(tx_buffer));

    /*
     * LI = 0
     * Version = 4
     * Mode = Server
     */
    tx_buffer[0] =
          (NTP_LI_NO_WARNING << 6)
        | (NTP_VERSION << 3)
        | NTP_MODE_SERVER;

    /*
     * Stratum
     */
    tx_buffer[1] = NTP_STRATUM_PRIMARY;

    /*
     * Poll interval
     */
    tx_buffer[2] = NTP_POLL_DEFAULT;

    /*
     * Precision
     */
    tx_buffer[3] = (uint8_t)NTP_PRECISION_DEFAULT;

    /*
     * Root Delay
     */
    put_u32(&tx_buffer[NTP_ROOT_DELAY], 0);

    /*
     * Root Dispersion
     */
    put_u32(&tx_buffer[NTP_ROOT_DISP], 0);

    /*
     * Reference ID = GPS
     */
    tx_buffer[NTP_REFERENCE_ID + 0] = 'G';
    tx_buffer[NTP_REFERENCE_ID + 1] = 'P';
    tx_buffer[NTP_REFERENCE_ID + 2] = 'S';
    tx_buffer[NTP_REFERENCE_ID + 3] = 0;

    /*
     * Take current timestamp once.
     */
    sec = get_ntp_seconds();
    frac = get_ntp_fraction();

    /*
     * Reference Timestamp
     */
    put_u32(&tx_buffer[NTP_REF_TS], sec);
    put_u32(&tx_buffer[NTP_REF_TS + 4], frac);

    /*
     * Originate Timestamp
     *
     * Copy client's Transmit Timestamp.
     */
    memcpy(
        &tx_buffer[NTP_ORIG_TS],
        &rx_buffer[NTP_TX_TS],
        8
    );

    /*
     * Receive Timestamp
     */
    put_u32(&tx_buffer[NTP_RECV_TS], sec);
    put_u32(&tx_buffer[NTP_RECV_TS + 4], frac);
}

void ntp_server_init(void)
{
    uint8_t txsize[8] = {2,0,0,0,0,0,0,0};
	uint8_t rxsize[8] = {2,0,0,0,0,0,0,0};

	if (wizchip_init(txsize, rxsize) != 0)
	{
		  // ошибка
		Error_Handler(); // нужно ли вызывать так жестко
	}

	uint8_t version = getVERSIONR();

	if(version != 0x04){
		Error_Handler();
	}

	wizchip_setnetinfo(&net_info);

    socket(
        NTP_SOCKET,
        Sn_MR_UDP,
        NTP_PORT,
        0
    );

    getSn_SR(NTP_SOCKET);
}

void ntp_server_process(void)
{
    uint8_t ip[4];
    uint16_t port;
    int32_t len;

    if(getSn_RX_RSR(NTP_SOCKET) == 0)
        return;

    len = recvfrom(
        NTP_SOCKET,
        rx_buffer,
        sizeof(rx_buffer),
        ip,
        &port
    );

    if(len != NTP_PACKET_SIZE)
        return;

    build_ntp_response();

    /*
     * Transmit Timestamp
     *
     * Set immediately before packet transmission.
     */
    {
        uint32_t sec = get_ntp_seconds();
        uint32_t frac = get_ntp_fraction();

        put_u32(&tx_buffer[NTP_TX_TS], sec);
        put_u32(&tx_buffer[NTP_TX_TS + 4], frac);
    }

    sendto(
        NTP_SOCKET,
        tx_buffer,
        sizeof(tx_buffer),
        ip,
        port
    );
}
