#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 11 apr 2025

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
W=$#
check=`expr $W - 1`
S=
for i
do
    #Controlli sul ultimo parametro
    if test $cnt -eq $check
    then
        case $i in
        ?)
            echo ERRORE: il parametro $i non è valido
            echo SUGGERIMENTO: inserire una stringa di almeno 2 caratteri
            exit 4
            ;; 
        *)
            S=$i
            ;;
        esac
        break;
    fi
    cnt=`expr $cnt + 1`
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

#Creazione file temporaneo che conterrà il risultato della ricerca
tmp="/tmp/NomiAssoluti"
> $tmp

#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $S $tmp
done

NOCC=`wc -l  < $tmp`

if test $NOCC -gt 0
then
    echo SONO STATE TROVATE $NOCC OCCORRENZE...
    if test ! $NOCC -eq 0
    then
        echo ===================================================================================
        for percorso in `cat $tmp`
        do
            echo $percorso
            `chmod u+x $percorso`
            echo PERMESSI AGGIUNTI CON SUCCESSO
        done
        echo ===================================================================================
    fi    
else
    echo NON SONO STATE TROVATE OCCORRENZE...
    rm $tmp
    exit 5
fi
rm $tmp