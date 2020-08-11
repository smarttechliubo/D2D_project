/******************************************************************
 * @file  rrc_d2d_message_process.c
 * @brief:    [file description] 
 * @author: bo.liu
 * @Date 2019年8月14日
 * COPYRIGHT NOTICE: (c) smartlogictech. All rights reserved. 
 * Change_date         Owner         Change_content
 * 2019年8月14日        bo.liu       create file

*****************************************************************/

/**************************include******************************/
#include <typedef.h>
#include <rrc_global_def.h>
#include <MasterInformationBlock.h>
#include <SystemInformationBlockType1.h>
#include <RRCConnectionSetup.h>
#include <RRCConnectionRequest.h>
#include <RRCConnectionRelease.h>
#include <CCCH-Message.h>
#include <log.h>


 
 
 
/**************************function******************************/

#ifdef UPER_ENCODE_TO_FILE

 /*!   
 * @brief:  Write the encoded output into some FILE stream. 
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *buffer :          [output buffer ]
 * @param: size :             [buffer size  ]
 * @param: *app_key :         [file description header ]
 */
int write_out(const void *buffer, size_t size, void *app_key) {
	FILE *out_fp = app_key;
	size_t wrote = fwrite(buffer, 1, size, out_fp);
	return (wrote == size) ? 0 : -1;
}
#endif 

 /*!   
 * @brief: encode d2d mib info to specific buffer 
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *buf :             [mib encode output buffer]
 * @param: buffer_size#endif :[output buffer maximum size ]
 */
int EncodeD2dMib(uint8_t *buf,uint32_t max_buffersize, uint32_t *encode_size )
{
	MasterInformationBlock_t  *bch_msg; 

	asn_enc_rval_t ec; 
    


	int  size ; 
	int i; 
	uint8_t sfn = (0x7f << 1); 
	uint8_t pdcch_config = (0x7f << 1); //!7bit ,取高7bit 

	
	/* Allocate the Rectangle_t */
	bch_msg = calloc(1, sizeof(MasterInformationBlock_t)); /* not malloc! */
	if(!bch_msg) {
		perror("calloc() failed");
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}


	bch_msg->dl_Bandwidth = 1; 

	bch_msg->systemFrameNumber.buf = &sfn; 
	bch_msg->systemFrameNumber.size = 1; 
	bch_msg->systemFrameNumber.bits_unused = 1; 
  
	
    
	bch_msg->pdcch_Config.buf =  &pdcch_config;
	bch_msg->pdcch_Config.size = 1; 
	bch_msg->pdcch_Config.bits_unused  = 1; 
	 
	


    ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,
                               &asn_DEF_MasterInformationBlock, 
                               bch_msg,buf,max_buffersize); 
	AssertFatal((ec.encoded != -1), RRC , "function %s Could not encode  (at %s)\n", \
                __func__, ec.failed_type->name);
    *encode_size = ec.encoded; 
    

		
	//xer_fprint(stdout,&asn_DEF_MasterInformationBlock,(void*)bch_msg); //！提取出IE 内容
	//asn_fprint(stdout, &asn_DEF_MasterInformationBlock, bch_msg); //!不提取bit,只打印原始Bite
   // system("pause");
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */

}

 


 /*!   
 * @brief:  encode sib1 message to buffer 
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *encode_buffer :   [encode sib1 message output buffer ]
 * @param: buffersize :       [maximum buffer_size  ]
 */
