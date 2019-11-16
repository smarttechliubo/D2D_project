/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "d2dccch.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example`
 */

#ifndef	_CCCH_MessageType_H_
#define	_CCCH_MessageType_H_


#include <asn_application.h>

/* Including external dependencies */
#include "RRCConnectionRequest.h"
#include "RRCConnectionSetup.h"
#include "RRCConnectionComplete.h"
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum CCCH_MessageType_PR {
	CCCH_MessageType_PR_NOTHING,	/* No components present */
	CCCH_MessageType_PR_rrcConnectionrequest,
	CCCH_MessageType_PR_rrcConnectionsetup,
	CCCH_MessageType_PR_rrcConectioncomplete
} CCCH_MessageType_PR;

/* CCCH-MessageType */
typedef struct CCCH_MessageType {
	CCCH_MessageType_PR present;
	union CCCH_MessageType_u {
		RRCConnectionRequest_t	 rrcConnectionrequest;
		RRCConnectionSetup_t	 rrcConnectionsetup;
		RRCConnectionComplete_t	 rrcConectioncomplete;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CCCH_MessageType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CCCH_MessageType;
extern asn_CHOICE_specifics_t asn_SPC_CCCH_MessageType_specs_1;
extern asn_TYPE_member_t asn_MBR_CCCH_MessageType_1[3];
extern asn_per_constraints_t asn_PER_type_CCCH_MessageType_constr_1;

#ifdef __cplusplus
}
#endif

#endif	/* _CCCH_MessageType_H_ */
#include <asn_internal.h>