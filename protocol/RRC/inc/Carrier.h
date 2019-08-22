/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "d2dsib1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example`
 */

#ifndef	_Carrier_H_
#define	_Carrier_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum Carrier__band_info {
	Carrier__band_info_band0	= 0,
	Carrier__band_info_band1	= 1,
	Carrier__band_info_band2	= 2,
	Carrier__band_info_band3	= 3,
	Carrier__band_info_band4	= 4,
	Carrier__band_info_band5	= 5,
	Carrier__band_info_band6	= 6,
	Carrier__band_info_band7	= 7
} e_Carrier__band_info;

/* Carrier */
typedef struct Carrier {
	long	 band_info;
	long	 ul_carrier_freq;
	long	 dl_carrier_freq;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Carrier_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_band_info_2;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_Carrier;
extern asn_SEQUENCE_specifics_t asn_SPC_Carrier_specs_1;
extern asn_TYPE_member_t asn_MBR_Carrier_1[3];

#ifdef __cplusplus
}
#endif

#endif	/* _Carrier_H_ */
#include <asn_internal.h>