#!/bin/bash
#File Comandi ricorsivo  inerente alla prova d'esame del 11 apr 2025

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
S=$2
tmp=$3

#Cerco le occorrenze desiderate
controllo=1
for i in *
do 
    case $i in 
    *.$S)
        if test -f $i 
        then
            if test ! -x $i
            then
                echo o $i
                echo `pwd`/$i >> $tmp
                controllo=2
            fi
        fi
        ;;
    *)
        ;;
    esac

done

if test $controllo -eq 2
then 
    echo La directory `pwd` contiene amlemno un occorrenza
else 
    echo la direcotry `pwd` non contiente alcun occorrenza 
fi


#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $S $tmp
   fi
done

