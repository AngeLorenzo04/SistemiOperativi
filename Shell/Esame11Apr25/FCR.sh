#File comandi ricorsivo inerente alla prova d'esame del 11 aprile 2025

cont=0

cd $1

for F in *      #F nome specificato nel testo!
do
        if test -f $F -a ! -x $F                #se e' un file e non ha diritto di esecuzione
        then
                #controlliamo che il nome del file termini per .$S
                #ATTENZIONE CHE QUESTO CONTROLLO SI PUO' FARE SOLO CON UN CASE!
                case $F in
                *.$2)
                        cont=`expr $cont + 1`   #incrementiamo il conteggio 
                        echo `pwd`/$F >> $3     #inseriamo (in append) il suo nome ASSOLUTO nel file temporaneo, passato come terzo parametro
                        ;;
                *) ;;                           #altrimenti non facciamo nulla  
                esac
        fi
done

if test $cont -ge 1
then
        #se abbiamo trovato almeno un file dobbiamo stampare il nome assoluto della dir corrente
        echo DIRECTORY TROVATA CON NOME ASSOLUTO `pwd`
else
        echo LA DIRECTORY CON NOME ASSOLUTO `pwd` NON CONTIENE ALCUN FILE NON ESEGUIBILE CON NOME CHE TERMINA PER .$2
fi

for i in *
do
        if test -d $i -a -x $i
        then
                #invocazione ricorsiva
                $0 `pwd`/$i $2 $3       #sempre passaggio del nome assoluto della dir
        fi
done

            
