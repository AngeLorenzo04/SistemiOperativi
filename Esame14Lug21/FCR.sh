#!/bin/sh
#FILE COMANDI RICORSIVO INERENTE ALLA PROVA D'ESAME DEL 14 LUGLIO 2021


cd $1

#Contatore di file
cont=0
#Lista con i file validi
file=
#Lista di directory valide
dir=
#Contiamo i file all interno della directory
for i in *
do
       echo DEBUG-ANALYZE $i	
	if test ${#i} -eq 2
	then
		L=`wc -l < $i`
		if test $L -eq $3
		then
			echo DEBUG-VALID $i
			cont=`expr $cont + 1`
			file="$file $i"
		fi	
	fi
done

if test $cont -gt 2 -a $cont -lt $2
then
	echo DEBUG-VALID_DIR $1 
	echo ================
	echo $file
	echo ================
fi


for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh $i $2 $3
	fi
done


