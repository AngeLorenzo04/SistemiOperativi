#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 10 Settembre 2025


#Controllo comandi LASCO
if test $# -lt 2
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Controllo che il primo parametro sia un carattere
C=
case $1 in
    ?) 
        C=$1
        shift
        ;;
    *)
        echo ERRORE: parametro $1 non è valido
        echo SUGGERIMENTO: inserire un singolo carattere
        exit 2
        ;;
esac 

#Controllo che gli altri parametri siano dei percorsi
percorsi=
for i
do
    case $i in
    */*)
        if test ! -d $i -o ! -x $i
        then
           echo ERRORE: il parametro $i non è valido
           echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
            exit 3
        fi
        percorsi="$percorsi $i"
        ;;
    *)
        echo ERRORE: il parametro $i non è valido
        echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
        exit 4
        ;;
    esac
done

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $C
done

        