#!/bin/sh
#FILE COMANDI PRINCIPALE INERENTE ALLA PROVA D'ESAME DEL 5 APRILE 2019
#Controllo parametri LASCO

case $# in
	0|1)
		echo ERRORE: parametri insufficenti
		echo SUGGEST: inserire almeno 3 parametri
		exit 1;;
	*)
		;;
esac
#Arrivati qui il numero di parametri e' corretto
#Controgliamo i primi N parametri

#Variabile che tiene conto di quale parametro stiamo studiando
cont=1
#Variabile che conterrà la lista di percorsi
dir=
#Variabile che conterrà la stringa
S=

for i 
do 
	if test ! $cont -eq $#
	then
		case $i in
			/*)
				if test ! -d $i -o ! -x $i
				then
					echo ERRORE: $i non e\' una directory o non e\' traversabile
					echo SUGGEST: inserire solo directory traversabili
					exit 2
				fi
				dir="$dir $i";;
			*)
				echo ERROR: $i non e\' un percorso in forma assoulta
				echo SUGGEST: inserire solo directory in forma assoluta
				exit 3;;
		esac
		#Arrivati qui il controllo sui primi N parametri e' terminato
	else
		case $i in
			*/*)
				echo ERRORE: $i deve essere una stringa non in percorso
				exit 4;;
			*)
				S=$i;; #Salvo la stringa
		esac
	fi
	cont=`expr $cont + 1` #incremento la variabile cont
done
#Arrivati a questo punto il controllo dei parametri è terminato
#Aggiorno la variabile ambiente path
PATH=`pwd`:$PATH
export PATH

#Creo file che conterra la soluzione da mostrare
tmp=/tmp/valid$$
> $tmp

#Chiamo iterativamente il file FCR.sh
for i in $dir
do
	FCR.sh $i $S $tmp
done

echo ===========================================
echo numero totale di file trovati `wc -l < $tmp`
echo ===========================================

echo
echo
cat $tmp
echo
echo


#Variabile che tiene conto di cosa stiamo mostrando
cont=1

for i in `cat $tmp`
do
	
	for j in $i
	do
		if test ! `expr $cont % 2` -eq 0 
		then
			echo ===============================
			echo Lunghezza file in caratteri: $j
		else
			echo Percorso assoluto del file: $j
			echo ::::::::::::::::::::::::::::::
			cat $j
			echo ::::::::::::::::::::::::::::::
			echo
			echo Ordinare il file $j? y/n
			read ans
			case $ans in
				y|Y)
					echo
					echo Ordinamento di $j
					echo ::::::::::::::::::::::::::::
					sort $j
					echo ::::::::::::::::::::::::::::
					;;
				*)
					;;
			esac
		fi
		cont=`expr $cont + 1`
		
	done

done


rm $tmp

