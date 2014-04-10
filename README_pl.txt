Program zosta� napisany w C i u�ywa biblioteki GTK.
Projekt programu jest to plik w formacie CodeBlocks.
Program kompiluje si� u�ywaj�c nast�puj�cych flag kompilatora:
-Wall -Werror -lm -std=c99 `pkg-config --cflags gtk+-2.0`
Oraz flag linkera:
`pkg-config --libs gtk+-2.0`

Format wykorzystywanych plik�w:
S�ownik: plik tekstowy zapisany w kodowaniu CP1250 (wa�ne) zawieraj�cy s�owa oddzielone znakami nowej linii.
Krzy��wka: plik tekstowy zapisany w kodowaniu CP1250 sk�ada si� z nag��wka i zawarto�ci krzy��wki.
Nag��wek to ROZMIAR_X ROZMIAR_Y.
Zawarto�� krzy��wki to:
	# - czarne pole
	_ - bia�e pole
	litery
Przyk�adowa krzy��wka:
11 11
######_####
######_####
####_____##
####_#_#_##
##_________
##_#_#_#_##
_________##
##_#_#_####
##_____####
####_######
####_######

Krzy��wka mo�e by� te� zape�niona jakimi� literami.
Przyk�adowe s�owniki i krzy��wki znajduj� si� odpowiednio w folderach dict i board.

Interfejs programu jest podzielony na trzy cz�ci:
*Menu
*Obszar wy�wietlania krzy��wki
*Obszar z podpowiedziami do wype�nianego pola krzy��wki

W menu mo�na stworzy� now� krzy��wk�, otworzy� istniej�c� lub zapisa� otwart�.

Podczas tworzenia nowej krzy��wki nale�y wskaza� jej rozmiar i poda� �cie�k� do pliku
ze s�ownikiem, kt�ry ma by� u�ywany do wype�niania krzy��wki.

Przy otwieraniu krzy��wki najpierw nale�y wskaza� plik z krzy��wk�, nast�pnie plik ze s�ownikiem.

W menu zawarte s� r�wnie� funkcje umo�liwiaj�ce edycj� krzy��wki.
Structure mode pozwala na malowanie kursorem myszy czarnych i bia�ych p�l w obszarze wy�wietlania
krzy��wki.
Fill mode s�u�y do wype�niania krzy��wki s�owami. Mo�na klika� myszk� na obszarze wy�wietlania i zostan�
zaznaczone pola do wype�nienia, a w obszarze z podpowiedziami poka�� si� s�owa, kt�rymi mo�na zape�ni�
zaznaczone pole wciskaj�c przycisk Insert.
Jest te� funkcja solve, kt�ra dzia�a tylko wtedy gdy krzy��wka jest zape�niona wy��cznie s�owami nale��cymi
do aktualnie u�ywanego s�ownika. Funkcja solve wykorzystuje algorytm o bardzo du�ej z�o�ono�ci obliczeniowej
i mo�e bardzo d�ugo si� wykonywa�. Interfejs programu si� zawiesza do p�ki funckja solve nie zako�czy dzia�ania.
Wi�kszo�� przyk�adowych krzy��wek daje si� zape�ni� jedynie s�owami ze s�ownika eng.lst.

Modu�y:
S� dwa modu�y - GUI i krzy��wka.
GUI jest zawarty w pliku main.c i s�u�y do sterowania modu�em 'krzy��wka' oraz wy�wietla stan modu�u krzy��wka.
Modu� krzy��wka jest podzielony na:
board, solver i words.
board - definicja struktury danych krzy��wka oraz operacje do modyfikacji jej stanu
solver - pozwala na zape�nienie krzy��wki s�owami
words - dostarcza s�ownik wyko�ystywany przez solver i GUI
