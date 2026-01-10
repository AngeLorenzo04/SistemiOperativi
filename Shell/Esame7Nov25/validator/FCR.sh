#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 7 Novembre 2025

#Mi sposto nella directory desiderata
cd $1
percoso="$4$1/"

#Salvo i parametri in specifice variabili
caratteri=$2
tmp=$3

flagFind=0 #Flag per verificare la presenza di un occorrenza

#Cerco all'interno della directory le varie occorrenze
for i in *
do
    if test -f $i -a -r $i #Controllo che sia un file leggibile
    then

        flagChar=1 #Ipotiziamo che tutti i caratteri siano presenti

        for carattere in $caratteri #Controllo che ci sia un occorrenza per ogni carattere
        do 
            `grep $carattere $i > /dev/null`

            if test $? -eq 1  #Se il carattere non è presente imposto la falg come 0
            then
                flagChar=0
                break
            fi

        done

        if test $flagChar -eq 1 #Caso in cui trovo l'occorrenza
        then
            flagFind=1
            break
        fi
    fi
done

echo FLAGFIND $flagFind
if test $flagFind -eq 1
then
    echo + $percoso >> $tmp 
else 
    echo - $percoso >> $tmp 
fi

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 $i "$caratteri" $tmp $percoso
   fi
done