import sys
# insert at 1, 0 is the script path (or '' in REPL)

sys.path.insert(1, '../../src/')

from heuristic import *

import io
from itertools import count
import threading
lock = threading.Lock()

import coap 
from scapy.all import *

import gen_rulemanager as RM
from compr_parser import *
from protocol import * 
from gen_parameters import T_POSITION_CORE
import binascii


MAX_FIELDS_STORED = 64

# Create a Rule Manager and upload the rules.
rm = RM.RuleManager()
with open("test_rule.json", "r") as file:
    rule = file.read()
rm.Add(file=io.StringIO(rule))

POSITION = T_POSITION_CORE
schc_machine = SCHCProtocol(role=POSITION)           
schc_machine.set_rulemanager(rm)

#temporary structure to divide and hold the values
fields_dictionary: Dict[str, List[any]] = {"dev_ip_id": [], "dev_ip_pre": [], "app_ip_id": [], "app_ip_pre": [], "uri": [] }


def core_to_dev(pkt):
    clasify(pkt)
    compressed, devid = schc_machine._apply_compression(device_id="1", raw_packet=bytes(pkt), verbose=False)
    
    eframe = Ether(dst="00:00:00:aa:00:00:00", src="00:00:00:aa:00:01") / compressed.get_content()
    sendp(eframe, iface="eth0")


def clasify(uncomp_packet):
    p = Parser()
    fields, res, err = p.parse(bytes(uncomp_packet), T_DIR_DW, layers=["IPv6", "UDP", "CoAP"])
    current = {}
    current["dev_ip_pre"] = (fields[('IPV6.DEV_PREFIX', 1)][0]).hex()
    current["dev_ip_id"] = fields[('IPV6.DEV_IID', 1)][0].hex() 
    current["app_ip_id"] = fields[('IPV6.APP_PREFIX', 1)][0].hex()
    current["app_ip_id"] = fields[('IPV6.APP_IID', 1)][0].hex()
   
    uri_list = []
    for i in itertools.count(start=1):
        try:
            uri_list.append(fields[('COAP.URI-PATH', i)][0])
        except:
            break
    
    current["uri"] = uri_list
    update_dictionary(current)
    print(fields_dictionary)


def update_dictionary(fields):
    for f in fields:
        update_field_list(f, fields[f])

def update_field_list(field_name: str, value: any):
    if field_name in fields_dictionary:
        with lock:
            if len(fields_dictionary[field_name]) == 0 or value not in fields_dictionary[field_name]:
                fields_dictionary[field_name].append(value)

def dev_to_core(pkt):
    print(pkt)


def process_pkt(pkt):
    if pkt.getlayer(Ether) != None :
        if pkt.getlayer(IPv6) != None:
            core_to_dev(pkt[IPv6])
        else:
            dev_to_core(pkt[Raw])
            


if __name__ == "__main__":
    
    #   To be used in actual test as follows, we: 
    #       1) sniff for packets reaching our iot "subnetwork" 
    #       2)check if the dst addr is present
    #       3) compress the pkt with the rule assosiated with the dst device -> send schcpkt
    print("sniffing")
    
    sniff(prn=process_pkt, filter="not icmp6", iface=["eth0", "eth1"])

    #main()
    #print(recents)
