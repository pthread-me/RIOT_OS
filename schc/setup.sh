#!/bin/zsh

BRIDGE_NUM=1
INTERFACE_NUM=2

create(){	 
	echo "creating $BRIDGE_NUM bridges"
	for ((i=0; i<BRIDGE_NUM; i++)) 
	do
		sudo ip link add name br$i type bridge 
		sudo ip link set dev br$i up
	done
	
	
	echo "creating $INTERFACE_NUM tap interfaces"
	for ((i=0; i<INTERFACE_NUM; i++)) 
	do
		sudo ip tuntap add tap$i mode tap
		sudo ip link set dev tap$i up
	done
}

delete(){
	echo "deleting $BRIDGE_NUM bridges"
	for ((i=0; i<BRIDGE_NUM; i++)) 
	do
		sudo ip link delete br$i  
	done

	echo "deleting $INTERFACE_NUM tap interfaces"
	for ((i=0; i<INTERFACE_NUM; i++)) 
	do
		sudo ip link delete tap$i 
	done
}


#to be hardcoded
setup(){

	sudo ip link set dev tap0 master br0
	sudo ip link set dev tap1 master br0
	
	#add OPTIONs as desired
	sudo tc qdisc add dev tap0 root netem 
	sudo tc qdisc add dev tap1 root netem 
}

help(){
	printf "Call the script with sudo user privlage, and ONE of the options bellow
		-c: to create taps and bridges
		-d: to delete all the taps and bridges created by -c
		-h: for help"
}


case "$1" in
	-d)
		delete
		;;
	-c)
		create
		setup
		;;
	-h|*)
		help
		;;
esac
