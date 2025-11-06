#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 10 Luglio 2024

#Mi sposto nella direcotry desiderata
cd $1

#Salvo i parametri in delle variabili
X=$2
tmp=$3

#Cerco le varie occorrenze
for i in * 
do 
    if test -f $i -a -r $i 
    then
        len=`wc -l < $i`
        if test $len -gt $X -a $len -lt 255
        then
            echo `pwd` >> $tmp
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