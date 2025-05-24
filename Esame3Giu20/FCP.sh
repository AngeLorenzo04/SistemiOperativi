#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA D'ESAME DEL 3 GIUGNO 2020

#Controllo parametri LASCO
case $# in
	0|1)
		echo ERRORE: numero parametri insuficente
		echo SUGGERIMENTO: inserire almeno 3 parametri
		exit 1;;
	*);;
esac
#Arrivati qui il numermo di parametri è corretto

#Controllo sul primo parametro deve essere un singolo carattere
#Variabile che conterrà il valore del primo parametro
C=$1
shift

if test ${#C} -gt 1
then
	echo ERRORE: $i non è un carattere
	echo SUGGERIMENTO: inserire un caratere
	exit 2
fi


#Controllo sui successivi N parametri devono essere delle directory in forma assoluta
for i 
do
	case $i in
		/*)
			if test ! -d $i -o ! -x $i
			then
				echo ERRORE: $i non una direcotry o non traversabile
				echo SUGGERIMENTO: passare una directory traversabile in forma assoluta 
				exit 3
			fi
			;;
		*) 
			echo ERRORE $i non una directory in forma assoluta
			echo SUGGERIMENTO: passare uan directory traversabile in forma assoluta
			exit 4;
	esac
done
#Arrivati qui siamo sicuri che i parametri passati siano corretti

#Creo il file che conterrà le soluzioni
tmp=/tmp/nomiAssoluti$$
> $tmp

#Esecuzione iterativa del file comandi ricorsivo
for i 
do
	`pwd`/FCR.sh $i $C $tmp
done

echo

echo =====================================================================
echo I       numero di directory trovate: `wc -l < $tmp`                'I'
echo =====================================================================

echo

echo ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
echo
for i in `cat $tmp`
do
	echo $i
	echo Ciao Elena, desideri visualizzare gli elementi di questa directory? y/n
	read ans
	case $ans in
		y|Y)
			echo
			echo /----------------------------------------/
			ls -la $i
			echo /----------------------------------------/
			echo;;
		*)
			echo;;
	esac
done
echo :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

echo





#Aggiorno e esporto la variabile ambiente PATH
PATH=`pwd`:$PATH
export PATH


rm $tmp
