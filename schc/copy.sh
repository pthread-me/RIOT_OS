#!/bin/zsh

THIS=(~/RIOT/examples/native/schc)
TO=(~/hub/RIOT_OS)

check_format(){
	if [ "$2" == "" ]; then
		echo "must provide a commit message when executing with -g flag"
		exit 1
	fi
}

copy(){
	cp -r $THIS $TO
	echo "copied all the contents of $THIS to $TO"
}


commit(){
	printf "\n------------------------------------------------------------\nOn: " >> $TO/log.txt
	date >> $TO/log.txt
	printf "status:" >> $TO/log.txt

	
	(cd $TO git status) >> $TO/log.txt
	(cd $TO git add -A)
	
	printf "\nCommit result:\n" >> $TO/log.txt
	(cd $TO git commit -m "$2") >> $TO/log.txt
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
		check_format
		commit
		;;
	*|-h|--help|help)
		help
		;;
esac
