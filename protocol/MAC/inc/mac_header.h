/**********************************************************
* @file mac_header.h
* 
* @brief  mac header
* @author   guicheng.liu
* @date     2019/08/29
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_HAEDER_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_HAEDER_H_

/*MAC subheader short with 7bit Length field */
typedef struct {
    uint8_t E:1;
    uint8_t R:2;
    uint8_t LCID:5;
    uint8_t F:1;
    uint8_t L:7;
} __attribute__ ((__packed__)) mac_header_short;

/*MAC subheader long  with 15bit Length field */
typedef struct {
    uint8_t R:2;
    uint8_t E:1;
    uint8_t LCID:5;
    uint8_t F:1;
    uint8_t L_MSB:7;
    uint8_t L_LSB:8;
    uint8_t padding;
} __attribute__ ((__packed__)) mac_header_long;

/*MAC subheader short without length field */
typedef struct {
    uint8_t LCID:5;
    uint8_t E:1;
    uint8_t R:2;
} __attribute__ ((__packed__)) mac_header_fixed;

typedef struct
{
	uint16_t tb_length;
	uint8_t num_ce;
	uint8_t num_sdu;
	uint8_t rx_ceIds[MAX_NUM_CE];
	uint8_t rx_lcIds[MAX_LOGICCHAN_NUM];
	uint16_t rx_lengths[MAX_LOGICCHAN_NUM];
}mac_header_info;

#endif //_SMARTLOGICTECH_PROTOCOL_MAC_HAEDER_H_
