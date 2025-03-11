#!/usr/bin/env python3

from aiocoap import *
from scapy.all import *




dev_ip = "2001::14"

def main():
    request = Message(code=GET, uri='coap://['+dev_ip+']]/test', mid=1, mtype=1)
    print(request.encode())





if __name__ == "__main__":
    main()
