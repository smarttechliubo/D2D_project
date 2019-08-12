/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "EUTRA-RRC-Definitions"
 * 	found in "d2dsib1.asn1"
 * 	`asn1c -pdu=all -fcompound-names -gen-PER -no-gen-OER -no-gen-example`
 */

#include "SystemInformationBlockType1.h"

static asn_TYPE_member_t asn_MBR_SystemInformationBlockType1_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SystemInformationBlockType1, p_Max),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_P_Max,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"p-Max"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SystemInformationBlockType1, tdd_Config),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_TDD_Config,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"tdd-Config"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SystemInformationBlockType1, carrierFreq_info),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Carrier,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"carrierFreq-info"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SystemInformationBlockType1, radioResourceConfigCommon),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RadioResourceConfigCommonSIB,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"radioResourceConfigCommon"
		},
};
static const ber_tlv_tag_t asn_DEF_SystemInformationBlockType1_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_SystemInformationBlockType1_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* p-Max */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* tdd-Config */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* carrierFreq-info */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* radioResourceConfigCommon */
};
static asn_SEQUENCE_specifics_t asn_SPC_SystemInformationBlockType1_specs_1 = {
	sizeof(struct SystemInformationBlockType1),
	offsetof(struct SystemInformationBlockType1, _asn_ctx),
	asn_MAP_SystemInformationBlockType1_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_SystemInformationBlockType1 = {
	"SystemInformationBlockType1",
	"SystemInformationBlockType1",
	&asn_OP_SEQUENCE,
	asn_DEF_SystemInformationBlockType1_tags_1,
	sizeof(asn_DEF_SystemInformationBlockType1_tags_1)
		/sizeof(asn_DEF_SystemInformationBlockType1_tags_1[0]), /* 1 */
	asn_DEF_SystemInformationBlockType1_tags_1,	/* Same as above */
	sizeof(asn_DEF_SystemInformationBlockType1_tags_1)
		/sizeof(asn_DEF_SystemInformationBlockType1_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_SystemInformationBlockType1_1,
	4,	/* Elements count */
	&asn_SPC_SystemInformationBlockType1_specs_1	/* Additional specs */
};

