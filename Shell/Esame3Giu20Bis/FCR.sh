#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 3 Giugno 2020

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
C=$2
tmp=$3

#Cerco le occorrenze
for i in *
do
    if test -d $i 
    then
        case $i in
            $C?$C)
                echo `pwd`/$i >> $tmp  
                ;;
            *)
                ;;
        esac
    fi
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $C $tmp
   fi
done