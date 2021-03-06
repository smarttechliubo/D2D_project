/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "d2dmib.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example`
 */

#ifndef	_MasterInformationBlock_H_
#define	_MasterInformationBlock_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>
#include <BIT_STRING.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum MasterInformationBlock__dl_Bandwidth {
	MasterInformationBlock__dl_Bandwidth_dlBw1dot5	= 0,
	MasterInformationBlock__dl_Bandwidth_dlBw3	= 1,
	MasterInformationBlock__dl_Bandwidth_dlBw6	= 2,
	MasterInformationBlock__dl_Bandwidth_dlBw12	= 3
} e_MasterInformationBlock__dl_Bandwidth;

/* MasterInformationBlock */
typedef struct MasterInformationBlock {
	long	 dl_Bandwidth;
	BIT_STRING_t	 systemFrameNumber;
	BIT_STRING_t	 pdcch_Config;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} MasterInformationBlock_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_dl_Bandwidth_2;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_MasterInformationBlock;

#ifdef __cplusplus
}
#endif

#endif	/* _MasterInformationBlock_H_ */
#include <asn_internal.h>
