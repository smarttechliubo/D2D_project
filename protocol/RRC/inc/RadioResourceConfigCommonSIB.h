/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "d2dsib1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example`
 */

#ifndef	_RadioResourceConfigCommonSIB_H_
#define	_RadioResourceConfigCommonSIB_H_


#include <asn_application.h>

/* Including external dependencies */
#include "PuschHoppingConfig.h"
#include "UlRefSigConfig-PUSCH.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RadioResourceConfigCommonSIB */
typedef struct RadioResourceConfigCommonSIB {
	PuschHoppingConfig_t	 psush_Hop_Config;
	UlRefSigConfig_PUSCH_t	 ul_ref_signal_pusch;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RadioResourceConfigCommonSIB_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RadioResourceConfigCommonSIB;
extern asn_SEQUENCE_specifics_t asn_SPC_RadioResourceConfigCommonSIB_specs_1;
extern asn_TYPE_member_t asn_MBR_RadioResourceConfigCommonSIB_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _RadioResourceConfigCommonSIB_H_ */
#include <asn_internal.h>