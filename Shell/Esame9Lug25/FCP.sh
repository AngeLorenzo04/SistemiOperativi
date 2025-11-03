#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 9 Luglio 2025


#Controllo comandi LASCO
if test $# -lt 2
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 2 parametri
   exit 1
fi

#Controllo che gli altri parametri siano dei percorsi
percorsi=
cnt=0
w=$#
check=`expr $w - 1`
X=
for i
do
    if test $cnt -eq $check
    then
        #Controllo che lìultimo parametro sia un numero intero
        N=`expr $i + 1`
        if test $? -ne 0
        then
            echo ERRORE: Il parametro $i non è valido
            echo SUGGERIMETNO: Inserire un valore numerico
            exit 4
        else
            if test $i -gt 100
            then
                echo ERRORE: il parametro $i non è valido
                echo SUGGERIMENTO: Inserire un valore positivo compreso tra 0 e 100
                exit 5
            fi
        fi
        X=$i
        break;
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

#Creazione file temporaneo che conterrà il risultato della ricerca
tmp="/tmp/NomiAssoluti"
> $tmp



#Invocazione iterativa del file comandi ricorsivo
L=0
for G in $percorsi
do
    FCR.sh $G $X $L $tmp
done

NOCC=`wc -l  < $tmp`

echo SONO STATE TROVATE $NOCC OCCORRENZE...
if test ! $NOCC -eq 0
then
    echo ===================================================================================
    cat $tmp
    echo Il file più lungo ha $L righe
    echo ===================================================================================
fi

rm $tmp