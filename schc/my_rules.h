/* This file is to only be used in controlled emulation as such, we replace the
 * COMPRESS_IP, COMPRESS_UDP and COMPRESS_COAP macros with our own to be able to control
 * which compression is performed
 *
 */

#include "schc.h"
#include "picocoap.h"

// macros used to set compression at each layer, 1=compress layer
#define COMPRESS_IP 0
#define COMPRESS_UDP 0
#define COMPRESS_COAP 0

//-----------------------------------------------------------------------------
// DECLARING THE IP COMPRESSION RULES
// ----------------------------------------------------------------------------
#if COMPRESS_IP
static const struct schc_ipv6_rule_t ipv6_rule = {
	//	id, up, down, length
		10, 10, 10,
		{
			//	field, 			MO, len, pos,dir, 	val,			MO,				CDA
				{ IP6_V,	 	1, 4,	1, BI, 		{6},			&mo_equal,	 	NOTSENT	   },
				{ IP6_TC, 		1, 8,	1, BI, 		{0},			&mo_ignore, 	VALUESENT	   },
				{ IP6_FL, 		1, 20,	1, BI, 		{0},			&mo_ignore,		VALUESENT	   },
				{ IP6_LEN, 		1, 16,	1, BI, 		{0},			&mo_ignore, 	COMPLENGTH },
				{ IP6_NH, 		1, 8, 	1, BI, 		{17},			&mo_equal,		NOTSENT	},
				{ IP6_HL, 		1, 8, 	1, BI, 		{0}, 	        &mo_ignore, 	VALUESENT	   },

				{ IP6_DEVPRE,	1, 64,	1, BI,		{0xfe, 0x80,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00},			&mo_equal,		NOTSENT 	   },
				{ IP6_DEVIID,	1, 64,	1, BI, 		{0xdc, 0x14, 0x72, 0xff, 0xfe, 0x10, 0xcd, 0x43},			&mo_equal,		NOTSENT	   },
				{ IP6_APPPRE,	1, 64,	1, BI,		{0xfe, 0x80,  0x00, 0x00, 0x00, 0x00, 0x00, 0x00},			&mo_equal,		NOTSENT  },
				{ IP6_APPIID,	1, 64,	1, BI, 	    {0xdc, 0x14, 0x72, 0xff, 0xfe, 0x10, 0xcd, 0x43},			&mo_equal,		NOTSENT  },
		}
};

#else
static const struct schc_ipv6_rule_t ipv6_uncompressed = {
	//	id, up, down, length
		10, 10, 10,
		{
			//	field, 			MO, len, pos,dir, 	val,			MO,				CDA
				{ IP6_V,	 	1, 4,	1, BI, 		{0},			&mo_ignore,		VALUESENT},
				{ IP6_TC, 		1, 8,	1, BI, 		{0},			&mo_ignore,		VALUESENT},
				{ IP6_FL, 		1, 20,	1, BI, 		{0},			&mo_ignore,		VALUESENT},
				{ IP6_LEN, 		1, 16,	1, BI, 		{0},			&mo_ignore,		VALUESENT},
				{ IP6_NH, 		1, 8, 	1, BI, 		{0},			&mo_ignore,		VALUESENT},
				{ IP6_HL, 		1, 8, 	1, BI, 		{0}, 	        &mo_ignore,		VALUESENT},
				{ IP6_DEVPRE,	1, 64,	1, BI,		{0},			&mo_ignore,		VALUESENT},
				{ IP6_DEVIID,	1, 64,	1, BI, 		{0},			&mo_ignore,		VALUESENT},
				{ IP6_APPPRE,	1, 64,	1, BI,		{0},			&mo_ignore,		VALUESENT},
				{ IP6_APPIID,	1, 64,	1, BI, 	    {0},			&mo_ignore,		VALUESENT},
		}
};
#endif

