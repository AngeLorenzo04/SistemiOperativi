#!/bin/sh
#FILE COMANDI RICORSIVO INERENTE ALLA PROVA D'ESAME DEL 3 GIUGNO 2020

cd $1

#Cerco i file che mi possono interessare
for i in *
do
	D=`pwd`/$i #Directory in cui ci troviamo
	case $D in
		*/$2?$2)
			if test -d $i
			then
				echo $D >> $3
			fi;;	
		*);;
	esac
done

#Esploro ricorsivamente la directory in cui mi trovo
for i in *
do
	if test -d $i -a -x $i
	then
	 /home/acer/esercizi_SisOp/Esame3Giu20/FCR.sh `pwd`/$i $2 $3
	fi
done