int EncodeD2dSib1(uint8_t *encode_buffer, uint32_t max_buffersize, uint32_t *encode_size )
{
	SystemInformationBlockType1_t  *sib1_msg; 
	FILE *fp;
	asn_enc_rval_t ec; 
	const char *filename; 


	uint16_t *spare = calloc(1, sizeof(uint16_t));
	int  size ; 
	int i;
	long p_max = 30; 
	long *hop_mode_addr = calloc(1,sizeof(long)); 
	long *group_assign_pusch_addr =calloc(1,sizeof(long));
	long *cycle_shift_addr = calloc(1,sizeof(long));  
	

 
	/* Allocate the Rectangle_t */
	sib1_msg = calloc(1, sizeof(SystemInformationBlockType1_t)); /* not malloc! */
	if(!sib1_msg) {
		perror("calloc() failed");
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}


	sib1_msg->p_Max = p_max; 
	
	sib1_msg->tdd_Config.subframeAssignment = (long)TDD_Config__subframeAssignment_dduu;

    sib1_msg->carrierFreq_info.band_info = 0; 
	sib1_msg->carrierFreq_info.dl_carrier_freq = 2300; //![0-65535]
	sib1_msg->carrierFreq_info.ul_carrier_freq = 2300; 
	
	sib1_msg->radioResourceConfigCommon.psush_Hop_Config.hop_enable = 0; 
	if (1 == sib1_msg->radioResourceConfigCommon.psush_Hop_Config.hop_enable)
	{
		*hop_mode_addr = 10; 
		sib1_msg->radioResourceConfigCommon.psush_Hop_Config.hop_mode = hop_mode_addr; 
	}
	sib1_msg->radioResourceConfigCommon.ul_ref_signal_pusch.group_hopping_enable = 0; 
	if (1 == sib1_msg->radioResourceConfigCommon.ul_ref_signal_pusch.group_hopping_enable)
	{	
		*group_assign_pusch_addr = 10; 
		sib1_msg->radioResourceConfigCommon.ul_ref_signal_pusch.group_assign_pusch = group_assign_pusch_addr; 
	}
	sib1_msg->radioResourceConfigCommon.ul_ref_signal_pusch.seq_hopping_enable = 0; 
	if(1 == sib1_msg->radioResourceConfigCommon.ul_ref_signal_pusch.seq_hopping_enable )
	{
		*cycle_shift_addr = 10; 
		sib1_msg->radioResourceConfigCommon.ul_ref_signal_pusch.cycle_shift = cycle_shift_addr;
    }
    //!<encode BCH 
    //ec = asn_encode(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_SystemInformationBlockType1, sib1_msg, write_out, fp); 
    ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_SystemInformationBlockType1, 
                              sib1_msg,encode_buffer,max_buffersize); 


	 AssertFatal((ec.encoded != -1), RRC , "function %s Could not encode  (at %s)\n", __func__, ec.failed_type->name);
     *encode_size = ec.encoded;
	
		
	
	//fclose(fp);
	//xer_fprint(stdout,&asn_DEF_SystemInformationBlockType1,(void*)sib1_msg);
	//asn_fprint(stdout, &asn_DEF_MasterInformationBlock, bch_msg); 
    
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */



}

#if 0

 /*!   
 * @brief:  encode D2D RRC CONNECTION SETUP message to buffer  
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *encode_buffer :   [encode sib1 message output buffer ]
 * @param: buffersize :       [maximum buffer_size  ]
 */
