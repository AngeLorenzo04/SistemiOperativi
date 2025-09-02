#!/bin/bash
#File Comandi Principale Inerente alla prova d'esame del 13 Luglio 2022

#Controllo comandi LASCO
if test $# -lt 2
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 2 parametri
   exit 1
fi

#Controllo che il primo parametro sia un percorso assoluto
case $1 in
*/*)
    if test ! -d $1 -o ! -x $1
    then
       echo ERRORE: il parametro $1 non è valido
       echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
       exit 2
    fi
    G=$1
    shift
    ;;
*)
    echo ERRORE: il parametro $1 non è valido
    echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
    exit 3
    ;;
esac

#Controllo che il secondo parametro sia un numero intero
N=`expr $1 + 1`
if test $? -ne 0
then
    echo ERRORE: Il parametro $1 non è valido
    echo SUGGERIMETNO: Inserire un valore numerico
    exit 4
fi
L=$1
shift

#Controllo che gli altri parametri siano singoli caratteri
caratteri=
for i 
do
    case $i in
    ?)
        caratteri="$caratteri $i"
        ;;
    *)
        echo ERRORE: il parametro $i non è valido
        echo SUGGERIMENTO: inserire un singolo carattere
        exit 5
        ;;
    esac
done

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creo un file che conterrà il risultato della ricerca
tmp="/tmp/nomiAssoluti"
> $tmp

#Invocazione singola fase
FCR.sh $G $L $tmp $*

NOCC=`wc -l  < $tmp`

echo SONO STATE TROVATE $NOCC OCCORRENZE...
if test ! $NOCC -eq 0
then
    echo ======================
    cat $tmp
    echo ======================
fi

rm $tmp