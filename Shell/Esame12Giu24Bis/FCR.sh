#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 12 Giugno 2024


#Mi sposto nella cartella desiderata
cd $1

echo G $1

#Salvo i parametri in delle variabili
C=$2

#Cerco le occorrenze 
case $1 in 
    */?$C?$C)
        echo SONO DENTRO
        check=0
        for i in *
        do 
            if test -f $i -a -r $i
            then
                len=`wc -l < $i`
                if test `expr $len % 2` -eq 0 -a $len -ne 0
                then
                    check=1
                    break;
                fi
            fi
        done

        echo ALLERT: la direcotry `pwd`/$i rispetta le seguenti specifiche:
        echo a
        echo b
        if test $check -eq 1
        then
            echo c
        fi
            
        ;;
    *)
        ;;
esac

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $C
   fi
done
