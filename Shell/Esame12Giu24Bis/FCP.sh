#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 12 Giugno 2024

#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Controllo che gli altri parametri siano dei percorsi
percorsi=
cnt=0
w=$#
check=`expr $w - 1`
C=
for i
do
    if test $cnt -eq $check
    then
        case $i in
            ?)
                C=$i
                break
                ;;
            *)
                echo ERRORE: Il parametro $i non è valido
                echo SUGGERIMENTO: inserire un singolo carattere
                exit 4
                ;;
        esac
    fi

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
    cnt=`expr $cnt + 1`
done

#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $C
done