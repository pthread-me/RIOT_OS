#!/usr/bin/env python3
dev_ip = "aaaa::1"

import coap 
import time
from scapy.all import *


def p1(i):
    if i == 0:
        coap_layer = coap.CoAP(options=[("Uri-Path", "zuuuuuu")])
        transport_layer = UDP(dport=5683) 
        IP_layer = IPv6(dst="aaaa::1", src="f::1")
        req = IP_layer / transport_layer / coap_layer   
    elif i ==1:
        coap_layer = coap.CoAP(options=[("Uri-Path", "first")])
        transport_layer = UDP(dport=5683) 
        IP_layer = IPv6(dst="bbbb::2", src="c::2")
        req = IP_layer / transport_layer / coap_layer   
    else:
        coap_layer = coap.CoAP(options=[("Uri-Path", "second")])
        transport_layer = UDP(dport=5683) 
        IP_layer = IPv6(dst="aaaa::1", src="f::1")
        req = IP_layer / transport_layer / coap_layer
    
    eframe = Ether(dst="00:00:00:aa:00:00:02", src="00:00:00:aa:00:03") / req
    sendp(eframe, iface="eth0") 


if __name__ == "__main__":
    while(True):
        p = input()
        if p == "1":
            p1(1)
        elif p == "2":
            p1(2)
        else:
            p1(0)
