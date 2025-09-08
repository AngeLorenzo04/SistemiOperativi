#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 12 Aprile 2023

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili 
S1=$2
S2=$3
tmp1=$4
tmp2=$5

#Cerco le occorrenze
for i in *
do 
    if test -f $i 
    then
        case $i in
            *.$S1)
                echo `pwd`/$i >> $tmp1
                ;;
            *.$S2)
                echo `pwd`/$i >> $tmp2
                ;;
        esac
    fi
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $*
   fi
done