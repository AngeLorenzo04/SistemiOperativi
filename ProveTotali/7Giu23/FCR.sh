#!/bin/sh
#FILE COMANDI RICORSIVO INERENTE ALLA PROVA D'ESAME DEL 7GIU23

cd $1

X=$2

file=
cnt=

if test $X -le 0
then
    echo LIVELLO CORRETTO
    echo `pwd` >> $3 #Salvo la directory nel file temporaneo
    for F in *
    do
        if test -f $F -a -r $F
        then
            cnt=`wc -c < $F`
            if test $cnt -le 0
            then
                echo FILE $F VUOTO 
            else
                file="$file `pwd`/$F"

            fi             
        fi
    done

    if test "$file"
    then
        echo DEBUG-C
        main $file
    else
        echo NESSUN FILE RISPETTA LE SPECIFICHE
    fi
    
else
    for D in *
    do
        if test -d $D -a -x $D 
        then
            X=`expr $X - 1`

            FCR.sh $D $X $3
        fi
    done
fi