#!/bin/bash
#File comandi ricorsivo inerente alla prova d'esame del 7 Gennaio 2024

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
X=$2
cont=$3
tmp=$4

if test `expr $cont % 2` -eq 0 #Verifico se il livello è pari
then
    for i in *
    do 
        #Salvo tutte le occorrenze
        if test -f $i -a -r $i
        then
            len=`wc -l < $i`
            if test $len -eq $X
            then
                echo `pwd`/$i >> $tmp
            fi
        fi
    done
fi

cont=`expr $cont + 1` #Aggiorno cont

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
        $0 `pwd`/$i $X $cont $tmp
   fi
done