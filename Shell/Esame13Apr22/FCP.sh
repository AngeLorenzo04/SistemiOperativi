#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 13 aprile 2022

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
    echo SUGGERIMETNO: Inserire un valore numerico
    exit 2
fi
X=$1
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

#Invocazione iterativa del file comandi ricorsivo
n=1
for G in $percorsi
do
    #Creo per ogni iterazione un file che conterrà i risultati della ricerca
    tmp="/tmp/nomiAssoluti-$n"
    > $tmp
    FCR.sh $G $X $tmp
    echo Il numero di file trovati in questa fase e\' `wc -l < $tmp`
    n=`expr $n + 1`
done

echo 
echo
#Controllo se la prima gerarchia ha le stesse occorenze di qualche altra gerarchia
numero_gerarchie=$#
echo NUMERO GERARCHIE $numero_gerarchie
for file1 in `cat /tmp/nomiAssoluti-1`
do
    # Confronta con tutte le altre gerarchie
    n=2
    while test $n -le $numero_gerarchie
    do
        for file2 in `cat /tmp/nomiAssoluti-$n`
        do
            if diff $file1 $file2 > /dev/null 2>&1
            then
                echo "I file $file1 e $file2 sono uguali"
            fi
        done
        n=`expr $n + 1`
    done
done

#Cancello tutti i file temporanei 
rm /tmp/nomiAssoluti-*       