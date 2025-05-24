#!/bin/sh
#FILE COMANDI RICORSIVO INERENTE ALLA PROVA D' ESAME DEL 15 APRILE 2016	

cd $1

#checker dir
checkdir=0

echo DEBUG-ANALYZE $1

#Controllo che ci siano solo file
for i in *
do
	if ! test -f $i 
	then 
		checkdir=1
		break
	fi
done


#Arrivati qui siamo sicuri che nella directory ci siano solo file

#checker file
checkfile=0
#Numero di righe
righe=

if test  $checkdir -eq 0 #Se ci sono solo file
then #ALLORA
	for i in * #Controlla per ogni file
	do
		righe=`wc -l < $i`
		if test ! $righe -gt $2 #che la sua lunghezza sia maggiore del parametro numerico
		then
			checkfile=1 #Segnala la presena di file non validi
			break
		fi
	done
	
	if test $checkfile -eq 0 #Se ci sono solo file validi
	then
		echo `pwd` >> $3 #aggiorno la lista di directory globale
	fi

else

	#Esploro ricorsivametne la directory
	for i in *
	do
        	if test -d $i -a -x $i
        	then
			echo DEBUG-PARAM `pwd`/$i $2 $3
                	FCR.sh `pwd`/$i $2 $3
        	fi
	done

fi
