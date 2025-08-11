#!/bin/bash
#FILE COMANDI RICORSIVO INERENTE ALLA PROVE D'ESAME DEL 11 GIUGNO 2025

echo DEBUG-FCR
cd $1

check=0
for i in *
do
    echo DEBUG-ELABORO $i
    if test -f $i
    then
        for parola in `cat $i`
        do
            echo DEBUG-PAROLA $parola
            for target in $(echo $2 | fold -w1)
            do
                echo DEBUG-TARGET $target
                if ! grep $target $i
                then
                    check=1
                fi
            done
            echo CHECK-$check

            if test $check -eq 1 
            then
                echo NON TUTTI I CARATTERI SONO PRESENTI
            fi
            check=0

        done
    fi

done

for i in *
do
        if test -d $i -a -x $i
        then
                #invocazione ricorsiva
                $0 `pwd`/$i $2 $3       #sempre passaggio del nome assoluto della dir
        fi
done