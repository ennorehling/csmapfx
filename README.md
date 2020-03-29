Wenn er sich das denn antun will :wink:
er braucht das Fox Toolkit und Ruby.
die Versionen weiss ich nicht, aber ich nehme an, dass es sowas wie Ruby 1.9 war.
Xolgrimheute um 20:14 Uhr
thx
Einschränkungen/Vorgaben
oder kann das veröffentlicht werden?
Phygonheute um 20:15 Uhr
erstmal nur an Enno

2020-03-29 Enno:
Habe ein Release build mit VS2013 gemacht mit Ruby 1.9.1, 
Fox 1.6.57 und boost 1.66

Der gemeinste Trick war, dass der Code für boost::signals im source
tree mit drin ist, und nicht gelinkt werden darf.

Fox habe ich aus Sourcen kompiliert, mit dem selben Compiler und gleicher CRT (Multihreaded).

Kein Debug Build bisher (runtime libraries sind Dreck).

Kein libpng, es wird 1.2 benötigt, und ich habe keine binaries dafür gefunden.

Kein Support für bzip reporte, weil ich kein libbz2 zur Hand hatte. Suchen im Source 
nach HAVE_BZ2LIB_H um das zu reaktiveieren.

Mein ruby ist aus der Zeit vom Wechsel zwischen 1.9.1 nach 1.9.2, da
fehlten ein paar Symbole, deshalb mit TODO_RUBY einzelne Teile entfernt.

Weil mein ruby nicht fuer diesen Compiler war, war das config.h unpassend,
und ich habe mir ein eigenes von Hadn gemacht und ins Projekt getan.
