APPLICATION = schc

RIOTBASE = $(HOME)/RIOT
PKGBASE = $(HOME)/RIOT/pkg
BOARD ?= native

QUIET ?= 1

DEVELHELP = 1
CFLAGS += -DDEVELHELP
CFLAGS += -DSCHEDSTATISTICS

USEPKG += libschc
USEMODULE += libschc_udpv6
USEMODULE += libschc_coap

USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_ipv6_ext
USEMODULE += gnrc_nettype_udp
USEMODULE += gnrc_udp
USEMODULE += gnrc_pktbuf
USEMODULE += nanocoap

include $(RIOTBASE)/Makefile.include
