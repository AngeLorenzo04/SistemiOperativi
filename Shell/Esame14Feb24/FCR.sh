#!/bin/bash
#File comandi ricorsivo inerente alla prova d'esame del 14 febbraio 2024


#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
X=$2
tmp=$3

#Cerco le occorrenze
for i in *
do
    if test -f $i -a -r $i 
    then
        len=`wc -l  < $i`
        if test $len -ge $X
        then
            echo `pwd`/$i >> $tmp
        fi
    fi
done


#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $X $tmp
   fi
done