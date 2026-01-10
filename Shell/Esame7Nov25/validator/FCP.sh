#!/bin/bash
#File Comandi Principale inerente alla prova d'esame del 7 Nonvembre 2025

#Controllo comandi LASCO
if test $# -lt 2
then
   echo ERRORE: numero parametri non valido
   echo SUGGERIMENTO: inserire almeno 2 parametri
   exit 1
fi

#Memorizzo il primo parametro
# G=
# case $1 in
#     */*)
#         if test ! -d $1 -o ! -x $1
#         then
#            echo ERRORE: il parametro $1 non è valido
#            echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
#             exit 2
#         fi
#         G=$1
#         shift
#         ;;
#     *)
#         echo ERRORE: il parametro $1 non è valido
#         echo SUGGERIMENTO: inserire una directory traversabile in forma assoluta
#         exit 3
#     ;;
# esac

#Memorizzo il percoso inserito
G=$1
shift

#Controllo che gli altri parametri siano dei caratteri
caratteri=
for i
do
    case $i in
    ?)
        caratteri="$caratteri $i"
        ;;
    *)
        echo ERRORE: il parametro $i non è valido
        echo SUGGERIMENTO: inserire un carattere
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
FCR.sh $G "$caratteri"  $tmp


NOCC=`grep + $tmp | wc -l`

echo Total files: $NOCC
if test ! $NOCC -eq 0
then
    cat $tmp
fi

rm $tmp

