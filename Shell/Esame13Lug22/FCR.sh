#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 13 Luglio 2022

#Mi sposto nella gerarchia desiderata
cd $1
shift
#Salvo i parametri in delle variabili
L=$1
shift
tmp=$1
shift
caratteri=$*

#Cerco le occorrenze
for i in *
do
    echo Aalizzo-$i 
    if test -f $i 
    then
        len=`wc -l < $i` #Calcolo la lunghezza del file
        valid=0

        if test $len -eq $L #Verifico che la lunghezza del file sia valida 
        then 

            for carattere in $caratteri #Per ogni carattere 
            do
                if grep -q $carattere $i #Verifico che il carattere NON sia presente nel file
                then
                    echo File $i contiene $carattere
                else
                    echo File $i non contiene $carattere
                    valid=1
                fi
            done

            if test $valid -eq 0 #Se tutti i caratteri sono presenti
            then
                echo `pwd`/$i >> $tmp   #Aggiungo il file ai file validi
            fi
        fi
        
        

    fi
done

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $X $turn $tmp
   fi
done