//-----------------------------------------------------------------------------
// DECLARING THE COMPRESSION RULES FOR UDP
// ----------------------------------------------------------------------------
#if COMPRESS_UDP
static const struct schc_udp_rule_t client_udp_pkt = {
//	up, down, length
	4,   4,     4,
	//Fields
	{
		{ UDP_DEV,	1,	16,	1,	BI,	    {0x16, 0x34},	&mo_equal,      NOTSENT},
		{ UDP_APP,	1,	16,	1,	BI,		{0xEA, 0x60},	&mo_equal,	    NOTSENT},
		{ UDP_LEN,	0,	16,	1,	BI,		{0},			&mo_ignore,	    VALUESENT},
		{ UDP_CHK,	0,	16,	1,	BI,		{0},			&mo_ignore,	    VALUESENT},
	}

};

static const struct schc_udp_rule_t server_udp_pkt = {
//	up, down, length
	4,   4,     4,
	//Fields
	{
		{ UDP_DEV,	1,	16,	1,	BI,	    {0xEA, 0x60},	&mo_equal,      NOTSENT},
		{ UDP_APP,	1,	16,	1,	BI,		{0x16, 0x34},	&mo_equal,	    NOTSENT},
		{ UDP_LEN,	0,	16,	1,	BI,		{0},			&mo_ignore,	    VALUESENT},
		{ UDP_CHK,	0,	16,	1,	BI,		{0},			&mo_ignore,	    VALUESENT},
	}
};

#else
static const struct schc_udp_rule_t udp_uncompressed = {
//	up, down, length
	4,   4,     4,
	//Fields
	{
		{ UDP_DEV,	0,	16,	1,	BI,	    {0},	    &mo_ignore,     VALUESENT},
		{ UDP_APP,	0,	16,	1,	BI,		{0},		&mo_ignore,	    VALUESENT},
		{ UDP_LEN,	0,	16,	1,	BI,		{0, 0},		&mo_ignore,	    VALUESENT},
		{ UDP_CHK,	0,	16,	1,	BI,		{0, 0},		&mo_ignore,	    VALUESENT},
	}
};
#endif

//-----------------------------------------------------------------------------
// DECLARING THE COMPRESSION RULES FOR COAP LAYER
// ----------------------------------------------------------------------------
#if COMPRESS_COAP
// GET rule for coap
static const struct schc_coap_rule_t coap_get = {
    5,5,5,
	{
		{COAP_V,    1, 2, 1,    BI,     {COAP_V1},              &mo_equal,      NOTSENT},
		{COAP_T,    2, 2, 1,    BI,     {CT_CON, CT_NON},               &mo_matchmap,   MAPPINGSENT},
		{COAP_TKL,  1, 4, 1,    BI,     {0},                    &mo_ignore,     VALUESENT},
        {COAP_C,    1, 8, 1,    BI,     {CC_GET},               &mo_equal,      NOTSENT},
        {COAP_MID,  1, 16, 1,   BI,     {0},                    &mo_ignore,     VALUESENT},
    }
};

static const struct schc_coap_rule_t coap_post = {
    5,5,5,
	{
		{COAP_V,    1, 2, 1,    BI, {COAP_V1},              &mo_equal,      NOTSENT},
		{COAP_T,    2, 2, 1,    BI, {CT_CON, CT_NON},       &mo_matchmap,   MAPPINGSENT},
		{COAP_TKL,  1, 4, 1,    BI, {0},                    &mo_ignore,     VALUESENT},
        {COAP_C,    1, 8, 1,    BI, {CC_POST},              &mo_equal,      NOTSENT},
        {COAP_MID,  1, 16, 1,   BI, {0},                    &mo_ignore,     VALUESENT},
    }
};


