#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 12 Aprile 2023

#Controllo comandi LASCO
if test $# -lt 4
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 4 parametri
   exit 1
fi

#Salvo i primi due parametri in delle variabili, essendo delle stringhe non c'è bisogno di fare particolari controlli
S1=$1
S2=$2
shift
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

#Creazione file temporanei che conterranno il risultato della ricerca
tmp1="/tmp/nomiAssoluti1"
> $tmp1

tmp2="/tmp/nomiAssoluti2"
> $tmp2

#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $S1 $S2 $tmp1 $tmp2
done

#Dai file temporanei estraggo il numero di occorrenze trovate

TOT1=`wc -l  < $tmp1`
TOT2=`wc -l  < $tmp2`

echo
echo SONO STATE TROVATE $TOT1 OCCORRENZE DI FILE CON ESTENSIONE .$S1...
if test ! $TOT1 -eq 0
then
    echo ===================================================================================
    cat $tmp1
    echo ===================================================================================
fi

echo
echo SONO STATE TROVATE $TOT2 OCCORRENZE DI FILE CON ESTENSIONE .$S2...
if test ! $TOT1 -eq 0
then
    echo ===================================================================================
    cat $tmp2
    echo ===================================================================================
fi

echo
#Verifico che l'interazione con l'utente rispetti le specifiche
if test $TOT1 -le $TOT2
then
    echo Inserisci un numero tra 1 e $TOT1
    read X
    echo
   #Controllo che il valore inserito sia valido
    if test $X -lt 1 -o $X -gt $TOT1
    then
        echo ERRORE: il parametro inserito non è valido
        echo SUGGERIMENTO: inserire un valore compreso tra 1 e $TOT1
        exit 4
    fi

    echo Elaboro il $X-esimo file con estensione .$S1...
    head -n $X $tmp1 | tail -n 1
    echo
      echo Elaboro il $X-esimo file con estensione .$S2...
    head -n $X $tmp2 | tail -n 1


fi

rm $tmp1
rm $tmp2