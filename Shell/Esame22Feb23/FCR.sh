#!/bin/bash
#FIle Comandi Ricorsivo inerente alla prova d'esame del 22 Febraio 2023

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
X=$2
tmp=$3

#Cerco le occorrenze all'interno della gerarchia
for F in *
do
    if test -f $F 
    then
        len=`wc -l < $F`
        if test $len -lt $X
        then
            echo `pwd`/$F >> $tmp
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