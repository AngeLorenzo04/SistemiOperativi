#!/bin/bash
#File Comandi Principale inenerente alla prova d'esame del 7 Settembre 2022

#Controllo comandi LASCO
if test $# -lt 3
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

#Verifico che il valore sia dispari
X=$1
shift
check=`expr $X % 2`
if test $check -eq 0
then
    echo ERRORE: il parametro $X non è valido
    echo SUGGERIMENTO: Inserire un valore numerico dispari
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

#Creo un file temporaneo che conterrà le occorrenze trovate
tmp="/tmp/nomiAssoluti"
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

rm $tmp

