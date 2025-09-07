#!/bin/bash
#File Comandi ricorsivo inerente alla prova d'esame del 25 Gennaio 2023

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
E1=$2
E2=$3
tmp1=$4
tmp2=$5
for i in *
do 
    if test -f $i 
    then
        case $i in
            *.$E1)
                if test -r $i
                then
                    echo `pwd`/$i >> $tmp1
                fi
                ;;
            *.$E2)
                if test -w $i -a -r $i
                then
                    echo `pwd`/$i >> $tmp2

                fi
                ;;
        esac
    fi
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $E1 $E2 $tmp1 $tmp2
   fi
done