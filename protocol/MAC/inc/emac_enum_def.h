/**********************************************************
* @file emac_status.h
* 
* @brief  
* @author   guicheng.liu
* @date     2019/09/02
* COPYRIGHT NOTICE: (c) 2019  smartlogictech. 
* All rights reserved.
**********************************************************/
#ifndef _SMARTLOGICTECH_PROTOCOL_MAC_INC_EMAC_ENUM_H_
#define _SMARTLOGICTECH_PROTOCOL_MAC_INC_EMAC_ENUM_H_

typedef enum
{
	ECRC_NULL,
	ECRC_ACK,
	ECRC_NACK,
	ECRC_DTX
}crc_result_e;

typedef enum
{
	PHR,
	CRNTI,
	CONRES,
	TA
}mac_ce_e;

typedef enum
{
	EFORMAT0,
	EFORMAT1,
	EFARMAT_MAX
}dci_format_e;

typedef enum
{
	ESTATUS_NONE,
	ESTATUS_ACTIVE,
    ESTATUS_INACTIVE
}mac_status_e;

typedef enum
{
	EMAC_SRC, //SOURCE
	EMAC_DEST //DESTINATION
}mode_e;

typedef enum
{
	ESORTING_HIGH     = 0,
	ESORTING_LOW     = 1,
	ESORTING_UNSUPPORT
}sorting_policy_e;

typedef enum
{
	EPATTERN_GREEDY,
	EPATTERN_FAIR
}allocation_pattern_e;
#endif /* _SMARTLOGICTECH_PROTOCOL_MAC_INC_EMAC_ENUM_H_ */
