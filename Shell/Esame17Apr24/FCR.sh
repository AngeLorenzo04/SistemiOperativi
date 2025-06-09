#!/bin/sh
#FILE COMANDI  RICORSIVO INERENTE ALLA PROA D'ESAME DEL 17Apr24

cd $1 #Ci spostiamo nella directori da nalizzare

i=0

for F in *
do
    if test $i -eq 3
    then
        if test -f $F
        then
            echo `pwd`/$F >> $2
        fi
    fi
done



