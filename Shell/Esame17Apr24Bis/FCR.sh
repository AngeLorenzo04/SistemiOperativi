#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 17 Aprile 2024

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
tmp=$2

#Cerco le occorrenze
cont=1
for i in *
do
    if test $cont -eq 3
    then 
        if test -f $i 
        then
            echo `pwd`/$i >> $tmp
        else test -d $i 
            echo `pwd`/$i è una directory che si trova in terza posizione
        fi
    fi
    cont=`expr $cont + 1`
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $tmp
   fi
done