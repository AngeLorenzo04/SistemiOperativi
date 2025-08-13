#!/bin/bash
#Fil comandi ricorsivo inerente alla prova d'esame del 11 Settembre 2024

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
X=$2
turn=$3
tmp=$4

cont=0

if test $turn -eq $X
then
    for F in *
    do 
        if test -f $F -a -r $f
        then
            cont=`expr $cont + 1`
            if test $cont -eq 2
            then
                echo `pwd` >> $tmp
            fi
        fi
    done
fi

turn=`expr $turn + 1 `

for i in *
do 
    if test -d $i -a -x $i
    then
        $0 `pwd`/$i $X $turn $tmp
    fi
done