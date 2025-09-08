# 📖 Guida Completa ai Comandi Shell (Unix/Linux)

Questa guida raccoglie i principali comandi di **Unix/Linux Shell** divisi per argomento, con esempi pratici ed opzioni utili. È pensata per servire come schema di studio, riferimento rapido o preparazione a test ed esami.

---

## 📂 Gestione directory

- **mkdir** → crea una o più directory
  ```bash
  mkdir nuova_dir
  mkdir -p a/b/c     # crea directory annidate
  mkdir -v nuova_dir # mostra conferma
  ```

- **rmdir** → elimina directory *solo se vuote*
  ```bash
  rmdir vecchia_dir
  rmdir -p a/b/c     # elimina catena di directory se vuote
  ```

- **cd** → cambia directory
  ```bash
  cd dir       # entro in dir
  cd           # torno alla home
  cd ~         # equivalente a cd
  cd ..        # salgo di un livello
  cd -         # torno alla directory precedente
  ```

- **pwd** → mostra percorso assoluto della directory corrente
  ```bash
  pwd
  ```

- **ls** → elenca file e directory
  ```bash
  ls        # elenco semplice
  ls -l     # dettagli (permessi, proprietario, data, dimensione)
  ls -a     # mostra file nascosti (iniziano con .)
  ls -lh    # dettagli con dimensioni leggibili (KB, MB)
  ls -R     # elenca ricorsivamente sotto-directory
  ls -t     # ordina per data di modifica
  ls -r     # ordine inverso
  ls -d */  # solo directory
  ls -F     # mostra tipo ( / = dir, * = eseguibile, @ = link )
  ```

---

## 📑 File e collegamenti

- **cp** → copia file o directory
  ```bash
  cp src dst        # copia file
  cp file1 file2 dir/ # copia più file in una directory
  cp -r dir1 dir2   # copia ricorsiva
  cp -i src dst     # chiede conferma prima di sovrascrivere
  cp -u src dst     # copia solo se più recente
  ```

- **mv** → sposta o rinomina file/dir
  ```bash
  mv file dir/      # sposta
  mv vecchio nuovo  # rinomina
  mv -i f1 f2       # conferma sovrascrittura
  ```

- **rm** → elimina file/dir
  ```bash
  rm file
  rm -i file        # chiede conferma
  rm -r dir         # elimina ricorsivamente
  rm -rf dir        # elimina forzando (attenzione!)
  ```

- **ln** → crea collegamenti
  ```bash
  ln file1 file2    # hard link
  ln -s file1 file2 # soft link (symlink)
  ```
  🔑 Differenza:
  - *Hard link*: è un riferimento diretto all’inode, resta valido anche se il file originale viene eliminato.
  - *Soft link*: è un “collegamento simbolico” al percorso, smette di funzionare se il file originale viene eliminato o spostato.

---

## 📜 Visualizzazione contenuti

- **cat** → stampa contenuto file
  ```bash
  cat file
  cat -n file   # numeri di riga
  cat -b file   # numeri solo righe non vuote
  cat -s file   # comprime righe vuote consecutive
  ```

- **more** → visualizza file a schermate (spazio = avanti, q = esci)
  ```bash
  more file
  ```

- **less** → simile a more, ma più potente (scorrimento avanti/indietro)
  ```bash
  less file
  ```

- **head** → prime righe di un file
  ```bash
  head file         # prime 10 righe
  head -n 5 file    # prime 5 righe
  ```

- **tail** → ultime righe di un file
  ```bash
  tail file         # ultime 10 righe
  tail -n 20 file   # ultime 20 righe
  tail -f log.txt   # segue aggiornamenti in tempo reale
  ```

- **wc** → conta righe, parole, caratteri
  ```bash
  wc file
  wc -l file   # conta righe
  wc -w file   # conta parole
  wc -c file   # conta byte/caratteri
  ```

- **rev** → rovescia stringhe o contenuto file
  ```bash
  echo "ciao" | rev  # output: oaic
  ```

---

## 🔎 Ricerca e ordinamento

- **grep** → cerca testo nei file
  ```bash
  grep "ciao" file
  grep -i "ciao" file   # case-insensitive
  grep -n "ciao" file   # mostra numero riga
  grep -r "ciao" dir    # ricerca ricorsiva
  grep -v "ciao" file   # esclude le righe con "ciao"
  grep -E "abc|def" f   # regex (abc o def)
  ```

- **sort** → ordina righe
  ```bash
  sort file
  sort -r file         # ordine inverso
  sort -n file         # ordinamento numerico
  sort -k2 file        # ordina per seconda colonna
  sort -t, -k2 -n file # ordina per colonna 2 (CSV numerico)
  ```