int EncodeD2dRrcConnectionSetup(uint8_t  *encode_buffer, uint32_t buffersize)
{
	RRCConnectionSetup_t  *encode_msg; 
	FILE *fp;
	asn_enc_rval_t ec; 
	const char *filename; 

	SRB_ToAddMod_t  *srb_addmod; 


	DRB_ToAddMod_t   *drb_addmod;
	DRB_Identity_t   drb_id; 

	unsigned char buf[2048]; 
	int  size ; 
	int i;


	/* Allocate the Rectangle_t */
	encode_msg = calloc(1, sizeof(RRCConnectionSetup_t)); /* not malloc! */
	if(!encode_msg) {
		perror("calloc() failed");
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}

	encode_msg->radioResourceConfigCommon.pusch_dedi_config.beta_off_ack_ind = 0;
	encode_msg->mac_config.maxharq_tx = 4;
	encode_msg->mac_config.max_out_sync	 = 4 ;



	srb_addmod = (calloc(1,sizeof(SRB_ToAddMod_t)));

	  //! step1:srb_add
	(srb_addmod)->srb_Identity = 1; //SRB1
	(srb_addmod)->rlc_config.present = RLC_Config_PR_um_bi_direction; 
	//!尤其注意ASN1C中的定义的枚举，要使用枚举值，而不能自己写int值
	(srb_addmod)->rlc_config.choice.um_bi_direction.ul_um_rlc.sn_FieldLength = SN_FieldLength_size10;
	(srb_addmod)->rlc_config.choice.um_bi_direction.dl_um_rlc.sn_FieldLength = SN_FieldLength_size10; 
	(srb_addmod)->rlc_config.choice.um_bi_direction.dl_um_rlc.t_Reordering = T_Reordering_ms200; 

	(srb_addmod)->logicChannelConfig.logicch_id = 0; //![0-4]
	(srb_addmod)->logicChannelConfig.priority = 1; //![1-16]
	(srb_addmod)->logicChannelConfig.channel_type = LogicChannelConfig__channel_type_ccch; 
	encode_msg->srb_ToAddModList = calloc(1,sizeof(SRB_ToAddModList_t)); 
	
	ASN_SEQUENCE_ADD(&(encode_msg->srb_ToAddModList->list),srb_addmod);

	//!step2: drb add 
	drb_addmod = calloc(1,sizeof(DRB_ToAddMod_t)); 
	encode_msg->drb_ToAddModList = calloc(1,sizeof(DRB_ToAddModList_t)); 
	drb_addmod->drb_Identity = 1; //![1-4]
	drb_addmod->rlc_config.present = RLC_Config_PR_um_bi_direction; 
	drb_addmod->rlc_config.choice.um_bi_direction.dl_um_rlc.sn_FieldLength = SN_FieldLength_size10; 
	drb_addmod->rlc_config.choice.um_bi_direction.dl_um_rlc.t_Reordering = T_Reordering_ms200; 
	drb_addmod->rlc_config.choice.um_bi_direction.ul_um_rlc.sn_FieldLength = SN_FieldLength_size10; 
	drb_addmod->logicChannelConfig.logicch_id = 1; //![0-4]
	drb_addmod->logicChannelConfig.priority = 2; //![1..16]
	drb_addmod->logicChannelConfig.channel_type = LogicChannelConfig__channel_type_dtch; 
	ASN_SEQUENCE_ADD(&(encode_msg->drb_ToAddModList->list),drb_addmod);

	//!step3: drb release, 在connect setup时一般不使用，为了今后扩展使用此消息进行release

	/*
	encode_msg->drb_ToReleaselist = calloc(1,sizeof(DRB_ToReleaseList_t)); 
	drb_id = 1; 
	ASN_SEQUENCE_ADD(&(encode_msg->drb_ToReleaselist->list),&drb_id);
	*/

    //!<encode BCH

    ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_RRCConnectionSetup, encode_msg,encode_buffer,buffersize); 
	AssertFatal((ec.encoded != -1), RRC , "function %s Could not encode  (at %s)\n", __func__, ec.failed_type->name);
	*encode_size = ec.encoded;

	//fclose(fp);
	//xer_fprint(stdout,&asn_DEF_RRCConnectionSetup,(void*)encode_msg);
	//asn_fprint(stdout, &asn_DEF_MasterInformationBlock, bch_msg); 
 //   system("pause");
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */

}



 /*!   
 * @brief:  encode rrc connection requset message to buffer 
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *encode_buffer :   [encode sib1 message output buffer ]
 * @param: buffersize :       [maximum buffer_size  ]
 */
