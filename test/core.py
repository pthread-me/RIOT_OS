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

rule_counter = 1
DEVICE_ID = 1
PKT_ID = 1
IP_PADDING = "0000000000000000"

# Create a Rule Manager and upload the rules.
rm = RM.RuleManager()
with open("default_rule.json", "r") as file:
    rule = file.read()
rm.Add(file=io.StringIO(rule))

POSITION = T_POSITION_CORE
schc_machine = SCHCProtocol(role=POSITION)           
schc_machine.set_rulemanager(rm)


uri_queue   = PrioQueue()
app_pre     = PrioQueue()
app_id      = PrioQueue()
dev_pre     = PrioQueue()
dev_id      = PrioQueue()

# Find the pkt_id with the highest cummulative prio and create a rule for it. Then find the top 3 values of each
# field and have a mapping rule for them.
def update_rules():
    rm._ctxt[0]['SoR'] = rm._ctxt[0]['SoR'][:1]
    rm._ctxt[0]['SoR'].append(equality_rules())
    rm._ctxt[0]['SoR'].append(matching_rules())

def matching_rules():
    a_pre = []
    a_id = []
    d_pre = []
    d_id = []
    uri = []
    n = min(len(uri_queue), len(app_pre), len(app_id), len(dev_pre), len(dev_id))

    for e in app_pre[:n]:
        a_pre.append(str(ipaddress.IPv6Address(int(e.value+IP_PADDING, 16))) + "/64")
    for e in app_id[:n]:
        a_id.append(str(ipaddress.IPv6Address(int(IP_PADDING+e.value, 16))) + "/64")
    for e in dev_pre[:n]:
        d_pre.append(str(ipaddress.IPv6Address(int(e.value+IP_PADDING, 16))) + "/64")
    for e in dev_id[:n]:
        d_id.append(str(ipaddress.IPv6Address(int(IP_PADDING+e.value, 16)))+"/64"  )
    
    for e in uri_queue[:n]:
        uri.append(e.value[0].decode('utf-8'))

    print(uri)
    compression = [
            {"FID": "IPV6.VER",  "TV": 6, "MO": "equal", "CDA": "not-sent"},
            {"FID": "IPV6.TC",  "TV": 0, "MO": "ignore", "CDA": "value-sent"},
            {"FID": "IPV6.FL",  "TV": 0, "MO": "ignore","CDA": "value-sent"},
            {"FID": "IPV6.LEN",          "MO": "ignore","CDA": "compute-length"},
            {"FID": "IPV6.NXT", "TV": 17, "MO": "ignore", "CDA": "value-sent"},
 
            {"FID": "IPV6.DEV_PREFIX","TV": d_pre, "MO": "match-mapping","CDA": "mapping-sent"},
            {"FID": "IPV6.DEV_IID", "TV": d_id,"MO": "match-mapping","CDA": "mapping-sent"},
            {"FID": "IPV6.APP_PREFIX", "TV": a_pre, "MO": "match-mapping","CDA": "mapping-sent"},
            {"FID": "IPV6.APP_IID", "TV": a_id, "MO": "match-mapping","CDA": "mapping-sent"},
            {"FID": "UDP.DEV_PORT",  "TV": 5683,"MO": "equal",  "CDA": "value-sent"},
            {"FID": "UDP.APP_PORT",             "MO": "ignore",  "CDA": "value-sent"},
            {"FID": "UDP.LEN",       "TV": 0,   "MO": "ignore", "CDA": "compute-length"},
            {"FID": "UDP.CKSUM",     "TV": 0,   "MO": "ignore", "CDA": "compute-checksum"},
            {"FID": "COAP.Uri-Path", "FP": 1, "DI": "DW", "TV": uri, "MO": "match-mapping","CDA": "mapping-sent"},

    ]
  
    global rule_counter
    rule = {"RuleIDValue": copy.deepcopy(rule_counter), "RuleIDLength": 3, "Compression": copy.deepcopy(compression)}
    rule_counter = rule_counter + 1
    if rule_counter == 3:
        rule_counter = 1
    return rm._create_compression_rule(copy.deepcopy(rule), device_id=1)
 

