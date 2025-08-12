#!/bin/bash
#File Comandi Ricorsivo inerente alla prova del 12 Giugno 2024

#Mi sposto nella directory interessata 
cd $1

#salvo i parametri in delle variabili
tmp=$2
tmp1=$3

ab=0
c=0

#Cerco le occorrenze
for i in *
do

    echo analizzo $i
    if test -d $i -a -x $i
    then
        case $i in  #Controllo che il nome della directory rispetti i primi due canoni
        ????)
            c2=${i:1:1}
            c4=${i:3:1}
            
            if test $c2 = $c4
            then
                echo C2 $c2
                echo C4 $c4
                echo $i rispetta le condizioni a,b
                ab=1

            fi
            ;;
        *)
            ;;
        esac

        if test $ab -eq 1 #Se effettivamente li rispetta
        then
            for f in $i  #Controllo che quella directory abbia dei file
            do 
                if test -f $f 
                then       
                    echo controllo file $f     
                    len=`wc -l < $f`    
                    echo controllo file $f con lunghezza $len
                    if test $len -gt 0  #Se il file non è vuoto
                    then
                        len=`expr $len%2`
                        if test $len -eq 0  #Se il file ha un numero di righe pari
                        then
                            c=1 #Allora la directory rispetta la condizione c
                        fi
                    fi
                fi
            done
        fi


        #Salvo negli appositi file temporanei le varie directory
        if test $ab -eq 1
        then
            echo `pwd`/$i >> $tmp
        fi
          if test $c -eq 1
        then
            echo `pwd`/$i >> $tmp1
        fi

        #Rollback dei valori delle variabili booleane
        c=0
        ab=0
    fi
done

#Parte ricorsiva
for i in *
do
        if test -d $i -a -x $i
        then
                #invocazione ricorsiva
                FCR.sh `pwd`/$i $tmp $tmp1
        fi
done