int EncodeD2dRrcConnectRequest(uint8_t  *encode_buffer, uint32_t buffersize)
{
	RRCConnectionRequest_t  *encode_msg; 
	FILE *fp;
	asn_enc_rval_t ec; 
	const char *filename; 

	short ueip = 0xf01e; //!15bit,注意这里是按照大端模式编码

	unsigned char buf[1024]; 
	int  size ; 
	int i;
	long p_max = 30; 
	

 	
	/* Allocate the Rectangle_t */
	encode_msg = calloc(1, sizeof(RRCConnectionRequest_t)); /* not malloc! */
	if(!encode_msg) {
		perror("calloc() failed");
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}
	
	encode_msg->ue_Identity = 8; //![0-8]
	encode_msg->establishmentCause = RRCConnectionRequest__establishmentCause_emergency;
	//RRCConnectionRequest__establishmentCause_normal;
	encode_msg->ue_Ip.buf = &ueip; 

	encode_msg->ue_Ip.size = 2; 
	encode_msg->ue_Ip.bits_unused = 1; 


	
	

	ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_RRCConnectionRequest, encode_msg,encode_buffer,buffersize); 
     AssertFatal((ec.encoded != -1), RRC , "function %s Could not encode  (at %s)\n", __func__, ec.failed_type->name);
     *encode_size = ec.encoded;
	
	
	//fclose(fp);
//	xer_fprint(stdout,&asn_DEF_RRCConnectionRequest,(void*)encode_msg);
	//asn_fprint(stdout, &asn_DEF_MasterInformationBlock, bch_msg); 
  //  system("pause");
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */
}




 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *encode_buffer :   [encode sib1 message output buffer ]
 * @param: buffersize :       [maximum buffer_size  ]

 */
int EncodeD2dRrcConnectRelease(uint8_t  *encode_buffer, uint32_t buffersize)
{
	RRCConnectionRelease_t  *encode_msg; 
	FILE *fp;
	asn_enc_rval_t ec; 
	const char *filename; 

	short ueip = 0xf01e; //!15bit,注意这里是按照大端模式编码

	unsigned char buf[1024]; 
	int  size ; 
	int i;
	long p_max = 30; 
	
#ifdef UPER_ENCODE_TO_FILE

	if(argc < 2) 
	{
		fprintf(stderr, "Specify filename for BER output\n");
	} else 
	{
		filename = argv[1];
		fp = fopen(filename, "wb");
		/* for BER output */
		if(!fp) {
			perror(filename);
			return RRC_MESSAGE_EN_DECODE_ERROR;
	    }
	}
 #endif 
 
	/* Allocate the Rectangle_t */
	encode_msg = calloc(1, sizeof(RRCConnectionRequest_t)); /* not malloc! */
	if(!encode_msg) {
		perror("calloc() failed");
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}
	
	encode_msg->ue_Identity = 8; //![0-8]
	encode_msg->releaseCause = RRCConnectionRelease__releaseCause_normal;
	//RRCConnectionRequest__establishmentCause_normal;



	
	
    //!<encode BCH 
 #ifdef UPER_ENCODE_TO_FILE
    ec = asn_encode(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_RRCConnectionRelease, encode_msg, write_out, fp); 
    fclose(fp);
	if(ec.encoded == -1) {
		fprintf(stderr, "Could not encode  (at %s)\n",
		 ec.failed_type->name);  //ec.failed_type ? 
		system("pause");
		return RRC_MESSAGE_EN_DECODE_ERROR;
	} else {
		printf("ec.encoded = %d bytes\n",ec.encoded); 
		fprintf(stderr, "Created %s with PER encoded success\n", filename);

		
		fp = fopen(filename,"rb"); 
		size = fread(buf, 1, sizeof(buf), fp);
		}
#else 

   ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_RRCConnectionRelease, encode_msg,encode_buffer,buffersize); 
   if(ec.encoded == -1) {
   		fprintf(stderr, "Could not encode  (at %s)\n",
   		 ec.failed_type->name);  //ec.failed_type ? 
   		system("pause");
   		return RRC_MESSAGE_EN_DECODE_ERROR;
   	} else {
   		size = ec.encoded; 
   		}

#endif 
	 
		if (size !=0)
		{
			printf("encode output byte: ");
		    for (i = 0; i < size;i++)
			{
			   fprintf(stdout,"%x", buf[i]); 
			}
			printf("  \n");
		}
	
	//fclose(fp);
	//xer_fprint(stdout,&asn_DEF_RRCConnectionRelease,(void*)encode_msg);
	//asn_fprint(stdout, &asn_DEF_MasterInformationBlock, bch_msg); 
   // system("pause");
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */
}

