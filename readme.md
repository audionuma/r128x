**This project is now frozen as I'm not able to keep it alive on recent apple computers. You can go to https://github.com/ShikiSuen/r128x for an updated UI**

**Ce projet est désormais figé car je ne peux plus développer sur des machines Apple récentes. Vous pouvez tester https://github.com/ShikiSuen/r128x pour une interface utilisateur mise à jour**

# r128x, a tool for loudness measurement of files on Mac OSX Intel. #
# with gui now !
#
![r128x-gui](capture01.png)

r128x is released under a GPL license.

## Binaries ##
 
v 0.3 2020-04-11 binaries can be found in the _release_ tab.
(voir version Française plus bas)

## Description ##
r128x is a tool for measuring Integrated Loudness, Loudness range and Maximum True Peak of audio files on the Mac OS X system with an Intel processor.

It uses libebur128 (https://github.com/jiixyj/libebur128) for the Integrated Loudness and Loudness Range measures.

It uses CoreAudio's AudioConverter service to oversample audio data and obtain the Maximum True Peak.

It uses CoreAudio ExtAudioFile's service to read the samples from an audio file.

You can build two different binaries from the sources : a command line utility (r128x-cli) and a graphical interface utility (r128x-gui).

## Installation ##
You can build the executables from the Xcode project, or use the provided executables.

Just drag the executable file to your hard drive.
/usr/local/bin/ should be a nice place to put the command line binary r128x-cli if you want it to be in your $PATH.

## Usage ##
r128x-cli /some/audio/file.wav
Will print out the file name, the Integrated Loudness in LUFS, the Loudness range in LU, the Maximum True Peak in dBTP.
Will print an error message in case of unsupported file or if an error occurs during processing.

r128x-gui supports drag and drop of audio files. You can also use the file selector.

## Issues ##
Channel mapping is static, using the default libebur128 channel mapping :
L / R / C / LFE / Ls / Rs.
You will have incorrect results if your file's channels mapping is different.
As r128x uses CoreAudio to read audio files, only file formats/codecs supported by CoreAudio are supported by r128x.

# r128x, une application pour les mesures de Loudness de fichiers audio sur Mac OS X Intel. #

r128x est distribué sous licence GPL.

v 0.3 11/04/2020
## Description ##
r128x est une application pour la mesure du Loudness Intégré, du Loudness range et du Maximum True Peak de fichiers audio sur Mac OS X avec processeur Intel.

Elle utilise la bibliothèque libebur128 (https://github.com/jiixyj/libebur128) pour les mesures de Loudness Intégré et de Loudness Range.

Elle utilise le service AudioConverter de CoreAudio pour le sur-échantillonage nécessaire à la mesure du Maximum True Peak.

Elle utilise le service ExtAudioFile de CoreAudio pour la lecture des échantillons audio d'un fichier.

Vous pouvez compiler deux exécutables à partir des sources : un utilitaire en ligne de commande (r128x-cli) et une application avec interface graphique (r128x-gui).

## Installation ##
Vous pouvez compiler les exécutables depuis le projet Xcode, ou utiliser les exécutables fournis.

Il suffit de faire glisser les exécutables sur votre disque dur.

/usr/local/bin/ peut être un bon choix de répertoire de destination pour r128x-cli si vous voulez que l'exécutable soit dans votre $PATH.

## Utilisation ##
r128x-cli /chemin/vers/unfichier.wav
affichera le nom du fichier, son Loudness Intégré en LUFS, son Loudness Range en LU et son Maximum True Peak en dBTP.
affichera un message d'erreur si le fichier n'est pas pris en charge ou qu'une erreur s'est produite durant l'analyse.

r128x-gui accepte le glisser-déposer de fichiers audio. Vous pouvez également utiliser le sélecteur de fichiers.

## Problèmes ##
La configuration des canaux audio est statique et utilise la configuration par défaut de libebur128 : L / R / C / LFE / Ls / Rs.

Les résultats seront incorrects si la configuration des canaux audio de votre fichier est différente.

Étant donné que r128x utilise CoreAudio pour lire les fichiers audio, seuls les formats/codecs pris en charge par CoreAudio sont pris en charge par r128x.
