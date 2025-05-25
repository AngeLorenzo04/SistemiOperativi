#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA D'ESAME DEL 22 APRILE 2021

#Controllo comanidi lasco
case $# in
	0|1|2|3)
		echo ERRORE: numero parametri insufficiente
		echo SUGGERIMETO: inserire almeno 4 parametri
		exit 1;;
	*)
		;;
esac

#Arrivati qui il numero di parametri è corretto
#controlllo sul primo parametro
#Variabile che memorizza il primo parametro
W=$1
shift

`expr $W + 0 >> /dev/null 2>&1` 
N=$?

if ! test $N -eq 0
then
	echo ERRORE: $i parametro non valido
	echo SUGGERIMENTO: inserire un valore numermico positivo
	exit 2
else
	if test $W -le 0
	then
		echo ERRORE: $i parametro non valido
		echo SUGGERIMENTO: inserire un valore numerico positivo
		exit 3
	fi
fi

#Controllo sul secondo parametro
#Variabile che memorizza il seconod parametro
S=$1
shift

case S in
	*/*) 
		echo ERRORE: $S non puo essre un percorso
		exit 4;;
	*)
		;;
esac

#Controllo su tutti gli altri Q parametri
Q=

for i
do
	case $i in
		/*)
			if test ! -d $i -o ! -x $i
			then
				echo ERRORE: $i parametro non valido
				echo SUGGERIMENTO: inserire una directory
				exit 5
			fi
			Q="$Q $i";;	
		*)
			echo ERRORE: $i non in forma assoluta
			echo SUGGERIMENTO: inserire un percorso in forma assoluta
			exit 6;;
		esac
done
#Arrivati qui i paramteri sono corretti
#Esporto la variabile d'ambiente PATH
PATH=`pwd`:$PATH
export PATH

#Creo un file per contenere i dati necessari
tmp=/tmp/nomiAssoluti$$
> $tmp


#Chiamo iterativamente il file comandi ricorsivo
for i in $Q
do
	echo DEBUG-ITERATION $i
	FCR.sh $i $S $tmp
done

#Variabile che conta le dir valide
validDir=`wc -l < $tmp`

echo
echo ::::::::::::::::::::::::::::::::
echo : numero di directory valide $validDir :
echo ::::::::::::::::::::::::::::::::
echo

if test $validDir -ge $W
then
	#Contatore per il gioco
	cont=0

	echo Ciao $USER, scegli un numero tra 1 e $W
	read ans
	for i in `cat $tmp`
	do
	       echo DEBUG-cont $cont	       
		if test $cont -eq $ans
		then
			echo 
			echo IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
			echo I file $ans: $i   I
			echo IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
			echo
		fi
		cont=`expr $cont + 1`
	done
fi

rm $tmp
