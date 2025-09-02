#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 15 Giugno 2022

#Mi sposto nel percorso desiderato
echo SONO IN $1
cd $1

#Salvo i parametri in delle variabili per chiarezza
tmp=$2

#Cerco al'interno di questo percorso le occorrenze
for i in *
do
    echo ANALIZZO-$i
    if test -f "$i"
    then
        if grep -qv '^[a-z ]*$' "$i"
        then
            echo "Il file $i contiene caratteri diversi dalle lettere minuscole."
        else
            echo "Il file $i contiene SOLO lettere minuscole."
            echo `pwd`/$i >> $tmp
        fi
    fi
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $tmp
   fi
done