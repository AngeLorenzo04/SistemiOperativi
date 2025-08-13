#!/bin/bash
#File comandi ricorsivo inerente alla prova d'esame del 10Lug2024

#Mi sposto nella directory corrente
cd $1

#Salvo i parametri nelle variabili
X=$2
tmp=$3

#Cerco le occorrenze  che rispettano la specifica
for i in *
do
    if test -f $i -a -x $i
    then
        line=`wc -l < $i`
        if test $line -gt $X
        then
            echo `pwd`/$i >> $tmp #Salvo il l'occorrenza nel file temporaneo
        fi
    fi
done

for i in *
do
        if test -d $i -a -x $i
        then
                #invocazione ricorsiva
                $0 `pwd`/$i $N $tmp     #sempre passaggio del nome assoluto della dir
        fi
done

