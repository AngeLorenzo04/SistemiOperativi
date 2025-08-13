#!/bin/bash
#File comandi principale inerente alla prova d'esame del 11 settermbre 2024

#Controllo comandi LASCO
if test $# -lt 2
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 2 parametri
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
    FCR.sh $G $X 0 $tmp
done

NOCC=`wc -l  < $tmp`

echo SONO STATE TROVATE $NOCC OCCORRENZE...
if test ! $NOCC -eq 0
then
    echo ======================
    cat $tmp
    echo ======================
fi

rm $tmp