#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA D'ESAME DEL 7 GIUGNO 2023

# Controllo il numero dei parametri
case $# in
[0-2]) echo ERROR: numero di parametri non accettabile: $#
     exit 1;;
*)echo DEBUG-OK: numero di parametri corretto;;
esac



#Controllo che il primo parametro sia un valore numerico maggiore di 4
expr $1 + 0  > /dev/null 2>&1
N1=$?
if test $N1 -ne 2 -a $N1 -ne 3
then # in questo caso è certamente numerico
    echo DEBUG-OK: $1 è numerico
else
    echo ERROR: $1 non numerico
    exit 2
fi

X=$1

if test ! $X -lt 4 -o $X -le 0
then
    echo ERROR: il valore numerico deve essere strettamente positivo e strettamente minore di 4
    exit 3
else 
    echo DEBUG-OK $X e\' positivo e strettamente minore di 4
fi

shift #Serve per far in modo che il secondo parametro diventi il primo e cos' via

#Controllo che i percorsi passati siano percorsi traversabili e assoluti
for i 
do

    case $i in
    /*) if test ! -d $i -o ! -x $i
        then
        echo $i non directory
        exit 4
        fi;;
    *)  echo $i non nome assoluto; exit 5;;
    esac

done

tmp=/tmp/RISULTATO$$ #Creo un file dove salvare i risultati
>$tmp 

PATH=`pwd`:$PATH #Aggiorno la variabile PATH
export PATH

#Chiamata iterativa di FCR.sh
for G 
do 

    echo DEBUG-ITERAZIONE $G 
    FCR.sh $G $X $tmp 

done


#Stampo il risultato di FCR.sh
for D in `cat $tmp`
do
    echo E\' STAT TROVATA LA DIRECTORY:$D

done

rm $tmp