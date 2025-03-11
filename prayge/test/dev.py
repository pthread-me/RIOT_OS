import sys
sys.path.insert(1, '../../src/')


import io

import coap
from scapy.all import *
from scapy.contrib import *

import gen_rulemanager as RM
from compr_parser import *
from protocol import * 
from gen_parameters import *
import binascii


rm = RM.RuleManager()
with open("rule.json", "r") as file:
    rule = file.read()

rm.Add(file=io.StringIO(rule))

def schc_packet_recv(pkt):
    print("----------------------------------")
    print("got pkt")
    print(pkt)
    print("----------------------------------")
   
    if pkt.getlayer(Ether) != None:
        print("Ether with raw data:")
        if pkt[Raw] != None:
            src_mac = pkt[Ether].src
            compressed_pkt = BitBuffer(bytes(pkt[Raw]))
            print(compressed_pkt)        
            schc_machine.process_decompress(compressed_pkt, src_mac, T_DIR_DW) 
            
            #print(bytes(pkt[Raw]))


#origin, decompressed = schc_machine.schc_recv(schc_packet=packet, verbose=True)


schc_machine = SCHCProtocol(role=T_POSITION_DEVICE)
schc_machine.set_rulemanager(rm)
scheduler = schc_machine.system.get_scheduler()
tunnel = schc_machine.get_tunnel()

sniff(iface="eth0", filter="not icmp6", prn=schc_packet_recv, store=False)