def equality_rules():
    i = find_max_id()
    a_pre = ipaddress.IPv6Address(int(app_pre.getid(i)+IP_PADDING, 16))
    a_id = ipaddress.IPv6Address(int(IP_PADDING+app_id.getid(i), 16))

    d_pre = ipaddress.IPv6Address(int(dev_pre.getid(i)+IP_PADDING, 16))
    d_id = ipaddress.IPv6Address(int(IP_PADDING+dev_id.getid(i), 16))

    uri = uri_queue.getid(i)[0].decode('utf-8')

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
            {"FID": "UDP.DEV_PORT",  "TV": 5683,"MO": "equal",  "CDA": "value-sent"},
            {"FID": "UDP.APP_PORT",             "MO": "ignore",  "CDA": "value-sent"},
            {"FID": "UDP.LEN",       "TV": 0,   "MO": "ignore", "CDA": "compute-length"},
            {"FID": "UDP.CKSUM",     "TV": 0,   "MO": "ignore", "CDA": "compute-checksum"},
            {"FID": "COAP.Uri-Path", "FP": 1, "DI": "BI", "TV": uri, "MO": "equal","CDA": "not-sent"},
    ]
  
    global rule_counter
    rule = {"RuleIDValue": copy.deepcopy(rule_counter), "RuleIDLength": 3, "Compression": copy.deepcopy(compression)}
    rule_counter = rule_counter + 1
    if rule_counter == 3:
        rule_counter = 1

    return rm._create_compression_rule(rule, device_id=1)
    

def find_max_id():
    id_scores: Dict[int, int] = {}
    lists = [uri_queue, app_pre, app_id, dev_pre, dev_id]
    for l in lists:
        print(l)
        for f in l:
            if f.id in id_scores:
                id_scores[f.id] = id_scores[f.id] + f.prio
            else:
                id_scores[f.id] = f.prio

    return max(id_scores, key=id_scores.get)


def core_to_dev(pkt):
    print("\n\nUncompressed:")
    print(bytes(pkt))
    print(len(bytes(pkt)))
    compressed, devid = schc_machine._apply_compression(device_id="1", raw_packet=bytes(pkt), verbose=False)
    
    print("\nCompressed:")
    print(compressed)
    if compressed is not None:
        print(len(compressed._content))
# eframe = Ether(dst="00:00:00:aa:00:00:00", src="00:00:00:aa:00:01") / compressed.get_content()
#    sendp(eframe, iface="eth0")

    clasify(pkt)
    global PKT_ID
    #print(PKT_ID)
    PKT_ID = PKT_ID +1
    

    if PKT_ID%3 ==0:
        uri_queue.degrade()
        app_pre.degrade()
        app_id.degrade()
        dev_pre.degrade()
        dev_id.degrade()
    elif PKT_ID%5==0:
        update_rules()

def clasify(uncomp_packet):
    p = Parser()
    fields, res, err = p.parse(bytes(uncomp_packet), T_DIR_DW, layers=["IPv6", "UDP", "CoAP"])

    dev_pre.add(QField(1, PKT_ID, fields[('IPV6.DEV_PREFIX', 1)][0].hex()))
    dev_id.add(QField(1, PKT_ID, fields[('IPV6.DEV_IID', 1)][0].hex()))
    app_pre.add(QField(1, PKT_ID, fields[('IPV6.APP_PREFIX', 1)][0].hex()))
    app_id.add(QField(1,PKT_ID, fields[('IPV6.APP_IID', 1)][0].hex()))
    
    uri_list = []
    for i in itertools.count(start=1):
        try:
            uri_list.append(fields[('COAP.URI-PATH', i)][0])
        except:
            break
        
    uri_queue.add(QField(1, PKT_ID, uri_list))

def dev_to_core(pkt):
    print(pkt)


def process_pkt(pkt):
    if pkt.getlayer(Ether) != None :
        if pkt.getlayer(IPv6) != None:
            core_to_dev(pkt[IPv6])
        else:
            dev_to_core(pkt[Raw])
  
#    print("uri\n" + str(uri_queue))
#    print("app pre\n"+str(app_pre))   
#    print("app id\n"+str(app_id)) 
#    print("dev pre\n"+str(dev_pre))
#    print("dev_id\n"+str(dev_id))




if __name__ == "__main__":
    
    #   To be used in actual test as follows, we: 
    #       1) sniff for packets reaching our iot "subnetwork" 
    #       2)check if the dst addr is present
    #       3) compress the pkt with the rule assosiated with the dst device -> send schcpkt
    print("sniffing")
   
  
    sniff(prn=process_pkt, filter="not icmp6", iface=["eth0", "eth1"])
    #main()
    #print(recents)