#endif 



 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月20日
 * @param: *encode_buffer :   [param description ]
 * @param: max_buffersize :   [param description ]
 * @param: encode_size :      [param description ]
 * @param: ccch_messsage_type :[param description ]
 */
int EncodeD2dCcch(uint8_t      *encode_buffer, uint32_t max_buffersize, 
                       uint32_t *encode_size,  CCCH_MessageType_PR ccch_messsage_type)
{

	CCCH_Message_t *encode_msg; 
	
    FILE *fp;
	asn_enc_rval_t ec; 

	uint16_t *ueip  = NULL;

	

	RRCConnectionSetup_t    *rrc_connect_setup; 

    SRB_ToAddMod_t  *srb_addmod; 


	DRB_ToAddMod_t   *drb_addmod;
	DRB_Identity_t   drb_id; 

	unsigned char buf[1024]; 
	int  size ; 
	int i;


    	/* Allocate the Rectangle_t */
	encode_msg = calloc(1, sizeof(CCCH_Message_t)); /* not malloc! */
	if(!encode_msg) {
		perror("calloc() failed");
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}

   
    switch (ccch_messsage_type)
    {
    	case CCCH_MessageType_PR_rrcConnectionrequest:
    	{
    	 
    		
	    	encode_msg->message.present = CCCH_MessageType_PR_rrcConnectionrequest; 
			encode_msg->message.choice.rrcConnectionrequest.ue_Identity =  0; //![0-8]
			encode_msg->message.choice.rrcConnectionrequest.establishmentCause = RRCConnectionRequest__establishmentCause_normal; 
			encode_msg->message.choice.rrcConnectionrequest.ue_Ip.buf = MALLOC(2);
			encode_msg->message.choice.rrcConnectionrequest.ue_Ip.buf[0] = 0x1e; 
			encode_msg->message.choice.rrcConnectionrequest.ue_Ip.buf[1] = 0xf0; 
			encode_msg->message.choice.rrcConnectionrequest.ue_Ip.size = 2; 
			encode_msg->message.choice.rrcConnectionrequest.ue_Ip.bits_unused = 0;
		
			
			LOG_DEBUG(DUMMY, "Dummy send CCCH_MessageType_PR_rrcConnectionrequest to rrc \n"); 
			break; 
	    }
		case CCCH_MessageType_PR_rrcConnectionsetup:
		{
			encode_msg->message.present = CCCH_MessageType_PR_rrcConnectionsetup; 

			rrc_connect_setup = calloc(1, sizeof(RRCConnectionSetup_t));
			
			rrc_connect_setup->radioResourceConfigCommon.pusch_dedi_config.c_rnti= g_rrc_mac_report_rnti; 
			rrc_connect_setup->radioResourceConfigCommon.pusch_dedi_config.beta_off_ack_ind = 0;
	    	rrc_connect_setup->mac_config.maxharq_tx = 4;
	    	rrc_connect_setup->mac_config.max_out_sync	 = 4 ;
	    
	    
	    
	    	srb_addmod = (calloc(1,sizeof(SRB_ToAddMod_t)));
	    
	    	  //! step1:srb_add,srb0 设置为Nothing
	    	(srb_addmod)->srb_Identity = RB_TYPE_SRB0; //SRB0
		     /*!modify begin:  by bo.liu, Date: 2019/9/6
		       modify cause:SRB0 change to TM mode */ 
	    	(srb_addmod)->rlc_config.present = RLC_Config_PR_NOTHING; 
	    	
	    	#if 0
	    	(srb_addmod)->rlc_config.choice.um_bi_direction.ul_um_rlc.sn_FieldLength = SN_FieldLength_size10;
	    	(srb_addmod)->rlc_config.choice.um_bi_direction.dl_um_rlc.sn_FieldLength = SN_FieldLength_size10; 
	    	(srb_addmod)->rlc_config.choice.um_bi_direction.dl_um_rlc.t_Reordering = T_Reordering_ms200; 
	       
	        
	    	(srb_addmod)->logicChannelConfig.logicch_id = 0; //![0-4]
	    	(srb_addmod)->logicChannelConfig.priority = 1; //![1-16]
	    	(srb_addmod)->logicChannelConfig.channel_type = LogicChannelConfig__channel_type_ccch; 
	    	rrc_connect_setup->srb_ToAddModList = calloc(1,sizeof(SRB_ToAddModList_t)); 
	    	
	    	ASN_SEQUENCE_ADD(&(rrc_connect_setup->srb_ToAddModList->list),srb_addmod);
	        #endif 
	        
	    	//!step2: drb add 
	    	drb_addmod = calloc(1,sizeof(DRB_ToAddMod_t)); 
	    	rrc_connect_setup->drb_ToAddModList = calloc(1,sizeof(DRB_ToAddModList_t)); 
	    	drb_addmod->drb_Identity = 1; //![1-4]
	    	drb_addmod->rlc_config.present = RLC_Config_PR_um_bi_direction; 
	    	drb_addmod->rlc_config.choice.um_bi_direction.dl_um_rlc.sn_FieldLength = SN_FieldLength_size10; 
	    	drb_addmod->rlc_config.choice.um_bi_direction.dl_um_rlc.t_Reordering = T_Reordering_ms200; 
	    	drb_addmod->rlc_config.choice.um_bi_direction.ul_um_rlc.sn_FieldLength = SN_FieldLength_size10; 
	    	drb_addmod->logicChannelConfig.logicch_id = 1; //![0-4]
	    	drb_addmod->logicChannelConfig.priority = 2; //![1..16]
	    	drb_addmod->logicChannelConfig.channel_type = LogicChannelConfig__channel_type_dtch; 
	    	ASN_SEQUENCE_ADD(&(rrc_connect_setup->drb_ToAddModList->list),drb_addmod);

			//!step3: drb release, 在connect setup时一般不使用，为了今后扩展使用此消息进行release

			/*
			rrc_connect_setup->drb_ToReleaselist = calloc(1,sizeof(DRB_ToReleaseList_t)); 
			drb_id = 1; 
			ASN_SEQUENCE_ADD(&(rrc_connect_setup->drb_ToReleaselist->list),&drb_id);
			*/
			encode_msg->message.choice.rrcConnectionsetup = *rrc_connect_setup; 
			LOG_DEBUG(DUMMY, "Dummy send CCCH_MessageType_PR_rrcConnectionsetup to rrc \n"); 
			break;
		}
		case CCCH_MessageType_PR_rrcConectioncomplete:
		{
			encode_msg->message.present = CCCH_MessageType_PR_rrcConectioncomplete; 
			encode_msg->message.choice.rrcConectioncomplete.complete_status = 1;
			LOG_DEBUG(DUMMY, "Dummy send CCCH_MessageType_PR_rrcConectioncomplete to rrc \n"); 

			break; 
		}
		default :break; 
	}
 

	ec = asn_encode_to_buffer(0,ATS_UNALIGNED_BASIC_PER,&asn_DEF_CCCH_Message, encode_msg,encode_buffer,max_buffersize); 
	
    AssertFatal((ec.encoded != -1), RRC , "function %s Could not encode  (at %s), encode message = %d\n", \
                __func__, ec.failed_type->name,ccch_messsage_type);
    *encode_size = ec.encoded; 
    
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */

}

