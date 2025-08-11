#!/bin/bash
#FIle comandi principale inerente alla prova d'esame del 12 Aprile 2023


#Controllo parametri LASCO
if test $# -lt 3
then
    echo ERRORE: numero parametri insufficiente
    echo SUGGERIMENTO: Inserire alemno 4 parametri
    exit 1
fi

#Salvo la prima stringa in una variabile
S1=$1
shift
#Salvo la seconda stringa in una variabile
S2=$1
shift

#Controllo che gli altri parametri siano validi
percorsi=
for i 
do
    case $i in
        */*) 
            if test ! -d $i -o ! -x $i
            then 
                echo ERRORE: parametro $i non valido
                echo SUGGERIMENTO: inserire un percorso assoluto di una directory
                exit 2
            fi
            percorsi="$percorsi $i"
            ;;
        *) 
            echo ERRORE: parametro $i non valido
            echo SUGGERIMENTO: inserire un percorso di una directory
            exit 3
            ;;
    esac
done

PATH=`pwd`:$PATH
export PATH

tmp1="/tmp/estensione1"
tmp2="/tmp/estensione2"
> $tmp1
> $tmp2


for G in $percorsi
do
    FCR.sh $G $S1 $S2 $tmp1 $tmp2
done
#Conto le righe del file dove ho salvato le occorrenze 
NRO1=`wc -l < $tmp1`
NRO2=`wc -l < $tmp2`


if test $NRO1 -ge $NRO2
then
    echo inserisci un numero tra 1 e $NRO2
    read selected
    echo `head -n $selected $tmp1 | tail -n 1 $tmp1`
fi

tot=`expr $NRO1 + $NRO2`
echo Sonoo state trovate $tot occorrenze globali
echo LISTA OCCORRENZE TROVATE S1: $NRO1
echo ================================
echo `cat $tmp1`
echo ================================
echo LISTA OCCORRENZE TROVATE S2: $NRO2
echo ================================
echo `cat $tmp2`
echo ================================

rm $tmp1
rm $tmp2



