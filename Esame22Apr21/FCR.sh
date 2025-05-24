#!/bin/sh
#FILE COMANDI RICORSIVO INERETE ALLA PROVA D'ESAME DEL 22 APRILE 2021

cd $1


for i in *
do
	if test -f $i -a $i=*.$2
	then
		echo $1 >> $3
		break
	fi
done

#Esplorazione ricorsiva delle directory
for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done

