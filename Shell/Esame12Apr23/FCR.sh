#!/bin/bash
#FIle comandi ricorsivo inerente alla prova d'esame del 12 Aprile 2023
echo FCR
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
    echo analizzo $i
    if test -f $i -a -r $i
    then
        case $i in
            *.$S1)
                    echo il file $i ha estensione $S1
                    echo `pwd`/$i >> $tmp1
                    
                ;;
            *.$S2)
                    echo il file $i ha estensione $S2
                    echo `pwd`/$i >> $tmp2

                ;;
        esac
    fi

done

for i in *
do
   if test -d $i -a -x $i
   then
       $0 `pwd`/$i $S1 $S2 $tmp1 $tmp2
   fi
done