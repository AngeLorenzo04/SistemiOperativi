#!/bin/bash
#File comandi principale inerente alla prova d'esame del 15 Giugno 2022
#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Controllo che i parametri siano dei percorsi
percorsi=
for i
do
    case $i in
    */*)
        if test ! -d $i -o ! -x $i
        then
           echo ERRORE: il parametro $i non è valido
           echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
            exit 2
        fi
        percorsi="$percorsi $i"
        ;;
    *)
        echo ERRORE: il parametro $i non è valido
        echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
        exit 3
        ;;
    esac
done

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creo un file temporaneo che conterrà i risultati della ricerca
tmp="/tmp/nomiAssoluti"
> $tmp

#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $tmp
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