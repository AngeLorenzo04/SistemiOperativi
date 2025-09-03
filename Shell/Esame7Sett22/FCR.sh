#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 7 Settembre 2022

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabbili per chiarezza
X=$2
tmp=$3

#Cerco le occorrenze

cont=0

for i in *
do
    if test -f $i
    then
        len=`wc -l < $i`
        if test $len -eq $X 
        then
            check=`expr $cont % 2`
            if test $check -eq 1
            then
                echo  IL FILE `pwd`/$i È IN POSIZIONE DISPARI
            fi 
            echo `pwd`/$i >> $tmp
        fi
    fi
    cont=`expr $cont + 1`
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $X $tmp
   fi
done