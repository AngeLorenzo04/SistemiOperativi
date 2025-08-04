#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA DEL 11 APRILE 2025

#Controllo parametri LASCO
case $# in 
    0|1|2) 
        echo ERRORE: numero parametri insufficiente\n 
        echo SUGGERIMENTO: inserire almeno 3 parametri
        exit 1;;
    *);;
esac

#Controllo che tutti i parametri tranne l'ultimo siano percorsi assoluti

num=1
parametri=

for i 
do
    if test $num -ne $#
    then
        case $i in 
        /*) if test ! -d $i -o ! -x $i 
            then 
                echo ERRORE: il parametro $i non è traversabile o non è una directory
                echo SUGGERIMENTO: inserire una directory traversabile
                exit 2
            fi;;
        *)  
            echo ERRORE: il parametro $i non è in forma assouta
            echo SUGGERIMENTO: usare il perocorso assoluto
            exit 3;;
        esac
        parametri="$parametri $i"

    else
        case $i in
        ?) echo EROORE: $i contiene un solo carattere
            exit 4;;
        */*) echo ERRORE: $i contiene / 
            exit 5;;
        esac
    fi
        S=$i
        num=`expr $num + 1`
done


PATH=`pwd`:$PATH
export PATH

tmp='/tmp/nomiAssoluti'
> $tmp

echo $parametri
echo

for G in $parametri
do 
    echo ITERAZIOE $G
    FCR.sh  $G $S $tmp
done

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