static const struct schc_coap_rule_t coap_valid_ack = {
    5,5,5,
	{
		{COAP_V,    1, 2, 1,    BI, {COAP_V1},          &mo_equal,      NOTSENT},
		{COAP_T,    1, 2, 1,    BI, {CT_ACK},           &mo_equal,      NOTSENT},
		{COAP_TKL,  1, 4, 1,    BI, {0},                &mo_ignore,     VALUESENT},
        {COAP_C,    1, 8, 1,    BI, {CC_VALID},         &mo_equal,      NOTSENT},
        {COAP_MID,  1, 16, 1,   BI, {0},                &mo_ignore,     VALUESENT},
    }
};
#else
static const struct schc_coap_rule_t coap_uncompressed = {
    5,5,5,
	{
		{COAP_V,    1, 2, 1,  BI,   {0},    &mo_ignore, VALUESENT},
		{COAP_T,    1, 2, 1,  BI,   {0},    &mo_ignore, VALUESENT},
		{COAP_TKL,  1, 4, 1,  BI,   {0},    &mo_ignore, VALUESENT},
        {COAP_C,    1, 8, 1,  BI,   {0},    &mo_ignore, VALUESENT},
        {COAP_MID,  1, 16, 1, BI,   {0},    &mo_ignore, VALUESENT},
    }
};
#endif

//-----------------------------------------------------------------------------
// BUILDING THE COMPRESSION RULES
// ----------------------------------------------------------------------------
const struct schc_compression_rule_t client_pkt_compression = {
		.rule_id = 0x01,
		.rule_id_size_bits = 8,

#if COMPRESS_IP
	&ipv6_rule,
#else
    &ipv6_uncompressed,
#endif

#if COMPRESS_UDP
	&client_udp_pkt,
#else
    &udp_uncompressed,
#endif

#if COMPRESS_COAP
	&coap_get,
#else
    &coap_uncompressed,
#endif
};

const struct schc_compression_rule_t server_ack_compression = {
		.rule_id = 0x2,
		.rule_id_size_bits = 8,

#if COMPRESS_IP
	&ipv6_rule,
#else
    &ipv6_uncompressed,
#endif

#if COMPRESS_UDP
	&server_udp_pkt,
#else
    &udp_uncompressed,
#endif

#if COMPRESS_COAP
	&coap_valid_ack,
#else
    &coap_uncompressed,
#endif
};

const struct schc_compression_rule_t server_pkt_compression = {
		.rule_id = 0x3,
		.rule_id_size_bits = 8,

#if COMPRESS_IP
	&ipv6_rule,
#else
    &ipv6_uncompressed,
#endif

#if COMPRESS_UDP
	&server_udp_pkt,
#else
    &udp_uncompressed,
#endif

#if COMPRESS_COAP
	&coap_post,
#else
    &coap_uncompressed,
#endif
};

const struct schc_compression_rule_t client_ack_compression = {
		.rule_id = 0x4,
		.rule_id_size_bits = 8,

#if COMPRESS_IP
	&ipv6_rule,
#else
    &ipv6_uncompressed,
#endif

#if COMPRESS_UDP
	&client_udp_pkt,
#else
    &udp_uncompressed,
#endif

#if COMPRESS_COAP
	&coap_valid_ack,
#else
    &coap_uncompressed,
#endif
};


//-----------------------------------------------------------------------------
// DECLARING THE COMPRESSION DEVICE (in our case its one device that sends to
// itself
// ----------------------------------------------------------------------------
const struct schc_compression_rule_t* node1_compression_rules[] = {
		&client_pkt_compression, &client_ack_compression,
        &server_pkt_compression, &server_ack_compression,
};

const struct schc_device node1 = {
	.device_id = 0x01,
	.uncomp_rule_id = 0,
	.uncomp_rule_id_size_bits = 8,
	.compression_rule_count = 4,
	.compression_context = &node1_compression_rules,
	.fragmentation_rule_count = 0,
	.fragmentation_context = NULL
};

#define DEVICE_COUNT			1

/* server keeps track of multiple devices: add devices to device list */
const struct schc_device* devices[DEVICE_COUNT] = { &node1 };
