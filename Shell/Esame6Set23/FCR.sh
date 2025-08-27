#!/bin/bash

#File comandi ricorsivo inerente alla prova d'esame del 6 settembre 2023

#Mi sposto nella directory desiderata
cd $1
echo SONO IN $1
#Memorizzo i parametri in delle variabili
M=$2
tmp=$3
shift
shift
shift

#Cerco le occorrenze, ne devo trovare almeno M
cont=0
for i in *
do    
    echo DEBUG analizzo $i
    for F in $* 
    do
        if test $F = $i 
        then
            echo $F è uguale a $i
            cont=`expr $cont + 1`
        fi
    done

done

echo debug cont $cont

if test $cont -ge $M
then
    echo `pwd` valido cont:$cont 
    echo `pwd` >> $tmp
    cont=0
fi

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $M $tmp $*
   fi
done