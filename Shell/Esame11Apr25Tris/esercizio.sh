#!/bin/bash

# funzione di utilizzo
uso(){
    echo "Uso: $0 <dir1> <dir2> ... <dirn> <string>"
    echo "  <dirN> deve essere il nome di una directory esistente"
    echo "  <string> deve essere una stringa di almeno 2 caratteri"
    exit 1
}

# Controllo se il numero di parametri è corretto

if [ "$#" -lt 3 ]
then
    uso
fi

# estraggo i parametri
dirs=""
count=1
total_args=$#

while [ $count -lt $total_args ]
do
    if [ -d "$1" ]
    then
        dirs="$dirs $1"
    fi
    shift
    ((count++))
done

s="$1"

#Controllo se i parametri sono validi
len=${#s}
if [ $len -lt 2 ]
then
    uso
fi

nro=0
for d in $dirs
do
    if [ -d "$d" ]; then
        out=$(find "$d" -type f -name "*.$s" ! -executable -exec dirname {} + | sort -u)
        if [ -n "$out" ]
        then
            echo "$out"
            ((nro++))
        fi
    fi
done

if [ $nro -eq 0 ]
then
    echo "Nessuna directory contiene file validi"
    exit 2
else
    echo "$nro directory contengono file validi"
fi
