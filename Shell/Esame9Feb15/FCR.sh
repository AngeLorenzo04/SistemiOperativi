#!/bin/sh
# FILE COMANDI RICORSIVO DELLA PROVA D'ESAME DEL 9 FEBBRAIO 2015

cd "$1" || exit 1

directory=
righe=
fileValidi=""

for i in *; do
    directory=$(pwd) # memorizzo la directory in cui mi trovo
    if test -f "$i" -a -r "$i"; then
        righe=$(wc -l < "$i") # numero righe
        if test "$righe" -eq "$2"; then
            fileValidi="$fileValidi $i"
        fi
    elif test -d "$i" -a -x "$i"; then
        # chiamata ricorsiva
        "$0" "$i" "$2"
    fi
done

echo "=========================================================="
echo "Nella directory $directory i file validi sono:$fileValidi"
echo "=========================================================="

