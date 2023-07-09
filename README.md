## Kurze Geschichte

Dieser Source wurde mir Anfang 2020 �bergeben mit den Worten:
	Wenn er sich das denn antun will :wink:
	er braucht das Fox Toolkit und Ruby.
	die Versionen weiss ich nicht, aber ich nehme an, dass es sowas wie Ruby 1.9 war.

Die Abh�ngigkeit von Ruby habe ich ersetzt durch Javascript (via TinyJS), 
es wurde praktisch nur f�r den Mini-Rechner benutzt.

Die Abh�ngigkeit von Boost habe ich ersetzt, indem ich die signals entfernt, 
und smart pointer durch die von C++ ersetzt habe.

Urspr�nglich wurde eine libpng 1.2 benutzt, die aber schon lange nicht mehr
aktuell ist, ich habe daher den Code auf 1.6 umgeschrieben.

Die Unterst�tzung f�r bzip Reporte habe ich entfernt, weil ich kein libbz2 zur
Hand hatte, und das ohnehin ein kaum genutztes Feature ist.

Danach habe ich CMake f�r den Linux Build konfiguriert, unter Linux gibt es
fertige Pakete f�r fox-1.6.

Unter Windows ist es komplizierter, da funktioniert die Kombination aus CMake 
und vcpkg nicht, weil vcpkg (noch) kein Paket f�r Fox hat, und es entsprechend
kein find_package(fox) gibt. Daher hat das CMake File eine WIN32 Sektion, die
auf Grund der Environment-Variablen FOX_ROOT nach dem lib und include 
Verzeichnis sucht.

Da auch das nicht immer klappt, gibt es noch ein CSMapFX.sln Projekt, mit dem
VS2019 ein .EXE bauen kann.

Ich habe vor�bergehend ein Visual Studio Projekt aufgesetzt, Fox habe ich aus
den Sourcen selber kompiliert, mit dem selben Compiler und gleicher CRT 
(Multihreaded), und verweise direkt auf das Verzeichnis, in das ich es lokal
installiert habe. Das sollte bei Gelegenheit besser gemacht werden.

### Minor UI Bugs

* ctrl+n/ctrl+p macht die befehle zum aktiven widget, nicht die regionsliste, warum?
* popups haben keine cursor-navigation
* make context menu key open popups
* faction details: show alliance status
* regionlist: Geb�ude/Schiffe nodes l�schen die detail-anzeige nicht (nur Partei-Gruppierung tut das)
* details: selektion bewegt sich nicht mit cursor.
