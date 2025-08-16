#!/bin/bash

# File Comandi Principali inerente alla prova d'esame del 19 Febbraio 2024

#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi  

X=$1
shift
#Controllo che il primo parametro sia un numero intero
N=`expr $X + 1`
if test $? -ne 0
then
    echo ERRORE: Il parametro $X non è valido
    echo SUGGERIMETNO: Inserire un valore numerico
    exit 2
fi

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

tmp="/tmp/occorrenze"
> $tmp

#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $X $tmp
done


NOCC=`wc -l  < $tmp`

echo SONO STATE TROVATE $NOCC OCCORRENZE...

if test ! $NOCC -eq 0
then
    echo ======================
    cat $tmp
    echo ======================
fi

if test $NOCC -lt 2
then
    echo ERRORE: numero occorrenze minori di 2 
    echo SUGGERIMENTO: Richieste almeno 2 occorrenze
    exit 5
fi 

rm $tmp

