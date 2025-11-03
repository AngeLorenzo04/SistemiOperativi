#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 11 Giu 2025


#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
caratteri=$2
tmp=$3

#Cerco i file al interno della directory corrente
check=0
for i in *
do 
    if test -f $i 
    then
        for letter in $caratteri
        do
            if ! grep $letter $i 
            then
                check=1
                break;
            fi
        done 
    fi
done

#Se il file rispetta le specifiche lo salvo
if test $check -eq 0
then
    echo `pwd`/$i >> $tmp
fi


#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $caratteri $tmp
   fi
done