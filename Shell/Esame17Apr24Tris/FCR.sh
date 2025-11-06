#!/bin/bash
#File Comandi Ricorsiva inerente alla prova d'esame del 17 Aprile 2024


#Mi sposto nella direcotry desiderata
cd $1

#Cerco le varie occorrenze
cnt=0
for i in *
do
    if test $cnt -eq 3
    then 
        if test -f $i 
        then
            echo `pwd`/$i >> $tmp
        else test -d $i 
            echo `pwd`/$i è una directory che si trova in terza posizione
        fi
    fi
    cnt=`expr $cnt + 1`
done

if test $cnt -lt 3
then
    echo ALLERT: la directory `pwd`/$i ha meno di 3 elementi
fi 

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $tmp
   fi
done