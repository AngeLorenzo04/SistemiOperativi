#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA D'ESAME DEL 17 APRILE 2024

#Controllo parametri LASCO
case $# in
    [0-1]) 
        echo ERRORE: nupero parametri insufficiente \nSUGGERIMENTO: inserire almeno 4 parametri
        exit 1;;
    *);;
esac

#Arrivati qui il numero di parametri è corretto 
#Controllo sui parametri 

#Controllo che il parametro sia un nome relativo semplice
case $# in
    */*)
        echo ERRORE inserire un nome relativo semplice
        exit1;;
    *);;
esac

X=$1
shift
#Controllo che le gerarchie passate siano traversabili
for G
do
        case $G in
        /*) if test ! -d $G -o ! -x $G
            then
                echo Errore: $G non directory o non traversabile
                exit 4
            fi;;
        *)  ;;
        esac
done



#AGGIORNO LA VARIABILE AMBIENTE PATH
PATH=`pwd`:$PATH
export PATH

#Creo il file dove salvare i risultati di FCR
tmp=/tmp/nomiAssoluti$$
>$tmp

#Parto con la chiamata iterativa
for G 
do
    echo ITERATION-$G
    FCR.sh $G $tmp
done

NRO=`wc -l < $tmp`

if test $NRO -eq 0
then
    echo NON SONO STATI TROVATI FILE NELLA TERZA POSIZIONE NELLE VARIE DIRECTORY
    rm $tmp
    exit 0
fi

echo \n
echo DEBUG-LISTA FILE TROVATI
echo ========================
`cat $tmp`
echo ========================
echo \n

echo CREAZIONE DIRECTORY...
mkdir $X 
echo CREAZIONE LINK HARDWARE...
for F in `cat $tmp`
do
    ln $F $X
done

rm $tmp


