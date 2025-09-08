#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 17 Aprile 2024

#Controllo comandi LASCO
if test $# -lt 3
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
tmp="/tmp/nomiAssoluti"
> $tmp

#Invocazione iterativa del file comandi ricorsivo
echo
for G in $percorsi
do
    FCR.sh $G $tmp
done
echo

NOCC=`wc -l  < $tmp`

if test $NOCC -eq 0
then
    echo ERRORE: Non sono state trovate occorrenze
    exit 5
fi

echo Sono state trovate $NOCC occorrenze
echo
echo creo la directory $X...
mkdir $X 2>/dev/null
echo done...
echo Creo i link Hardware...
for i in `cat $tmp`
do 
    ln $i $X 2>/dev/null
done
echo done...
echo FINISH

rm $tmp