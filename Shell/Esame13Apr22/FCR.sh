#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 13 Aprile 2022

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
X=$2
tmp=$3

for i in *
do 
    if test -f "$i" 
    then
        len=`wc -l < "$i"`
        if test $len -eq $X
        then
            echo `pwd`/"$i" >> $tmp
        fi
    fi
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d "$i" -a -x "$i"
   then
      $0 "`pwd`/$i" $X $tmp
   fi
done