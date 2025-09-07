#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 25 GEnnaio 2023


#Controllo comandi STRETTO
if test $# -ne 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire esattamente 3 parametri
   exit 1
fi

#Controllo che gli altri parametri siano dei percorsi
G=
case $1 in
*/*)
    if test ! -d $1 -o ! -x $1
    then
       echo ERRORE: il parametro $1 non è valido
       echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
        exit 
    fi
    G=$1
    ;;
*)
    echo ERRORE: il parametro $1 non è valido
    echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
    exit 
    ;;
esac

#Salvo gli altri parametri in delle variabili
E1=$2
E2=$3

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creazione di file temporanei che conterranno il risultato della ricerca
tmp1="/tmp/E1"
> $tmp1

tmp2="/tmp/E2"
> $tmp2


#Invocazione unica iterazione
FCR.sh $G $E1 $E2 $tmp1 $tmp2


cont1=`wc -l  < $tmp1`
cont2=`wc -l  < $tmp2`

echo SONO STATE TROVATE $cont1 OCCORRENZE NELLA RICERCA E1
if test ! $cont1 -eq 0
then
    echo ===================================================================================
    cat $tmp1
    echo ===================================================================================
fi

echo

echo SONO STATE TROVATE $cont2 OCCORRENZE NELLA RICERCA E2
if test ! $cont2 -eq 0
then
    echo ===================================================================================
    cat $tmp2
    echo ===================================================================================
fi

rm $tmp1
rm $tmp2