#!/bin/sh
#FILE COMANDI RICORSIVO DELLA PROVA D'ESAME 15 FEB 2017

cd $1 #ci spostiamo nella directory scelta

#Definisco una variabile per tenere traccia del numero di righe
righe= 
#Definisco una variabile per tenere tracciadel nunmero di righe numerate
righeNumerate=
#Definisco una variabile che conterrà i percorsi dei file validi
fileValidi=

for i in *
do
	if test -f $i -a -r $i
	then
		righe=`wc -l < $i`  #aggiorno il numero di righe
		righeNumerate=`grep '[0-9]' $i | wc -l` #aggiorno il numero di righe che contengono numeri
		if test $righe -eq $2 -a $righe -eq $righeNumerate  #se il numero di righe è pari al parametro e il numero di righe numerate è uguale al totale delle righe
		then #allora
			fileValidi="$fileValidi $i" #salvo il pergorso del file
		fi
	else 
		if test -d $i -a -x $i
		then 
			$0 $i $2
		fi	
	fi

done

echo in `pwd` 'i file validi sono' $fileValidi
echo =============================================================
