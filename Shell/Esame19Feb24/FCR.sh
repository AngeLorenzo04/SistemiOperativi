#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 19 Febraio 2024

echo 1-$1
#mi sposto nella cartella che sto analizzando
cd $1

#Salvo nelle variabili i vari parametri
X=$2
tmp=$3

#Cerco le occorrenze
for i in *
do
    echo DEBUG-Analizzo $i
    if test -f $i -a -r $i
    then
        len=`wc -l < $i`
        if test $len -eq $X 
        then
            echo `pwd`/$i >> $tmp
        fi
    fi
done

#Chiamata ricorsiva di FCR
for i in *
do
   if test -d $i -a -x $i
   then
       $0 `pwd`/$i $X $tmp
   fi
done