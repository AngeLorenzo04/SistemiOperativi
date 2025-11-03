#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 10 Settembre 2025

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
C=$2

#Cerco le occorrenze
IsValid=0
case $1 in
    */$C?$C)
        for i in *
        do
            if test -f $i -a -r $i 
            then
                len=`wc -c < $i`
                if test $len -ne 0 -a `expr $len % 2` -eq 0

                then
                    IsValid=1
                    break;
                fi
            fi
        done
        ;;
    *)
        ;;
esac

if test $IsValid -eq 1
then
    echo `pwd` V
else
    echo `pwd` X
fi

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $C
   fi
done
