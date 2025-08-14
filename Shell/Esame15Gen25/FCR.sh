#!/bin/bash
# File comandi ricorsivo inerente all'esame di FCR del 15 Gennaio 2025

cd $1

#Salvo i parametri in delel variabili
N=$2
X=$3
tmp=$4



for i in *
do 
    if test -f $i -a -r $i 
    then 
        line=`wc -l < $i`
        if test $line -ge $N -a $line -le $X
        then
            echo `pwd`/$i >> $tmp
        fi
    fi
done

for i in *
do
   if test -d $i -a -x $i
   then
       FCR.sh `pwd`/$i $N $X $tmp 
   fi
done
