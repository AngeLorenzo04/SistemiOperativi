#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 22 Aprile 2021

#Controllo comandi LASCO
if test $# -lt 4
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 4 parametri
   exit 1
fi

#Controllo che il primo parametro sia un numero intero
N=`expr $1  + 1`
if test $? -ne 0
then
    echo ERRORE: Il parametro $1 non è valido
    echo SUGGERIMETNO: Inserire un valore numerico
    exit 2 
fi
W=$1
shift

#Salvo il contenuto del secondo parametro in una variabile
S=$1
shift

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

#Creazione file temporaneo che conterrà il risultato della ricerca
tmp="/tmp/nomiAssoluti"
> $tmp

#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $S $tmp
done

NOCC=`wc -l  < $tmp`

echo 

echo SONO STATE TROVATE $NOCC OCCORRENZE...
if test ! $NOCC -eq 0
then
    echo ===================================================================================
    cat $tmp
    echo ===================================================================================
fi

echo

#Verifico che siano state trovate almeno W occorrenze
if test $NOCC -ge $W 
then
    
    echo Ciao! $USER, inserisci un numero compreso tra 1 e $W 
    read X
    
    #Controllo che il valore inserito sia valido
    if test $X -lt 1 -o $X -gt $W 
    then
        echo ERRORE: Il valore inserito non è valido
        echo SUGGERIMENTO: Inserisci un numero compreso tra 1 e $W 
        exit 5
    fi


    #Stampo l'X-esima directory in tmp
    echo
    echo Ecco la $X-esima directory trovata...
    head -n $X $tmp | tail -n 1

fi

rm $tmp