#!/bin/bash
#File Comandi Ricorsivo inerente alla prova d'esame del 9 Luglio 2025


#Mi sposto nella cartella desiderata
cd $1

#Salvo i parametri in delle variabili
X=$2 
L=$3
tmp=$4
len=

#Cerco le occorrenze 
for i in *
do 
    #Verifico se siamo in presenza di file
    if test -f $i
    then 
        if test -r $i 
        then 
            #Calclolo la lunghezza del file
            len=`cat $i | wc -l`

            #Cerco tutti i file .sh
            case $i in
                *.sh) 
                    #Verifico che la lunghezza sia valida
                    if test $len -lt $X 
                    then
                        #Aggiorno la lunghezza massima
                        if test $L -lt $len 
                        then
                            L=$len
                        fi

                        #Memorizzo l'occorenza trovata
                        echo `pwd`/$i >> $tmp
                    fi

                    ;;
                *)
                    ;;
            esac
        fi
    fi 
done 

#Chiamata ricorsiva del file FCR.sh
for i in *
do
   if test -d $i -a -x $i
   then
      $0 `pwd`/$i $X $L $tmp
   fi
done