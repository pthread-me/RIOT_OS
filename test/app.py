import coap 
import time
from scapy.all import *

with open("samples/uniform.txt", 'r') as file:
    fields = [line.strip().split(' ') for line in file]


pkt_list = []

for field in fields:
    coap_layer = coap.CoAP(options=[("Uri-Path", f"{field[4]}")])
    transport_layer = UDP(dport=5683) 
    IP_layer = IPv6(dst=f"{field[2]}:{field[3]}", src=f"{field[0]}:{field[1]}")
    req = IP_layer / transport_layer / coap_layer
    eframe = Ether(dst="00:00:00:aa:00:00:02", src="00:00:00:aa:00:03") / req
    pkt_list.append(eframe)

for  eframe in pkt_list:
    sendp(eframe, iface="eth0")
