#include "../schc.h"

/* first build the compression rules separately per layer */

#if USE_IP6
static const struct schc_ipv6_rule_t ipv6_rule1 = {
	//	id, up, down, length
		10, 10, 10,
		{
			//	field, 			MO, len, pos,dir, 	val,			MO,				CDA
				{ IP6_V,	 	0, 4,	1, BI, 		{6},			&mo_ignore, 		NOTSENT  },
				{ IP6_TC, 		0, 8,	1, BI, 		{0},			&mo_ignore, 	NOTSENT  },
				{ IP6_FL, 		0, 20,	1, BI, 		{0, 0, 0},		&mo_ignore, 	NOTSENT  },
				{ IP6_LEN, 		0, 16,	1, BI, 		{0, 0},			&mo_ignore, 	NOTSENT  },
				{ IP6_NH, 		3, 8, 	1, BI, 		{6, 17, 58},	&mo_ignore, 	NOTSENT  },
				{ IP6_HL, 		0, 8, 	1, BI, 		{64}, 			&mo_ignore, 	NOTSENT  },
				{ IP6_DEVPRE,	4, 64,	1, BI,		{0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
														 0xBB, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
														 0xCC, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
														 0xDD, 0xDD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						&mo_ignore, 	NOTSENT  }, // you can store as many IP's as (MAX_FIELD_LENGTH / 8)
				{ IP6_DEVIID,	60, 64,	1, BI, 		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
						&mo_ignore, 		NOTSENT },
				{ IP6_APPPRE,	0, 64,	1, BI,		{0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						&mo_ignore, 		NOTSENT  },
				{ IP6_APPIID,	60, 64,	1, BI, 	    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
						&mo_ignore, 		NOTSENT }, // match the 60 first bits, send the last 4
		}
};
#endif


#if USE_UDP
static const struct schc_udp_rule_t udp_rule1 = {
//	up, down, length	
	4,   4,     4,
	
	//Fields
	{
		{ UDP_DEV,	0,	16,	1,	BI,	{0x90, 0x1F},	&mo_equal,	NOTSENT},
		{ UDP_APP,	0,	16,	1,	BI,	{040, 0x1F},	&mo_equal,	NOTSENT},
		{ UDP_LEN,	0,	16,	1,	BI,	{0, 0},			&mo_ignore,	NOTSENT},
		{ UDP_CHK,	0,	16,	1,	BI,	{0, 0},			&mo_ignore,	NOTSENT},
	}
	
};
#endif

/* next build the compression rules from the rules that make up a single layer */
const struct schc_compression_rule_t compression_rule_1 = {
		.rule_id = 0x01,
		.rule_id_size_bits = 8,

#if USE_IP6
	&ipv6_rule1,
#endif

#if USE_UDP
		&udp_rule1,
#endif
};


/* save compression rules in flash */
const struct schc_compression_rule_t* node1_compression_rules[] = {
		&compression_rule_1
};

const struct schc_device node1 = {
	.device_id = 0x01,
	.uncomp_rule_id = 0,
	.uncomp_rule_id_size_bits = 8,	
	.compression_rule_count = 1,
	.compression_context = &node1_compression_rules,
	.fragmentation_rule_count = 0,
	.fragmentation_context = NULL
};

#define DEVICE_COUNT			1

/* server keeps track of multiple devices: add devices to device list */
const struct schc_device* devices[DEVICE_COUNT] = { &node1 };
