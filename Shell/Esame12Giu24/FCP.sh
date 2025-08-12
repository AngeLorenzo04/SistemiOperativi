#!/bin/bash
#File comandi principale inerente alla prova del 12 Giugno 2024

#Controllo parametri lasco
if test $# -lt 3
then
    echo ERRORE: Numero parametri non valido
    echo SUGGERIMENTO: inserire almeno 3 parametri
    exit 1
fi 

cnt=0
C=
percorsi=

for i 
do
    cnt=`expr $cnt + 1`
    if test $cnt -eq $#
    then 
        C=$i
        break;
    else

        case $i in
        */*)
            if test ! -d $i -o ! -x $i
            then 
                echo ERRORE: parametro $i non valido
                echo SUGGERIMENTO: inserire il percorso di una directory in forma assoluta
                exit 2  
            fi
            percorsi="$percorsi $i"
            ;;
        *)
            echo ERRORE: parametro $i non valido
            echo SUGGERIMENTO: inserire il percorso di una directory in forma assoluta
            exit 3;;
        esac    
    fi
done

#Controllo che C sia un singolo carattere
case $C in
    ?) ;;
    *)
        echo ERRORE: parametro $C non valido
        echo SUGGERIMENTO: inserire un singolo carattere
        exit 4
        ;;
esac

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creo un file temporaneo dove salvare le soluzioni
tmp="/tmp/FileTemporaneo"
> $tmp

tmp1="/tmp/FileTemporaneo1"
> $tmp1

#Chiamo il iterativamente il file ricorsivo
for G in $percorsi
do
    FCR.sh $G $tmp $tmp1
done

NRO=`wc -l < $tmp`
NRO1=`wc -l < $tmp1`

echo sono stati trovati $NRO file che rispettano le specifiche a,b
echo LISTA DELLE OCCORRENZE
echo ========================
cat $tmp
echo ========================
echo
echo
echo sono stati trovati $NRO1 file che rispettano tutte le specifiche
echo LISTA DELLE OCCORRENZE
echo ========================
cat $tmp1
echo ========================

rm $tmp
rm $tmp1