import sys
# insert at 1, 0 is the script path (or '' in REPL)

sys.path.insert(1, '../../src/')

from heuristic import *

import io
import ipaddress
import copy
from itertools import count

import coap 
from scapy.all import *

import gen_rulemanager as RM
from compr_parser import *
from protocol import * 
from gen_parameters import T_POSITION_CORE
import binascii

DEVICE_ID = "1"

NUM_STATIC_RULES = 7 
MATHCING_RULE_SIZE = 3
rule_counter = 1

IP_PADDING = "0000:0000:0000:0000"
# Create a Rule Manager and upload the rules.
rm = RM.RuleManager()
with open("default_rule.json", "r") as file:
    rule = file.read()
rm.Add(file=io.StringIO(rule))
rm.add_sid_file("ietf-schc@2023-01-28.sid")
POSITION = T_POSITION_CORE
schc_machine = SCHCProtocol(role=POSITION)           
schc_machine.set_rulemanager(rm)


def core_to_dev(pkt):
    #print(f"raw len:  {len(bytes(pkt))}")
    compressed, devid = schc_machine._apply_compression(device_id=DEVICE_ID, raw_packet=bytes(pkt), verbose=False)
    
    raw_size = len(bytes(pkt))
    penalty = 0
    comp_size = len(bytes(pkt))
    
    if compressed is not None:
        comp_size = len(compressed._content)
  
    print_stats(raw_size, comp_size, penalty)
    
def dev_to_core(pkt):
    print(pkt)


def process_pkt(pkt):
    if pkt.getlayer(Ether) != None :
        if pkt.getlayer(IPv6) != None:
            core_to_dev(pkt[IPv6])
        else:
            dev_to_core(pkt[Raw])
  

def print_stats(raw_size, comp_size, penalty):
    print(f"{raw_size}, {comp_size}, {penalty}")


def prepare_rules(rules):
    with open(rules, "r") as file:
        for line in file:
            rm._ctxt[0]['SoR'].append(get_rule(line))

def get_rule(line):
    a_pre, a_id, d_pre, d_id, uri = line.strip().split(" ")

    a_pre = ipaddress.IPv6Address(a_pre+f":{IP_PADDING}")
    a_id = ipaddress.IPv6Address(f"{IP_PADDING}:" + a_id)

    d_pre = ipaddress.IPv6Address(d_pre+ f":{IP_PADDING}")
    d_id = ipaddress.IPv6Address(f"{IP_PADDING}:" + d_id)

    compression = [
            {"FID": "IPV6.VER",  "TV": 6, "MO": "equal", "CDA": "not-sent"},
            {"FID": "IPV6.TC",  "TV": 0, "MO": "ignore", "CDA": "value-sent"},
            {"FID": "IPV6.FL",  "TV": 0, "MO": "ignore","CDA": "value-sent"},
            {"FID": "IPV6.LEN",          "MO": "ignore","CDA": "compute-length"},
            {"FID": "IPV6.NXT", "TV": 17, "MO": "ignore", "CDA": "value-sent"},
 
            {"FID": "IPV6.DEV_PREFIX","TV": str(d_pre)+"/64", "MO": "equal","CDA": "not-sent"},
            {"FID": "IPV6.DEV_IID", "TV": str(d_id)+"/64","MO": "equal","CDA": "not-sent"},
            {"FID": "IPV6.APP_PREFIX", "TV": str(a_pre)+"/64", "MO": "equal","CDA": "not-sent"},
            {"FID": "IPV6.APP_IID", "TV": str(a_id)+"/64", "MO": "equal","CDA": "not-sent"},
            {"FID": "UDP.DEV_PORT",  "TV": 5683,"MO": "equal",  "CDA": "not-sent"},
            {"FID": "UDP.APP_PORT",             "MO": "ignore",  "CDA": "value-sent"},
            {"FID": "UDP.LEN",       "TV": 0,   "MO": "ignore", "CDA": "compute-length"},
            {"FID": "UDP.CKSUM",     "TV": 0,   "MO": "ignore", "CDA": "compute-checksum"},
            {"FID": "COAP.TKL", "TV": 0,                   "MO": "equal","CDA": "not-sent"},
            {"FID": "COAP.Uri-Path", "FP": 1, "DI": "BI", "TV": uri, "MO": "equal","CDA": "not-sent"},
    ]
  
    global rule_counter
    rule = {"RuleIDValue": copy.deepcopy(rule_counter), "RuleIDLength": 3, "Compression": copy.deepcopy(compression)}
    rule_counter = rule_counter + 1
    return rm._create_compression_rule(rule, device_id=1)
    

if __name__ == "__main__":
    
    prepare_rules("rules/v2/uniform.rule")
    #print("sniffing")
    sniff(prn=process_pkt, filter="not icmp6", iface=["eth0", "eth1"])
