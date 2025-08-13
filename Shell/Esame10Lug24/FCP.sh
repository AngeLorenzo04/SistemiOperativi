#!/bin/bash
#File comandi principale inerente alla prova d'esame del 10 luglio 2024

#Controllo parametri LASCO
if test $# -lt 2
then 
    echo ERRORE: numero parametri non valido
    echo SUGGERIMENTO: inserire almeno 4 parametri
    exit 1
fi

N=$1
shift

#Controllo i parametri

#Verifico che il primo parametro sia un valore intero minore di 20
test=`expr $N + 1`
if test $? -eq 1
then
    echo ERRORE: il parametro $N non è valido
    echo SUGGERIMENTO: il primo parametro deve essere un numero intero minore di 20
    exit 2
fi

if test $N -gt 20
then 
    echo ERRORE: il parametro $N non è valido
    echo SUGGERIMENTO: il primo parametro deve essere un numero intero minore di 20
    exit 3
fi

#Controllo che i prossimi parameteri siano percorsi assoluti di directory
percorsi=

for i
do 

    case $i in
    */*)
        if test  ! -d $i -o ! -x $if
        then 
            echo ERRORE: parametro $i non valido
            echo SUGGERIMENTO: inserire un percorso assoluto di una directory traversabile
            eixt 4
        fi
        percorsi="$percorsi $i"
        ;;
    *)
        echo ERRORE: parametro $i non valido
        echo SUGGERIMENTO: inserire un percorso assoluto di una directory traversabile
        eixt 5
        ;;
    esac
done


#Aggiorno la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creo file temporaneo che conterrà i risultati della parte ricorsiva
tmp="/tmp/Risultati"
> $tmp

#Invocazione iterativa del file ricorsivo
for G in $percorsi
do
    FCR.sh $G $N $tmp
done


#Mostro il contenuto del file temporaneo 

NRO=`wc -l < $tmp`

echo sono stati trovati $NRO file che rispettano le specifiche a,b
echo LISTA DELLE OCCORRENZE
echo ========================
cat $tmp
echo ========================

rm $tmp

