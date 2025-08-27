#!/bin/bash
#File comanadi principale inerente alla prova d'esame del 6 settembre 2023

#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Controllo che il primo parametro sia un percorso assoluto

case $1 in
*/*)
    if test ! -d $1 -o ! -x $1
    then
       echo ERRORE: il parametro $1 non è valido
       echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
        exit 2
    fi
    percorso=$1
    shift
    ;;
*)
    echo ERRORE: il parametro $1 non è valido
    echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
    exit 3
    ;;
esac


#Controllo che gli altri parametri siano dei nomi assoluti
files=
for i 
do 
    case $i in 
        /*)
            echo ERRORE: Parametro $i non valido
            echo SUGGERIMENTO: Inserire un nome assoluto
            exit 4
            ;;
        *)
            for F in $files
            do 
                if test $i = $F 
                then
                    echo ERRORE: parametro $i non valido
                    echo SUGGERIMETNO: non inserire più volte lo stesso parametro
                    exit 5
                fi
            done
            files="$files $i"
            ;;
    esac
done


#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creo il file temporaneo che conterrà le occorrenze cercate
tmp='/tmp/Occorrenze'
> $tmp

#Memorizzo il numero di parametri passati escludendo il primo
P=$#

#Ricavo la metà di questo numero
M=`expr $P / 2`


#Invocazione del file comandi Ricorsivo
FCR.sh $percorso $M $tmp $files

NOCC=`wc -l  < $tmp`

echo SONO STATE TROVATE $NOCC OCCORRENZE...
if test ! $NOCC -eq 0
then
    echo ======================
    cat $tmp
    echo ======================
fi

rm $tmp