/*!   
 * @brief:  decode mib message  
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *bch_msg :         [decode output struct ]
 * @param: *buf :             [message buffer ]
 * @param: size :             [message buffer size ]
 */
int DecodeD2dMib(MasterInformationBlock_t      **bch_msg,uint8_t *buf,uint32_t size )
{

	asn_dec_rval_t ec; 
	const char *filename; 
    MasterInformationBlock_t      *bch_msg_temp = NULL; 

	
	

    //!<encode BCH 
    ec = asn_decode(0,ATS_UNALIGNED_BASIC_PER,
                    &asn_DEF_MasterInformationBlock,
                    (void **)&bch_msg_temp, buf, size); 
                    
    *bch_msg = bch_msg_temp;
	AssertFatal(ec.code == RC_OK, RRC, "%s: Broken  decoding at byte %ld\n", __func__,(long)ec.consumed); 
	
	/* Also print the constructed Rectangle XER encoded (XML) */
	//xer_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg); //!<
    //asn_fprint(stdout, &asn_DEF_MasterInformationBlock, bch_msg);  
    
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */
}





 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *decode_msg :      [decode output struct ]
 * @param: *buf :             [message buffer ]
 * @param: size :             [message buffer size ]
 */
int DecodeD2dSib1(SystemInformationBlockType1_t       **decode_msg,uint8_t *buf,uint32_t size )
{

	asn_dec_rval_t ec; 
	const char *filename; 

	
	SystemInformationBlockType1_t       *decode_msg_temp = NULL; 

    //!<encode BCH 
    ec = asn_decode(0,ATS_UNALIGNED_BASIC_PER,
                    &asn_DEF_SystemInformationBlockType1,
                    (void **)&decode_msg_temp, buf, size); 
   
    *decode_msg = decode_msg_temp;
	AssertFatal(ec.code == RC_OK, RRC, "%s: Broken  decoding at byte %ld\n", __func__,(long)ec.consumed); 
	
	/* Also print the constructed Rectangle XER encoded (XML) */
	//xer_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg); //!<
    //asn_fprint(stdout, &asn_DEF_SystemInformationBlockType1, decode_msg);  
    
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */
}


