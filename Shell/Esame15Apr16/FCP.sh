#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA D'ESAME DEL 15 APRILE 2016

#Controllo dei parametri LASCO

case $# in
	0|1|2) 
		echo ERRORE: numero di parametri troppo corto devono essere almeno 3
		exit 1;;
	*);;
esac

#Controllo sui primi N parametri
#Devono essere nomi assoluti di directory

#Contatore che scorre l indice dei parametri
cont=1
#Variabile contenete le directory
dir=
#Variabile contenente il valore numerico
x=

for i
do
	if test ! $cont -eq $#
	then 
		case $i in
			/*) #Caso in cui stiamo esaminando una nome assoluto
			       	if test ! -d $i -o ! -x $i
				then 
					echo ERRORE: $i non una directory o non traversabile 
					exit 2
				fi
				dir="$dir $i";;
			*) #Caso in cui non e stato passato un nome assoluto
				echo ERRORE: $i  non nome assoluto di directory
				exit 3;;
		esac
	else
		#Controllo sul ultimo parametro deve essere un intero positivo
		`expr $i + 0 >> /dev/null 2>&1`
		N=$?
		if test $N -ne 0 #Verifico che l ultimo parametro sia un numero
		then 
			echo ERRORE: $i deve essere un parametro numerico
			exit 4
		else
			if test $i -le 0
			then
				echo ERRORE: $i deve essere positivo
				exit 5
			fi
			x=$i
		fi
	fi
	cont=`expr $cont + 1`
done


#Aggiorno la variabile path
PATH=`pwd`:$PATH
export PATH

#Creo file con i nomi delle directory valide 
> /tmp/valid$$
#Parto con le chiamate iterative di FCR
for i in $dir
do 
	echo DEBUG-ITERATION $i
	FCR.sh $i $x /tmp/valid$$ 
done

validDir=`wc -l < /tmp/valid$$`
echo ========================================
echo  NUMERO DI DIRECTORY VALID: $validDir 
echo =======================================

for i in `cat /tmp/valid$$`
do
	for j in $i/*
	do
		echo =======================
             	echo $j
             	echo ------------------------
             	echo `tail -$x $j | head -1`
		echo =======================
	done
done

rm /tmp/valid$$
