# SO-CypherSystem
Practica-Sistemes-Operatius

El sistema Cypher està format per unitats replicades de Trinity que permeten
interconnectar-se entre elles per la transmissió de text o àudio. Cal dissenyar el sistema
per a que sigui escalable.
El procés Trinity es manipularà utilitzant un conjunt comandes esteses del sistema, que
han de ser case insensitive. Per executar-se tindrà un fitxer de configuració associat que
contindrà una sèrie de paràmetres importants per la seva execució i funcions.

Per poder intuir una primera execució del sistema la Figura 2 ens mostra un exemple:
Figura 2. Execució d’un procés McGruder.
Figura 2. Execució de Trinity.
montserrat$ Trinity Config.dat
Starting Trinity...
$Persefone: SHOW CONNECTIONS
Testing...
3 connections available
8011
8013
8014
$Persefone: CONNECT 8012
Connecting...
8012 connected: Niobe
$Persefone: say Niobe “Hello Niobe”
$Persefone: SHOW AUDIOS Niobe
[Niobe] Audios
JAGGER.MP3
ORBISON.MP3
$Persefone: DOWNLOAD Niobe JAGGER.mp3
[Niobe] JAGGER.MP3 downloaded

Disconnecting Trinity...
montserrat$
