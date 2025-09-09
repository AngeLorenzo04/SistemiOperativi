#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 3 Giugno 2020

#Controllo comandi LASCO
if test $# -lt 3
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 3 parametri
   exit 1
fi

#Controllo che il primo parametro sia un carattere 
C=
case $1 in 
    ?)
        C=$1
        shift
        ;;
    *) 
        echo ERRORE: il parametro $1 non è valido
        echo SUGGERIMENTO: Inserire soltato un carattere
        exit 2
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
for G in $percorsi
do
    FCR.sh $G $C $tmp
done

#Mostro i risultati della ricerca
NOCC=`wc -l  < $tmp`

echo SONO STATE TROVATE $NOCC OCCORRENZE...
if test ! $NOCC -eq 0
then

    echo
    for D in `cat $tmp`
    do
        echo La direcotry $D corrisponde alla specifica! Ciao ELENA, desideri visualizzare il contenuto di $D \(inclusi i file nascosti\) ?[Y/n]
        read ans
        case $ans in 
            y|Y|s|S)
                echo Ecco il conenuto della directory...
                cd $D
                ls -a $D 
                cd
                ;;
            *)
                ;;
        esac
    done

fi

rm $tmp