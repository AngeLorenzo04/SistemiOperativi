#!/bin/bash
#FIle comandi ricorsivo inerente alla prova d'esame del 7 Giugno 2023

cd $1

#Salvo i parametri in delle variabili
X=$2
tmp=$3
cont=$4

if test $cont -eq $X
then

    for i in *
    do
        if test -d $i -a -x $i 
        then
            echo `pwd`/$i >> $tmp
        fi
    done 

fi

cont=`expr $cont + 1 `

for i in *
do
   if test -d $i -a -x $i
   then
       $0 `pwd`/$i $X $tmp $cont
   fi
done