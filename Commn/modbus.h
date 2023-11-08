#ifndef MODBUS_H
#define MODBUS_H

#ifdef __cplusplus
extern "C"
{
#endif

#define TXBUFSIZE 40
#define RXBUFSIZE 40

    enum CMD_LIST
    {
        GET_INP_REG = 0x3,
        GET_HOLD_REG,
        SET_HOLD_REG = 0x6,
    };

    enum modbus_frame
    {
        CMD_DEV_ADR = 0,
        CMD_REQ, // only cmd 0x03 & 0x04 are supported
        CMD_REG_ADR_H,
        CMD_REG_ADR_L,
        CMD_SIZE_H,
        CMD_SIZE_L,
        CMD_CRC_H,
        CMD_CRC_L,
        MODBUS_CMD_LEN
    };

    /* response */
    enum
    {
        RESP_DEV_ADR = 0,
        RESP_REQ,
        RESP_COUNT,
        RESP_REG_VAL1_H,
        RESP_REG_VAL1_L,
        RESP_REG_VAL2_H,
        RESP_REG_VAL2_L,
        RESP_REG_VAL3_H,
        RESP_REG_VAL3_L,
        RESP_REG_VAL4_H,
        RESP_REG_VAL4_L,
        RESP_REG_VAL5_H,
        RESP_REG_VAL5_L,
        RESP_REG_VAL6_H,
        RESP_REG_VAL6_L,
        RESP_REG_VAL7_H,
        RESP_REG_VAL7_L,
        RESP_CRC_H,
        RESP_CRC_L,
        RESP_END
    };

#define INP_REG_SIZE 7
#define INP_DB_REG_SIZE 2
#define HOLD_REG_SIZE 1

    int send_request(int adr, int request_id, int start_adr, int size);
    void sendModBusMsg(void);
    uint16_t compute_crc16(uint16_t crc, uint16_t data);
    uint16_t compute_crc(uint8_t buf[], uint8_t count);
    int check_rxd_data(uint8_t rxbuffer[]);
    int GetPacketSize();
    uint8_t validateModBusData(int address, int size);
    int process_msg(void);

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_H */
