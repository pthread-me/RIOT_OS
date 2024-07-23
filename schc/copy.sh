#!/bin/zsh

THIS=(~/RIOT/examples/native/schc)
TO=(~/hub/RIOT_OS)


check_format(){

	if [ -z "$1" ]; then
		echo "must provide a commit message when executing with -g flag"
		exit 1
	fi
}

copy(){
	cp -r $THIS $TO
	echo "copied all the contents of $THIS to $TO"

	rm -r $TO/schc/bin
}


commit(){
	printf "\n------------------------------------------------------------\nOn: " >> $TO/log.txt
	date >> $TO/log.txt
	printf "status:" >> $TO/log.txt

	
	git -C $TO status >> $TO/log.txt
	git -C $TO add -A
	
	printf "\nCommit result:\n" >> $TO/log.txt
	git -C $TO commit -m \""$1"\" >> $TO/log.txt
	echo "check $TO/log.txt for the status of the commit"
}

help(){
	printf "script used in coping contents of $THIS to $TO:\n\t-c:\t\t\tto copy\n\t-g <message>:\t\tto commit\n\t-h:\t\t\tfor help"
}

case "$1" in
	-c|--copy)
		copy
		;;
	-g|--git|--commit)
		check_format $2
		copy
		commit $2
		;;
	*|-h|--help|help)
		help
		;;
esac
