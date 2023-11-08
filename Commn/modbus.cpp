#include <cstdint>
#include "modbus.h"
#include "config.h"
#include "uart_interface.h"

uint8_t txbuffer[TXBUFSIZE];

#define MODBUS_RESP_LEN 100 // max len for modbus reponse

#ifdef MODBUS_MASTER
uint8_t modbus_txbuf[CMD_END];
uint8_t modbus_rxbuf[RESP_END];
uint8_t modbus_str[CMD_CRC_L * 2];
#else
uint8_t modbus_txbuf[MODBUS_RESP_LEN];
uint8_t modbus_rxbuf[MODBUS_CMD_LEN];
uint8_t modbus_str[RESP_CRC_L * 2];
#endif

#ifdef MODBUS_MASTER
#define RXD_DEV_ADR RESP_DEV_ADR
#define TX_LAST_BYTE CMD_CRC_L
#else
#define RXD_CRC CMD_CRC_H
#define RXD_DEV_ADR CMD_DEV_ADR
#endif

#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)

#ifdef MODBUS_MASTER

#else

uint8_t data_r_points[(INP_REG_SIZE + INP_DB_REG_SIZE) * 2]; // 6 inp data points
uint8_t data_rw_points[HOLD_REG_SIZE * 2];                   // 1 holding reg

#endif

uint16_t reg_start_adr, reg_count;
uint8_t modbus_func;
uint8_t packet_len;

int GetPacketSize()
{
#ifdef SLAVE
#ifdef modbus_ascii
    return RESP_CRC_L;
#else
    return RESP_END;
#endif
#else
#ifdef modbus_ascii
    return CMD_CRC_L;
#else
    return CMD_END;
#endif
#endif
}
//
// send a mod bus request to slave
// parameters:
// adr: slave adress
// request_id: modbus request id
// start_adr: start adr of register
// size: number of registers
//
int send_request(int adr, int request_id, int start_adr, int size)
{

    uint16_t crc;
    uint8_t idx = 0;
    if (request_id != 3 && request_id != 4) // currently only cmd 3 and 4 supported
        return -1;

    modbus_txbuf[CMD_DEV_ADR] = adr;
    modbus_txbuf[CMD_REQ] = request_id;
    modbus_txbuf[CMD_REG_ADR_H] = 0x0;
    modbus_txbuf[CMD_REG_ADR_L] = start_adr;
    modbus_txbuf[CMD_SIZE_H] = 0;
    modbus_txbuf[CMD_SIZE_L] = size;

    crc = compute_crc(modbus_txbuf, CMD_CRC_H);
    idx = CMD_CRC_H;
    modbus_txbuf[idx++] = crc & 0xff;
#ifndef modbus_ascii
    modbus_txbuf[idx++] = crc >> 8;
#endif
    packet_len = reg_count * 2 + 4; // adr+req+size+regs+crc
#ifndef modbus_ascii
    packet_len++; // crc is two bytes
#endif
    sendModBusMsg();
    return 1;
}
void sendModBusMsg(void)
{
    uint8_t i, j;
    uint8_t x, last_byte;

#ifdef MODBUS_MASTER
    last_byte = CMD_CRC_L;
#else
    last_byte = RESP_REG_VAL1_H + reg_count * 2 + 1;
#endif
#ifndef modbus_ascii
    last_byte++; // add one for 16bit crc
#endif
#ifdef modbus_ascii
    modbus_str[0] = ':';
    j = 1;
#endif
    for (i = 0; i < last_byte; i++)
    {
        x = modbus_txbuf[i];
#ifdef modbus_ascii
        modbus_str[j++] = TO_HEX(((x & 0xF0) >> 4));
        modbus_str[j++] = TO_HEX(((x & 0x0F)));
#else
        UartPutC(x);
#endif
    }
#ifdef modbus_ascii
    modbus_str[j++] = '\r';
    modbus_str[j++] = '\n';
    modbus_str[j] = '\0';
    UartWriteStr(modbus_str);
#endif
}

uint16_t compute_crc(uint8_t buf[], uint8_t count)
{
    uint8_t i;

#ifdef modbus_ascii
    uint8_t crc;
    crc = 0;
    for (i = 0; i < count; i++)
    {
        crc += buf[i];
    }
    crc = ~crc + 1;
    return crc;
#else
    uint16_t crc;
    crc = 0xFFFF;
    for (i = 0; i < count; i++)
    {
        crc = compute_crc16(crc, buf[i]);
    }
    return crc;
#endif
}
uint16_t compute_crc16(uint16_t crc, uint16_t data)
{
    const uint16_t CRC16 = 0xA001;
    uint16_t LSB;
    uint8_t i;
    crc = ((crc ^ data) | 0xFF00) & (crc | 0x00FF);
    for (i = 0; i < 8; i++)
    {
        LSB = (crc & 0x0001);
        crc = crc / 2;
        if (LSB)
            crc = crc ^ CRC16;
    }
    return (crc);
}

