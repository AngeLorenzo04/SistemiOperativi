#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 12 Aprile 2022

#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Controllo che il primo parametro sia un numero intero
N=`expr $1 + 1`
if test $? -ne 0
then
    echo ERRORE: Il parametro $1 non è valido
    echo SUGGERIMETNO: Inserire un valore numerico positivo
    exit 2
fi

X=$1
if test $X -lt 0
then
    echo ERRORE: il parametro $X non è valido
    echo SUGGERIMENTO: inserire un valore numerico positivo
fi
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

#Invocazione iterativa del file comandi ricorsivo
echo
n=1
for G in $percorsi
do
    #Creazione file temporaneo che conterrà il risultato della ricerca
    tmp="/tmp/nomiAssoluti-$n"
    > $tmp
    FCR.sh $G $X $tmp
    echo il numeor di file trovati al iterazione $n è `wc -l < $tmp`
    n=`expr $n + 1`
done

echo

#Cerco nelle varie iterazioni i file uguali a quelli della prima iterazione
numero_gerarchie=$#
echo NUMERO GERARCHIE $numero_gerarchie
for file1 in `cat /tmp/nomiAssoluti-1`
do
    n=2
    while test $n -le $numero_gerarchie
    do
        for file2 in `cat /tmp/nomiAssoluti-$n`
        do
            if diff $file1 $file2 >/dev/null 2>&1
            then
                echo i file $file1 e $file2 sono uguali
            fi
        done

        n=`expr $n + 1`
    done

done
echo

