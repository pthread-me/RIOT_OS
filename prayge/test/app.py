#!/usr/bin/env python3
dev_ip = "aaaa::1"

import coap 
from scapy.all import *


def main():
    coap_layer = coap.CoAP(options=[("Uri-Path", "test"), ("Uri-Path", "folder")])
    transport_layer = UDP(dport=5683) 
    IP_layer = IPv6(dst="aaaa::1")
    req = IP_layer / transport_layer / coap_layer   
    send(req) 


if __name__ == "__main__":
    main()
