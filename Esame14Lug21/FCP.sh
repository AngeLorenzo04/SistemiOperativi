#!/bin/sh
#FILE COMANDI PRINCIPALE PER LA PROVA D'ESAME DEL 14 LUGLIO 2021

#Controllo parametri LASCO
case $# in
	0|1|2|3) echo ERROR: numero parametri insufficente
		exit 1;;
	*) ;;
esac

#Arrivati qui il numero dei parametri e' correto

#Contatore per monitorare il ciclo
cont=1
#Variabili per memorizzare i valori numerici
H=
M=
#Variabile per memorizzare i percorsi passati
file=

for i
do
	if test  $cont -lt `expr $# - 1`
	then
		case $i in
			/*) 
				if test ! -d $i -o ! -x $i
				then
					echo ERROR: non e una direcotry o non e traversabile
					exit 2
				fi
				file="$file $i";;
			*)
			       	echo ERROR: percorsi non in forma assoluta
				exit 2;;
		esac
	else
		
		expr $i + 0 >> /dev/null 2>&1
		N=$?
		if test ! $N -eq 0
		then 
			echo ERROR: valore $i  non numerico

			exit 3
		else 
			if test $i -le 0
			then 
				echo ERROR: valore negativo
				exit 3
			fi
		fi
		
		if test $cont -eq `expr $# - 1`
		then
			H=$i
		else
			M=$i
		fi
	fi
	cont=`expr $cont + 1`
done

#Aggiorno la variabile PATH
PATH=`pwd`:$PATH
export PATH


for G in $file
do
	echo DEBUG-$G
	FCR.sh $G $H $M
done

