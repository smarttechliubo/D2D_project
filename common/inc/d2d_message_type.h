/** @file d2d_message_type.h
*
* @brief  d2d protocol message type definition
*
* @par
* @author   bo.liu
* @date     2019/07/29
* COPYRIGHT NOTICE: (c) 2019  smartlogictech.
* All rights reserved.
*/


#ifndef D2D_MESSAGE_TYPE_H_
#define D2D_MESSAGE_TYPE_H_


#define D2D_PROTOCOL_MESSAGE_BASE      1000   /**message base */
#define MESSAGE_GAP        100


/*************************************IP<-->RLC message***************************/
/**base+0*/
#define IP_RLC_MESSAGE_BASE    (D2D_PROTOCOL_MESSAGE_BASE + 0 * MESSAGE_GAP)
#define IP_RLC_DATA_IND        (IP_RLC_MESSAGE_BASE + 0)
#define RLC_IP_DATA_RPT        (IP_RLC_MESSAGE_BASE + 1)


/*************************************RRC<-->RLC message***************************/
/**base+100*/
#define  RRC_RLC_MESSAGE_BASE   		(D2D_PROTOCOL_MESSAGE_BASE + 1 * MESSAGE_GAP)
#define  RRC_RLC_INITIAL_REQ     		(RRC_RLC_MESSAGE_BASE + 0)
#define  RLC_RRC_INITIAL_CFM            (RRC_RLC_MESSAGE_BASE + 1)
#define  RRC_RLC_RELEASE_REQ            (RRC_RLC_MESSAGE_BASE + 2)
#define  RLC_RRC_RELEASE_CFM            (RRC_RLC_MESSAGE_BASE + 3)
#define  RRC_RLC_BUF_STATUS_REQ  		(RRC_RLC_MESSAGE_BASE + 4)
#define  RLC_RRC_BUF_STATUS_RPT  		(RRC_RLC_MESSAGE_BASE + 5)
#define  RRC_RLC_DATA_IND        		(RRC_RLC_MESSAGE_BASE + 6)
/**sib1 content changed */
#define  RRC_RLC_BCCH_PARA_CFG_REQ      (RRC_RLC_MESSAGE_BASE + 7)
#define  RLC_RRC_BCCH_PARA_CFG_CFM      (RRC_RLC_MESSAGE_BASE + 8)

/**connect setup cfg */
#define  RRC_RLC_CONNECT_SETUP_CFG_REQ   (RRC_RLC_MESSAGE_BASE + 9)
#define  RLC_RRC_CONNECT_SETUP_CFG_CFM   (RRC_RLC_MESSAGE_BASE + 10)


/*************************************RRC<-->MAC message***************************/
/**base+200*/
#define  RRC_MAC_MESSAGE_BASE   	(D2D_PROTOCOL_MESSAGE_BASE + 2 * MESSAGE_GAP)
#define  RRC_MAC_INITIAL_REQ    	(RRC_MAC_MESSAGE_BASE + 0)
#define  MAC_RRC_INITIAL_CFM 	    (RRC_MAC_MESSAGE_BASE + 1)
#define  RRC_MAC_RELEASE_REQ        (RRC_MAC_MESSAGE_BASE + 2)
#define  MAC_RRC_RELEASE_CFM        (RRC_MAC_MESSAGE_BASE + 3)

/**MIB & SIB1 content changed */
#define  RRC_MAC_BCCH_PARA_CFG_REQ  (RRC_MAC_MESSAGE_BASE + 4)
#define  RRC_MAC_BCCH_PARA_CFG_CFM  (RRC_MAC_MESSAGE_BASE + 5)
#define  MAC_RRC_BCCH_MIB_RPT       (RRC_MAC_MESSAGE_BASE + 6)
#define  RRC_MAC_BCCH_SIB1_REQ      (RRC_MAC_MESSAGE_BASE + 7)
#define  MAC_RRC_BCCH_SIB1_RPT      (RRC_MAC_MESSAGE_BASE + 8)
/**CCCH:rrc connect message:(connect request, connect setup,connect complete)*/
#define  MAC_RRC_CCCH_RPT           		(RRC_MAC_MESSAGE_BASE + 9)
#define  RRC_MAC_CONNECT_SETUP_CFG_REQ      (RRC_MAC_MESSAGE_BASE + 10)
#define  MAC_RRC_CONNECT_SETUP_CFG_CFM      (RRC_MAC_MESSAGE_BASE + 11)
 /**in -out sync message*/
