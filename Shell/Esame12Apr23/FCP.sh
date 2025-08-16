#!/bin/bash
#FIle comandi principale inerente alla prova d'esame del 12 Aprile 2023


#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 4 parametri
   exit 1
fi

#Salvo le prime due stringhe
S1=$1
shift
S2=$1
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

#Creao i file temporanei che conterranno le occorrenze
tmp1="/tmp/occorrenzeS1"
> $tmp1

tmp2="/tmp/occorenzeS2"
> $tmp2


#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $S1 $S2 $tmp1 $tmp2
done   

TOT1=`wc -l  < $tmp1`

echo SONO STATE TROVATE $TOT1 OCCORRENZE...
if test ! $TOT1 -eq 0
then
    echo ======================
    cat $tmp1
    echo ======================
fi



TOT2=`wc -l  < $tmp2`

echo SONO STATE TROVATE $TOT2 OCCORRENZE...
if test ! $TOT2 -eq 0
then
    echo ======================
    cat $tmp2
    echo ======================
fi


if test $TOT1 -le $TOT2
then
    echo Inserire un unmero compreso tra 1 e $TOT1
    read X
    echo
    if test $X -gt $TOT1 -o $X -lt 1
    then
        echo ERRORE: Valore inserito non valido
        echo SUGGERIMENTO: Inserire un unmero compreso tra 1 e $TOT1
        exit 5
    fi
    echo Riga $X del file contenente le occorrenze con estensione $S1...
    echo `head -n $X $tmp1| tail -n 1 $tmp1`
    echo
    echo Riga $X del file contenente le occorrenze con estensione $S2...
    echo `head -n $X $tmp2 | tail -n 1 $tmp2`
fi



rm $tmp2
rm $tmp1    