#!/bin/sh
#FILE COMANDI RICORSIVO DINERENTE ALLA PROVA D'ESAME DEL 17 APR 2015


cd $2

#Variabile per salvare il numero di righe che terminano con t
righeT=
for i in * 
do 
	if test -f $i -a -r $i
	then
		righeT=`grep 't$' $i | wc -l`
		if test $righeT -ge $1
		then
		    	echo `pwd`/$i >> $3
		fi	       
	else 
		if test -d $i -a -x $i
		then	
			FCR.sh $1 `pwd`/$i $3
		fi
	fi
done
			


