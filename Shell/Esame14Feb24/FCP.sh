#!/bin/bash
#File comandi principale inerente alla prova d'esame del 14 febbraio 2024

#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Controllo che il primo parametro sia un percorso assoluto
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

X=$1
shift

#Controllo che i restanti parametri siano stringhe con lunghezza almeno 2
stringhe=
for i 
do
    case $i in
        ?)
            echo ERRORE: parametro $i non valido
            echo SUGGERIMENTO: inserire una stringa da almeno 2 caratteri
            exit 5
            ;;
        *)
            stringhe="$stringhe $i"
            ;;
    esac
done

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creo un file temporaneo
tmp="/tmp/Occorrenze"
> $tmp

#Invoco il file comandi ricorsivo 
FCR.sh $G $X $tmp

NOCC=`wc -l  < $tmp`

echo SONO STATE TROVATE $NOCC OCCORRENZE...
if test ! $NOCC -eq 0
then
    echo ======================
    cat $tmp
    echo ======================
fi
echo
echo
echo STRINGE MANDATE
echo ================
for str in $stringhe
do
    echo $str
done
echo ================



rm $tmp