#define MAC_RRC_OUTSYNC_RPT                 (RRC_MAC_MESSAGE_BASE + 12)

/*************************************RRC<-->PHY  message***************************/
#define  RRC_PHY_MESSAGE_BASE   		(D2D_PROTOCOL_MESSAGE_BASE + 3 * MESSAGE_GAP)  /**base+300*/
#define  RRC_PHY_INITIAL_REQ   			(RRC_PHY_MESSAGE_BASE + 0)
#define  PHY_RRC_INITIAL_CFM 	    	(RRC_PHY_MESSAGE_BASE + 1)
#define  RRC_PHY_RELEASE_REQ            (RRC_PHY_MESSAGE_BASE + 2)
#define  PHY_RRC_RELEASE_CFM            (RRC_PHY_MESSAGE_BASE + 3)

#define  RRC_PHY_CS_REQ             	(RRC_PHY_MESSAGE_BASE + 4)
#define  RRC_PHY_BCCH_PARA_CFG_REQ  	(RRC_PHY_MESSAGE_BASE + 5)
#define  PHY_RRC_BCCH_PARA_CFG_CFM  	(RRC_PHY_MESSAGE_BASE + 6)
#define  RRC_PHY_CONNECT_SETUP_CFG_REQ  (RRC_PHY_MESSAGE_BASE + 7)
#define  PHY_RRC_CONNECT_SETUP_CFG_CFM  (RRC_PHY_MESSAGE_BASE + 8)

/*************************************MAC<--> RLC message***************************/
#define  MAC_RLC_MESSAGE_BASE   	(D2D_PROTOCOL_MESSAGE_BASE + 4 * MESSAGE_GAP)  /**base + 400*/
#define  MAC_RLC_BUF_STATUS_REQ     (MAC_RLC_MESSAGE_BASE + 0)
#define  RLC_MAC_BUF_STATUS_RPT     (MAC_RLC_MESSAGE_BASE + 1)
#define  MAC_RLC_DATA_REQ           (MAC_RLC_MESSAGE_BASE + 2)
#define  RLC_MAC_DATA_IND           (MAC_RLC_MESSAGE_BASE + 3)

/*************************************MAC<--> PHY message***************************/
#define  MAC_PHY_MESSAGE_BASE   (D2D_PROTOCOL_MESSAGE_BASE + 5 * MESSAGE_GAP)  /**base+500*/

#define  MAC_PHY_PBCH_TX_REQ        (MAC_PHY_MESSAGE_BASE + 0)
#define  PHY_MAC_PBCH_PDU_RPT       (MAC_PHY_MESSAGE_BASE + 1)
#define  MAC_PHY_PDCCH_SEND         (MAC_PHY_MESSAGE_BASE + 2)
#define  MAC_PHY_PUSCH_SEND         (MAC_PHY_MESSAGE_BASE + 3)
#define  PHY_MAC_DECOD_DATA_RPT     (MAC_PHY_MESSAGE_BASE + 4)
#define  PHY_MAC_ACK_RPT            (MAC_PHY_MESSAGE_BASE + 5)
#define  PHY_MAC_CQI_IND            (MAC_PHY_MESSAGE_BASE + 6)
#define  PHY_MAC_LINK_STATUS_IND    (MAC_PHY_MESSAGE_BASE + 7)



#endif /* D2D_MSG_TYPE_H_ */

