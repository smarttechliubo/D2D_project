/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "d2dconnectsetup.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example`
 */

#ifndef	_SN_FieldLength_H_
#define	_SN_FieldLength_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SN_FieldLength {
	SN_FieldLength_size5	= 0,
	SN_FieldLength_size10	= 1
} e_SN_FieldLength;

/* SN-FieldLength */
typedef long	 SN_FieldLength_t;

/* Implementation */
extern asn_per_constraints_t asn_PER_type_SN_FieldLength_constr_1;
extern asn_TYPE_descriptor_t asn_DEF_SN_FieldLength;
extern const asn_INTEGER_specifics_t asn_SPC_SN_FieldLength_specs_1;
asn_struct_free_f SN_FieldLength_free;
asn_struct_print_f SN_FieldLength_print;
asn_constr_check_f SN_FieldLength_constraint;
ber_type_decoder_f SN_FieldLength_decode_ber;
der_type_encoder_f SN_FieldLength_encode_der;
xer_type_decoder_f SN_FieldLength_decode_xer;
xer_type_encoder_f SN_FieldLength_encode_xer;
per_type_decoder_f SN_FieldLength_decode_uper;
per_type_encoder_f SN_FieldLength_encode_uper;
per_type_decoder_f SN_FieldLength_decode_aper;
per_type_encoder_f SN_FieldLength_encode_aper;

#ifdef __cplusplus
}
#endif

#endif	/* _SN_FieldLength_H_ */
#include <asn_internal.h>
