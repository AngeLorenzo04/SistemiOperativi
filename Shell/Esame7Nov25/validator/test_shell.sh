#!/bin/bash
# Use of validator:
# ./test_shell.sh your_shell_script
# if the your_shell_script parameter is missing it defaults to ./esercizio_c

if [ $# -lt 1 ]; then
	SHSCRIPT=./esercizio.sh
else
	SHSCRIPT=$1
	if [[ "$SHSCRIPT" != ./* ]]; then
		SHSCRIPT="./$SHSCRIPT"
	fi
fi
if [ ! -x $SHSCRIPT ]; then
	echo "File $SHSCRIPT not existing or not executable"
	echo "Global result: fail"
	exit
fi

run(){
	DIR=$1
	CHARS=$2
	FILENAME=out_$(basename "$DIR")_$CHARS.txt
	# CARRAY is a bash array
	CARRAY=()
	STRLEN=${#CHARS}
	for (( i=0; i < STRLEN; i++ )); do
	    	C=${CHARS:$i:1} 
	    	CARRAY+=("$C")
	done
	# ${SHSCRIPT} $DIR "${CARRAY[@]}" | sort | tee $FILENAME
	${SHSCRIPT} $DIR "${CARRAY[@]}" | sort > $$
	diff $$ $FILENAME > /dev/null
	if [ $? -eq 0 ]; then
		RES="+"
	else
		RES="-"
		GLOBAL_RES="fail"
	fi
	echo "$RES dir=$DIR, chars=${CARRAY[@]}"
	if [ "$RES" == "-" ]; then
		echo "*** Expecting:"
		cat $FILENAME
		echo "*** Got:"
		cat $$
	fi
	rm -f $$
}

GLOBAL_RES="success"
run g abc
run g/subdir1 abc
run g/subdir2 abc
run g/subdir2 ae
run g/subdir3 ae
run g/subdir3 abc

echo "Global result: $GLOBAL_RES"