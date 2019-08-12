/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "d2dconnectsetup.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example`
 */

#ifndef	_PollPDU_v1310_H_
#define	_PollPDU_v1310_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum PollPDU_v1310 {
	PollPDU_v1310_p512	= 0,
	PollPDU_v1310_p1024	= 1,
	PollPDU_v1310_p2048	= 2,
	PollPDU_v1310_p4096	= 3,
	PollPDU_v1310_p6144	= 4,
	PollPDU_v1310_p8192	= 5,
	PollPDU_v1310_p12288	= 6,
	PollPDU_v1310_p16384	= 7
} e_PollPDU_v1310;

/* PollPDU-v1310 */
typedef long	 PollPDU_v1310_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PollPDU_v1310;
asn_struct_free_f PollPDU_v1310_free;
asn_struct_print_f PollPDU_v1310_print;
asn_constr_check_f PollPDU_v1310_constraint;
ber_type_decoder_f PollPDU_v1310_decode_ber;
der_type_encoder_f PollPDU_v1310_encode_der;
xer_type_decoder_f PollPDU_v1310_decode_xer;
xer_type_encoder_f PollPDU_v1310_encode_xer;
per_type_decoder_f PollPDU_v1310_decode_uper;
per_type_encoder_f PollPDU_v1310_encode_uper;
per_type_decoder_f PollPDU_v1310_decode_aper;
per_type_encoder_f PollPDU_v1310_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _PollPDU_v1310_H_ */
#include <asn_internal.h>
