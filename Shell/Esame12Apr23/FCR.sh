#!/bin/bash
#FIle comandi ricorsivo inerente alla prova d'esame del 12 Aprile 2023

#Mi sposto nella directory da analizzare
cd $1

#Slavo nelle variabili i parametri passati
S1=$2
S2=$3
tmp1=$4
tmp2=$5


#Cerco le occorrenze
for i in *
do
    if test -f $i -a -x $i 
    then
        case $i in
                *.$S1)
                    echo $i ha estenzione S1
                    echo "$(pwd)/$i" >> "$tmp1"
                    ;;
                *.$S2) 
                    echo $i ha estenzione S2
                    echo "$(pwd)/$i" >> "$tmp2"
                    ;;  
        esac
    fi

done

for i in *
do
        if test -d $i -a -x $i
        then
                #invocazione ricorsiva
                FCR.sh `pwd`/$i $S1 $S2 $tmp1 $tmp2
        fi
done