- **uniq** → rimuove duplicati consecutivi
  ```bash
  sort file | uniq
  uniq -c file   # conta occorrenze
  ```

- **find** → cerca file/directory
  ```bash
  find dir -name "nomefile"
  find . -type f -size +1M   # file > 1MB
  find / -user utente        # file di un utente
  ```

---

## 🔐 Permessi e proprietà

- **chmod** → cambia permessi
  ```bash
  chmod u+x file    # aggiunge esecuzione all’utente
  chmod g-w file    # toglie scrittura al gruppo
  chmod 755 file    # rwx r-x r-x
  chmod -R 644 dir/ # permessi ricorsivi
  ```
  🔑 Notazione numerica:
  - r = 4, w = 2, x = 1
  - 7 = rwx, 6 = rw-, 5 = r-x, 3 = -wx
  - u: user
  - g: group
  - o: others

- **chown** → cambia proprietario
  ```bash
  chown utente file
  chown utente:gruppo file
  ```

- **chgrp** → cambia gruppo proprietario
  ```bash
  chgrp gruppo file
  ```

---

## ⚙️ Shell e scripting

- **sh / bash** → eseguono script o aprono shell
  ```bash
  sh script.sh
  bash script.sh
  sh -x script.sh   # debug: mostra i comandi eseguiti
  ```

- **read** → legge input da tastiera
  ```bash
  read nome
  echo "Ciao $nome"
  ```

- **if** → condizioni
  ```bash
  if [ $a -eq $b ]; then
    echo "uguali"
  else
    echo "diversi"
  fi
  ```

- **case** → selezione multipla
  ```bash
  case $var in
    "a") echo "A";;
    "b") echo "B";;
    *) echo "Altro";;
  esac
  ```

- **test** → controlli logici e file
  ```bash
  test -e file   # esiste
  test -d dir    # è directory
  test $a -lt $b # a < b
  ```

- **shift** → sposta argomenti negli script
  ```bash
  # $1 diventa $0, $2 diventa $1, ecc.
  shift 2
  ```

- **expr** → espressioni aritmetiche
  ```bash
  expr 5 + 3   # -> 8
  ```

- **eval** → esegue stringa come comando
  ```bash
  cmd="ls -l"
  eval $cmd
  ```

- **set/export/env** → gestione variabili di ambiente
  ```bash
  set           # mostra variabili locali
  export VAR=ciao
  echo $VAR
  env           # mostra variabili d’ambiente
  ```

---

## 🖥️ Processi e login

- **ps** → mostra processi
  ```bash
  ps          # processi attuali
  ps -ef      # tutti i processi con dettagli
  ps -u nome  # processi di un utente
  ```

- **top** → monitor in tempo reale
  ```bash
  top
  htop        # versione migliorata (se installata)
  ```

- **kill** → termina processi
  ```bash
  kill PID         # invia segnale TERM
  kill -9 PID      # forza terminazione
  ```

- **jobs / fg / bg** → gestione processi in foreground e background
  ```bash
  comando &   # esegue in background
  jobs        # mostra processi sospesi
  fg %1       # porta in foreground job 1
  bg %1       # riprende in background job 1
  ```

- **who / w / id** → info su utenti
  ```bash
  who    # chi è connesso
  w      # utenti con attività
  id     # UID, GID e gruppi
  ```

- **login / logout / exit** → gestione sessione
  ```bash
  login
  logout
  exit
  ```

---

## 📌 Utilità varie

- **date** → mostra data e ora
  ```bash
  date
  date +"%d-%m-%Y %H:%M:%S"
  ```

- **man comando** → manuale del comando
  ```bash
  man ls
  ```

- **which / whereis** → posizione dei comandi
  ```bash
  which ls
  whereis bash
  ```

- **tee** → duplica output (file + schermo)
  ```bash
  echo "ciao" | tee file.txt
  ```

---

## 🔀 Redirezione e pipe

- **Redirezioni**
  ```bash
  comando < file      # input da file
  comando > file      # output in file (sovrascrive)
  comando >> file     # output in append
  comando 2> errori   # errori in file
  comando &> out.txt  # stdout + stderr insieme
  ```

- **Pipe**
  ```bash
  comando1 | comando2
  cat file | grep "test" | sort | uniq
  ```

- **Dispositivi speciali**
  ```bash
  comando > /dev/null 2>&1   # scarta output ed errori
  ```

---

# ✅ Consigli per il ripasso
- Memorizza la differenza tra **>** e **>>**.
- Ricorda i numeri di permesso (`r=4`, `w=2`, `x=1`).
- Saper usare `grep`, `find`, `sort` e `uniq` insieme è fondamentale.
- Allenati con `ps`, `kill`, `jobs`, `fg`, `bg` per i processi.
- Fai pratica con redirezioni e pipe.
