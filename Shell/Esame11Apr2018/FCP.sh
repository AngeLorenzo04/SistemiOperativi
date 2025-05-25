#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA D'ESAME DEL 11 APRILE 2018
#Controllo parametri LASCO
case $# in
	0|1) echo ERRORE: inserire almeno 3 parametri
		exit 1;;

	*) ;;
esac
#arrivati qui il numero di parametri è corretto

#Salvo il primo parametri
Y=$1
shift

expr $x + 0 > /dev/null 2>&1
N=$?
if test $N -ne 0
then 
	echo ERRORE: primo parametro deve essere un valore numerico
	exit 2
else
	if test $x -le 0
	then 
		echo ERRORE: il primo parametro deve essere un valore numerico strettamente positivo
		exit 3
	fi
fi

#Controlliamo gli altri parametri
for i 
do
	case $i in
		/*) 
			if test ! -d $i -o ! -x $i
			then
				echo ERRORE: $i  non una directory o non traversabile
				exit 4
			fi ;;
		*)
			echo ERRORE: $i non una directory
			exit 5;;	
	esac
done

#Siamo sicuri che i parametri siano corretti

#Esportiamo la variabilie path
PATH=`pwd`:$PATH
export PATH

#Creao file temporaneo
tmp=/tmp/valid$$
> $tmp


for i 
do 
	echo DEBUG-ITERATION $i
	FCR.sh $i $Y $tmp
done 


numeroFileCreati=`wc -l < $tmp`
echo ===============================
echo numero di file creati $numeroFileCreati
echo ==============================

for i in `cat $tmp`
do
	echo =================
	echo file $i
	echo `cat $i`
	echo =================
done
	


rm $tmp
