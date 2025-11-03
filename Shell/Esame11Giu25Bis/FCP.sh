#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 11 Giugno 2025 


#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi


#Controllo che il primo parametro sia una directry assoluta
G=
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
        echo ERRORE: il parametro $i non è valido
        echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
        exit 3
        ;;
esac

#Controllo che gli altri parametri siano caratteri
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
            exit 4
            ;;
    esac
done

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creazione file temporaneo che conterrà il risultato della ricerca
tmp="/tmp/NomiAssoluti"
> $tmp

FCR.sh $G "$caratteri" $tmp

NOCC=`wc -l  < $tmp`

echo SONO STATE TROVATE $NOCC OCCORRENZE...
if test ! $NOCC -eq 0
then
    echo ===================================================================================
    cat $tmp
    echo ===================================================================================
fi

rm $tmp