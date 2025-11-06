#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 17 Aprile 2024

#Controllo comandi LASCO
if test $# -lt 2
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Verifico se il parametro $1 è un nome relativo
case $1 in
   */*)
      echo ERRORE: il parametro $1 non è valido
      echo SUGGERIMENTO: Inserire un nome relativo semplice
      exit 2
      ;;
   *)
      X=$1
      shift
      ;;
esac

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

#Creazione file temporaneo che conterrà il risultato della ricerca
tmp="/tmp/NomiAssoluti"
> $tmp

#Invocazione iterativa del file comandi ricorsivo
for G in $percorsi
do
    FCR.sh $G $tmp
done

NOCC=`wc -l  < $tmp`

if test ! $NOCC -eq 0
then
    echo ===================================================================================
    echo Sono state trovate $NOCC occorrenze
    echo ===================================================================================

    echo procedo a creare la directory $X 
    mkdir $X
    cd $X
    echo DONE: la dir $X è stata creata con sucesso
    ehco procedo a creare i lh
    for file in $tmp 
    do
        ln -h $file
    done
    echo DONE: lh creati con successo
else
    echo ERRORE: non sono state trovate occorrenze
    exit 5
fi

rm $tmp