int check_rxd_data(uint8_t rxbuffer[], int rxcount, uint8_t address)
{

    uint8_t i;
    uint8_t x, c, value, msb;
    uint8_t count, pos;
    uint8_t data_valid = 0;

    i = rxbuffer[0];
    int rxrptr = 0;

#ifdef modbus_ascii
    x = ':';
#else
    x = address;
#endif

    if (i == x)
    { // SOF

#ifndef modbus_ascii

        for (i = 0; i < rxcount; i++)
        {
            c = rxbuffer[rxrptr++];
            modbus_rxbuf[i] = c;
        }
#else
        rxrptr++; // drop ':'
        count = rxcount;
        pos = 0;

        for (i = 0; count > 0; count--)
        {

            x = rxbuffer[rxrptr++];

            if (x == '\0')
            { // end of message
                data_valid = 1;
                break;
            }

            c = x > '9' ? 'A' - 10 : '0';
            x -= c;
            if (pos == 0)
            {
                msb = x;
                pos++;
            }
            else
            {
                value = msb * 16 + x;
                modbus_rxbuf[i++] = value;
                pos = 0;

                if (i >= packet_len)
                    break;
            }
        }
#endif
    }

    if (data_valid)
    {

        data_valid = validateModBusData(address, rxcount);
        if (data_valid)
        {
            process_msg();

#ifndef MODBUS_MASTER
            sendModBusMsg(); // send response to query
#endif
        }
        return 1;
    }
    else
    {

        return -1;
    }
    return -1;
}
uint8_t validateModBusData(int address, int size)
{
    uint16_t crc;
    uint8_t data;
    uint8_t rd_crc;
#ifdef MODBUS_MASTER
    rd_crc = reg_count * 2 + RESP_REG_VAL1_H;
#else
    rd_crc = CMD_CRC_H;
#endif
    crc = compute_crc(modbus_rxbuf, rd_crc);
    if ((crc & 0x00FF) != modbus_rxbuf[rd_crc]) // Low byte calculation
        return 0;
#ifndef modbus_ascii
    if ((crc >> 8) != modbus_rxbuf[rd_crc + 1]) // High byte calculation
        return 0;
#endif
    data = modbus_rxbuf[RXD_DEV_ADR];
    if (data != address)
        return 0;
#ifdef MODBUS_MASTER
    data = modbus_rxbuf[RESP_COUNT];
    if (data != reg_count * 2)
        return 0;
#endif
#ifndef MODBUS_MASTER

    modbus_func = modbus_rxbuf[CMD_REQ];
    if ((modbus_func != GET_INP_REG) && (modbus_func != GET_HOLD_REG) && (modbus_func != GET_HOLD_REG))
        return 0;

    reg_start_adr = modbus_rxbuf[CMD_REG_ADR_L];
    reg_count = modbus_rxbuf[CMD_SIZE_L];
    if (modbus_func == GET_INP_REG)
    {

        if ((reg_start_adr + reg_count) > (INP_REG_SIZE + INP_DB_REG_SIZE))
        {
            if (reg_start_adr > (INP_REG_SIZE + INP_DB_REG_SIZE))
                return 0;
            else
                reg_count = (INP_REG_SIZE + INP_DB_REG_SIZE) - reg_start_adr;
        }
    }

#endif

    return 1;
}
#if 0
int process_msg_sensor(uint8_t buffer, uint8_t address)
{
    uint16_t val;
    uint8_t idx, adr;

    if (uart_rxbuf[RESP_REQ] != modbus_func) // request to read input reg value
        return -1;

    if (modbus_rxbuf[RESP_DEV_ADR] == address)
    {

        if (modbus_rxbuf[RESP_COUNT] == reg_count * 2)
        {
            idx = RESP_REG_VAL1_H;
            adr = reg_start_adr;

            for (int i = 0; i < reg_count; i++)
            {
                val = modbus_rxbuf[idx++] << 8;
                val |= modbus_rxbuf[idx++];
                if (modbus_func == GET_HOLD_REG)
                {
                    if (adr == 1)
                        system_control = val;
                }
                else if (modbus_func == GET_INP_REG)
                {
                    if (adr == 0)
                        inp_curr = val;
                    if (adr == 1)
                        inp_temp = val;
                    if (adr == 2)
                        inp_va_vol = val;
                    if (adr == 3)
                        inp_va_amp = val;
                    if (adr == 4)
                        inp_fault1 = val;
                    if (adr == 5)
                        inp_fault2 = val;
                    if (adr == 6)
                        system_control = val;
                    if (adr == 7)
                        mirrordata[DB_THR_OLOAD] = val;
                    if (adr == 8)
                        mirrordata[DB_THR_TEMP] = val;
                }
                adr++;
            }
        }
    }

    return 1;
}

int process_msg_modbus(int address)
{
    uint16_t crc;
    uint8_t idx;
    uint8_t adr;

    if (modbus_func == SET_HOLD_REG)
        return 0;

    modbus_txbuf[RESP_DEV_ADR] = address;
    modbus_txbuf[RESP_REQ] = modbus_func;
    modbus_txbuf[RESP_COUNT] = reg_count * 2;
    adr = (reg_start_adr)*2;
    idx = RESP_REG_VAL1_H;

    for (int i = 0; i < reg_count * 2; i++)
    {
        if (modbus_func == GET_INP_REG)
        {
            modbus_txbuf[idx++] = data_r_points[adr++];
        }
        if (modbus_func == GET_HOLD_REG)
        {
            modbus_txbuf[idx++] = data_rw_points[adr++];
        }
    }
    crc = get_crc(modbus_txbuf, idx);
    modbus_txbuf[idx++] = crc & 0xff;
#ifndef modbus_ascii
    modbus_txbuf[idx++] = crc >> 8;
#endif
    return 0;
}
#endif