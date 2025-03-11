import sys
# insert at 1, 0 is the script path (or '' in REPL)

sys.path.insert(1, '../../src/')

import io

import coap
from scapy.all import *
from scapy.contrib import *

import gen_rulemanager as RM
from compr_parser import *
from protocol import * 
from gen_parameters import T_POSITION_CORE
import binascii

# Create a Rule Manager and upload the rules.
rm = RM.RuleManager()
with open("core_rule.json", "r") as file:
    rule = file.read()
rm.Add(file=io.StringIO(rule))

with open("dev_rule.json", "r") as file:
    rule = file.read()
dev_ip = "aaaa::1"
rm.Add(file=io.StringIO(rule))


def main():
    coap_layer = coap.CoAP()
    coap_layer.fields_desc
    print(coap_layer.ver)
    transport_layer = UDP(dport=5683) 
    IP_layer = IPv6(dst="aaaa::1")


    uncomp_packet = IP_layer / transport_layer / coap_layer 
    p = Parser()
    parsed, res, err = p.parse(bytes(uncomp_packet), T_DIR_DW, layers=["IPv6", "UDP", "CoAP"])
#    print(parsed)
#    print(uncomp_packet)
#    send(uncomp_packet)

    compressed, devid = schc_machine._apply_compression(device_id="aaaa::1", raw_packet=bytes(uncomp_packet), verbose=True)
    if rm.FindRuleFromSCHCpacket(compressed, device="aaaa::1") == None:
        print("fuck")
    schc_machine.process_decompress(compressed, "aaaa::1", T_DIR_DW) 
    
    #eframe = Ether(dst="00:00:00:aa:00:00:00", src="00:00:00:aa:00:01") / compressed.get_content()
    #sendp(eframe, iface="eth0")

# Start SCHC Machine
POSITION = T_POSITION_CORE

# This is a hardcoded port number used by openschc (no ietc port is yet defined for schc)
socket_port = 0x5C4C
schc_machine = SCHCProtocol(role=POSITION)           
schc_machine.set_rulemanager(rm)
scheduler = schc_machine.system.get_scheduler()
tunnel = schc_machine.get_tunnel()

if __name__ == "__main__":
    main()

 