#if 0

 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *decode_msg :      [decode output struct ]
 * @param: *buf :             [message buffer ]
 * @param: size :             [message buffer size ]
 */
int DecodeD2dRrcConnectSetup(RRCConnectionSetup_t       *decode_msg,uint8_t *buf,uint32_t size )
{

	FILE *fp;
	asn_dec_rval_t ec; 
	const char *filename; 

#ifdef UPER_ENCODE_TO_FILE
	if(argc < 2) 
	{
		fprintf(stderr, "Specify filename for BER output\n");
	} else 
	{
		filename = argv[1];
		fp = fopen(filename, "rb");
		/* for BER output */
		if(!fp) {
			perror(filename);
			return RRC_MESSAGE_EN_DECODE_ERROR;
	    }
	}


	size = fread(buf, 1, sizeof(buf), fp); 
	fclose(fp);
	if(!size) {
		fprintf(stderr, "%s: Empty or broken\n", filename);
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}
#endif 
	
	

    //!<encode BCH 
    ec = asn_decode(0,ATS_UNALIGNED_BASIC_PER,
                    &asn_DEF_RRCConnectionSetup,
                    (void **)&decode_msg, buf, size); 
   
	fclose(fp);
	AssertFatal(ec.code == RC_OK, RRC, "%s: Broken  decoding at byte %ld\n", __func__,(long)ec.consumed); 
	
	/* Also print the constructed Rectangle XER encoded (XML) */
	//xer_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg); //!<
    //asn_fprint(stdout, &asn_DEF_RRCConnectionSetup, decode_msg);  
    
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */
}



/*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *decode_msg :      [decode output struct ]
 * @param: *buf :             [message buffer ]
 * @param: size :             [message buffer size ]
 */
