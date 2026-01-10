#Prova d'esame 7 Nov 25

uso(){
    echo "Usage: <Dir> <char1> <char2> ... <charn>"
    echo "  <Dir> : percorso relativo di una rirectory esistente"
    echo "  <charn> : singolo carattere"
    exit 1
}

#Controllo se i parametri sono validi
if [ "$#" -lt 2 ]; 
then 
    uso
fi

# Memorizzo il primo parametro
dir=$1
shift

if [ ! -d "$dir" ];
then 
 uso
fi

# Memorizzo i caratteri
caratteri=

for char
do 
    case $char in
    ?) 
        caratteri="$caratteri $char"
        ;;
    *)
        uso
        ;;
    esac
done

CONT=0

for subdir in $(find "$dir" -type d)
do
    DIR_OK=0
    for f in $(find "$subdir" -maxdepth 1 -type f -readable)
    do 
        FILE_OK=1
        for C in $caratteri
        do 
            grep -q "$C" "$F"
            RV=$?
            if [ $RV -eq 1 ];
            then
                FILE_OK=0
            fi
        done
        if [ "$FILE_OK" -eq 1 ]; 
        then
            DIR_OK=1
            CONT=$(($CONT + 1))

        fi
    done

    if [ "$DIR_OK" -eq 1 ];
    then
        echo + $subdir
    else
        echo - $subdir
    fi

done
echo "Total files: $CONT"