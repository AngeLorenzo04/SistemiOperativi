#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 10 Luglio 2024

#Controllo comandi LASCO
if test $# -lt 2
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Controllo che il primo parametro sia un numero intero
N=`expr $1 + 1`
if test $? -ne 0
then
    echo ERRORE: Il parametro $1 non è valido
    echo SUGGERIMETNO: Inserire un valore numerico
    exit 2
fi

X=$1
shift


if test $X -ge 20
then
    echo ERRORE: Il parametro $X non è valido
    echo SUGGERIMENTO: Inserire un valore strettamente minore di 20
    exit 3
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
            exit 4
        fi
        percorsi="$percorsi $i"
        ;;
    *)
        echo ERRORE: il parametro $i non è valido
        echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
        exit 5
        ;;
    esac
done

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creazione file temporaneo che conterrà il risultato della ricerca
tmp="/tmp/NomiAssoluti"
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
    echo ===================================================================================
    cat $tmp
    echo ===================================================================================
fi

rm $tmp
