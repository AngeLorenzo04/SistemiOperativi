#!/bin/bash
#File comandi ricorsivo inerente alla prova del 12 luglio 2023

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
X=$2
max=$3
tmp=$4

#Cerco le occorrenze
for i in *
do 
    if test -f $i -a -r $i
    then
        case $i in 
            *.txt)
                len=`wc -l < $i`
                #Se il file è valido lo aggiungo alla lista dei file validi
                if test $len -lt $X 
                then
                    echo DEBUG $i è valido
                    echo `pwd`/$i >> $tmp

                fi
                
                #Aggiorno la lunghezza massima
                if test $len -gt $max
                then
                    max=$len
                    echo DEBUG la lunghezza massima e aggiornata a $max
                fi

                ;;
            *) ;; 
        esac

    fi
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
       $0 `pwd`/$i $X $max $tmp
   fi
done

