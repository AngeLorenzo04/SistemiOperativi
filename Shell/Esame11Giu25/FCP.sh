#!/bin/bash
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVE D'ESAME DEL 11 GIUGNO 2025

#Controllo comandi lasco

case $# in
    0|1|2)
        echo ERRORE: numero parametri insufficiente
        echo SUGGERIMENTO: inserire almeno 3 parametri
        exit 1;;
    *) ;;
esac

#Salvo il primo parametro e shifto tutti gli altri di una posizione verso <-
d=$1
shift


#Controllo che il primo parametro sia un percorso assoluto
case $d in
    */*) 
        if test ! -d $d -o ! -x $d
        then 
            echo Non una direcotry o non traversabile
            echo SUGGERIMENTO: inserire nome assoluto di una directory
            exit 2
        fi
        ;;
    *)
        echo ERRORE: non un percorso in forma assoluta
        echo SUGGERIMENTO: inserire nome assoluto di una directory
        exit 3 ;; 
esac 


#Controllo che i parametri inseriti siano singoli caratteri
caratteri=
for i
do

    case $i in
    ?)     
        caratteri="$caratteri$i";;
    *) 
        echo ERRORE: parametro $i non valido
        echo SUGGERIMENTO: insierire un singolo carattere
        exit 5 ;;
    esac


done


#Esporto la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creo un file temporaneo dove salvare le risposte
tmp="/tmp/Result"
>$tmp

echo DEBUG-FCP-CARATTERI $caratteri

#Chiamo una sola volta il file comandi ricorsivo
echo SINGOLA ITERAZIONE 
FCR.sh $d $caratteri $tmp

#Conto le righe del file dove ho salvato le occorrenze 
NRO=`wc -l < $tmp`

if test $NRO -eq 0
then 
    echo Non sono state trovate occorrenze
    rm $tmp
    exit 6
fi

echo Sonoo state trovate $NRO occorrenze globali
echo LISTA OCCORRENZE TROVATE
echo ================================
echo `cat $tmp`
echo ================================

rm $tmp




