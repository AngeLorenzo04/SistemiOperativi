#!/bin/bash
#File Comandi Ricorsivo inerente alla seconda prova d'esame del 11 Aprile 2025

#Mi sposto nella directory desiderata
cd $1

#Salvo i parametri in delle variabili
s=$2
tmp=$3

#Cerco le occorrenze
controllo=0
for f in *
do 
    case $f in
        *.$s)
            if test -f $f -a -x $f
            then
                echo `pwd`/$f >> $tmp 
                controllo=1
            fi  
            ;;
        *)
            ;;
    esac
    
done

if test $controllo -eq 1
then
    echo La directory `pwd` contiene almeno un occorrenza
else
    echo La directory `pwd` non contiene nessuna occorrenza
fi

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $s $tmp
   fi
done