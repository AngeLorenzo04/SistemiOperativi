#!/bin/sh
#FILE COMANDI RICORSIVO INERENTE ALLA PROVA D'ESAME DEL 11 APRILE 2018

cd $1

#Variabile che tiene conto del numero di righe
righe=
#Variabile che salva i file validi
file=

#Cerco e salvo i file validi nella lista file
for i in *
do
	echo DEBUG-ANALYZE $i
	if test -f $i -a -r $i
	then 
		righe=`wc -l < $i`
		if test $righe -ge $2
		then
			file="$file $i"
		fi
	fi
done


#Variabile che contiene il nome del nuovo file
nFile=

for i in $file
do
	echo DEBUG-VALID $i
	righe=`wc -l < $i`
	if test $righe -gt 5
       	then
		nFile=$i.quinta
		> $nFile
		echo `head -5 $i | tail -1` >> $nFile

	else
		nFile=$i.NOquinta
		> $nFile
	fi
	echo `pwd`/$nFile >> $3
done


#PARTE RICORSIVA
for i in *
do
	if test -d $i -a -x $i
        then
        	FCR.sh `pwd`/$i $2 $3
	fi
done