int DecodeD2dRrcConnectRequest(RRCConnectionRequest_t           *decode_msg,uint8_t *buf,uint32_t size )
{

	FILE *fp;
	asn_dec_rval_t ec; 
	const char *filename; 

#ifdef UPER_ENCODE_TO_FILE
	if(argc < 2) 
	{
		fprintf(stderr, "Specify filename for BER output\n");
	} else 
	{
		filename = argv[1];
		fp = fopen(filename, "rb");
		/* for BER output */
		if(!fp) {
			perror(filename);
			return RRC_MESSAGE_EN_DECODE_ERROR;
	    }
	}


	size = fread(buf, 1, sizeof(buf), fp); 
	fclose(fp);
	if(!size) {
		fprintf(stderr, "%s: Empty or broken\n", filename);
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}
#endif 
	
	

    //!<encode BCH 
    ec = asn_decode(0,ATS_UNALIGNED_BASIC_PER,
                    &asn_DEF_RRCConnectionRequest,
                    (void **)&decode_msg, buf, size); 
   
	fclose(fp);
	AssertFatal(ec.code == RC_OK, RRC, "%s: Broken  decoding at byte %ld\n", __func__,(long)ec.consumed); 
	
	/* Also print the constructed Rectangle XER encoded (XML) */
	//xer_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg); //!<
    //asn_fprint(stdout, &asn_DEF_RRCConnectionRequest, decode_msg);  
    
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */
}


/*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月14日
 * @param: *decode_msg :      [decode output struct ]
 * @param: *buf :             [message buffer ]
 * @param: size :             [message buffer size ]
 */
int DecodeD2dRrcConnectRelease(RRCConnectionRelease_t           *decode_msg,uint8_t *buf,uint32_t size )
{

	FILE *fp;
	asn_dec_rval_t ec; 
	const char *filename; 

#ifdef UPER_ENCODE_TO_FILE
	if(argc < 2) 
	{
		fprintf(stderr, "Specify filename for BER output\n");
	} else 
	{
		filename = argv[1];
		fp = fopen(filename, "rb");
		/* for BER output */
		if(!fp) {
			perror(filename);
			return RRC_MESSAGE_EN_DECODE_ERROR;
	    }
	}


	size = fread(buf, 1, sizeof(buf), fp); 
	fclose(fp);
	if(!size) {
		fprintf(stderr, "%s: Empty or broken\n", filename);
		return RRC_MESSAGE_EN_DECODE_ERROR;
	}
#endif 
	
	

    //!<encode BCH 
    ec = asn_decode(0,ATS_UNALIGNED_BASIC_PER,
                    &asn_DEF_RRCConnectionRelease,
                    (void **)&decode_msg, buf, size); 
   
	fclose(fp);
	AssertFatal(ec.code == RC_OK, RRC, "%s: Broken  decoding at byte %ld\n", __func__,(long)ec.consumed); 
	
	/* Also print the constructed Rectangle XER encoded (XML) */
	//xer_fprint(stdout, &asn_DEF_BCCH_BCH_Message, bch_msg); //!<
    //asn_fprint(stdout, &asn_DEF_RRCConnectionRelease, decode_msg);  
    
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */
}

#endif 

 /*!   
 * @brief:  
 * @author:  bo.liu
 * @Date:  2019年8月16日
 * @param: *decode_msg :      [param description ]
 * @param: *buf :             [param description ]
 * @param: size :             [param description ]
 */
int DecodeD2dCcch(CCCH_Message_t **decode_msg,uint8_t *buf,uint32_t size)
{


	asn_dec_rval_t ec; 
	const char *filename; 
	CCCH_Message_t    *decode_msg_temp = NULL; 

    //!<encode BCH 
    ec = asn_decode(0,ATS_UNALIGNED_BASIC_PER,
                    &asn_DEF_CCCH_Message,
                    (void **)&decode_msg_temp, buf, size); 

    *decode_msg = decode_msg_temp; 
    
	LOG_DEBUG(RRC, "%s, decodoing byte = %d,decode_msg = %x\n", __func__,(long)ec.consumed,*decode_msg);
	AssertFatal(ec.code == RC_OK, RRC, "%s: Broken  decoding at byte %ld\n", __func__,(long)ec.consumed); 
	
    
	
	
	/* Also print the constructed Rectangle XER encoded (XML) */
	//asn_fprint(stdout, &asn_DEF_CCCH_Message, decode_msg); //!<
    //asn_fprint(stdout, &asn_DEF_RRCConnectionRelease, decode_msg);  
    
	return RRC_MESSAGE_EN_DECODE_OK; /* Encoding finished successfully */





}




