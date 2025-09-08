#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 22 Aprile 2021

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
S=$2
tmp=$3

#Cerco le occorrenze
check=0
for i in *
do 
    if test -f $i 
    then
        case $i in
        *.$S)
            check=1
            ;;

        *)
            ;;
        esac
    fi
done

if test $check -eq 1
then
    echo `pwd` >> $tmp 
fi

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $*
   fi
done
