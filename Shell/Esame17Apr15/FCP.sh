#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA D'ESAME DEL 17 APRILE 2015
#controllo parametri LASCO 
case $# in
	[0-2]) 
		echo ERROR: numero parametri non valido
		exit 1;;
	*);;
esac
#Se siamo arrivati qui significa che il numemro di parametri e valido
#Controlli sul primo parametro
expr $1 + 0 > /dev/null 2>&1
N=$?
if test $N -ne 0
then 
	echo ERROR: il primo parametro non e un valore numerico
	exit 2
else
	if test $1 -lt 0
	then 
		echo ERROR: il primo parametro deve essere positivo
		exit 2
	fi
fi
#Controlli su gli altri parametri
x=$1
shift

for i 
do
        case $i in
        /*) if test ! -d $i -o ! -x $i
            then
                echo Errore: $i non directory o non traversabile
                exit 4
            fi;;
        *)  echo Errore: $i non nome assoluto; exit 5;;
        esac
done




#Aggiorno la variabile PATH
PATH=`pwd`:$PATH
export PATH

>/tmp/conta$$ #creo il file contenete la soluzione

for i 
do
	echo Iterazione per il percorso $i
	FCR.sh $x $i /tmp/conta$$
done

echo Il numero di file totali che soddisfano i parametri e `wc -l < /tmp/conta$$`



#Variabile che contiene la riga che si vuole leggere
R=
for i in `cat /tmp/conta$$`
do
	echo Inserire la riga che si viole leggere dal file $i
	read R
	expr $R + 0 > /dev/null 2>&1
	N=$?
	if test $N -ne 0
	then
		echo ERROR: inserire un valore numerico
		exit 6
	else 
		if test $R -le 0
		then 
			echo ERROR: inserire un valore positivo
		fi
	fi
	echo `head -$R $i | tail -1`
done


rm /tmp/conta$$
