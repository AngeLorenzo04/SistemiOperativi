#!/bin/bash
#File Comandi Principale inerente alla seconda prova d'esame del 11 Aprile 2025

#Controllo comandi LASCO
if test $# -lt 2
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 2 parametri
   exit 1
fi

#Controllo che i parametri siano percorsi assoluti
W=$#
cont=0
check=`expr $W - 1`
percorsi=

for i
do
    #Controlli sul ultimo parametro
    if test $cont -eq $check
    then
        case $i in 
            ?)
                echo ERRORE: il parametro $i non è valido
                echo SUGGERIMENTO: inserire una stringa di almeno due caratteri
                exit 4
                ;;
            *)
                s=$i
                ;;
        esac
        break;
    fi    
    cont=`expr $cont + 1`

    #Controlli su gli altri parametri
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
tmp="/tmp/dirAssolute"
> $tmp

#Invocazione iterativa del file comandi ricorsivo
echo
for g in $percorsi
do
    FCR.sh $g $s $tmp
done

nro=`wc -l  < $tmp`

echo

if test ! $nro -eq 0
then
    echo SONO STATE TROVATE $nro OCCORRENZE...
    echo ===================================================================================
    for f in `cat $tmp`
    do
        echo $f
        `chmod u-x $f`
        echo PERMESSI RIMOSSI CON SUCCESSO...
    done
    echo ===================================================================================
else
    echo NON SONO STATE TROVATE OCCORRENZE...
    rm $tmp
    exit 5
fi

rm $tmp
