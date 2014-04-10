Program zosta³ napisany w C i u¿ywa biblioteki GTK.
Projekt programu jest to plik w formacie CodeBlocks.
Program kompiluje siê u¿ywaj¹c nastêpuj¹cych flag kompilatora:
-Wall -Werror -lm -std=c99 `pkg-config --cflags gtk+-2.0`
Oraz flag linkera:
`pkg-config --libs gtk+-2.0`

Format wykorzystywanych plików:
S³ownik: plik tekstowy zapisany w kodowaniu CP1250 (wa¿ne) zawieraj¹cy s³owa oddzielone znakami nowej linii.
Krzy¿ówka: plik tekstowy zapisany w kodowaniu CP1250 sk³ada siê z nag³ówka i zawartoœci krzy¿ówki.
Nag³ówek to ROZMIAR_X ROZMIAR_Y.
Zawartoœæ krzy¿ówki to:
	# - czarne pole
	_ - bia³e pole
	litery
Przyk³adowa krzy¿ówka:
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

Krzy¿ówka mo¿e byæ te¿ zape³niona jakimiœ literami.
Przyk³adowe s³owniki i krzy¿ówki znajduj¹ siê odpowiednio w folderach dict i board.

Interfejs programu jest podzielony na trzy czêœci:
*Menu
*Obszar wyœwietlania krzy¿ówki
*Obszar z podpowiedziami do wype³nianego pola krzy¿ówki

W menu mo¿na stworzyæ now¹ krzy¿ówkê, otworzyæ istniej¹c¹ lub zapisaæ otwart¹.

Podczas tworzenia nowej krzy¿ówki nale¿y wskazaæ jej rozmiar i podaæ œcie¿kê do pliku
ze s³ownikiem, który ma byæ u¿ywany do wype³niania krzy¿ówki.

Przy otwieraniu krzy¿ówki najpierw nale¿y wskazaæ plik z krzy¿ówk¹, nastêpnie plik ze s³ownikiem.

W menu zawarte s¹ równie¿ funkcje umo¿liwiaj¹ce edycjê krzy¿ówki.
Structure mode pozwala na malowanie kursorem myszy czarnych i bia³ych pól w obszarze wyœwietlania
krzy¿ówki.
Fill mode s³u¿y do wype³niania krzy¿ówki s³owami. Mo¿na klikaæ myszk¹ na obszarze wyœwietlania i zostan¹
zaznaczone pola do wype³nienia, a w obszarze z podpowiedziami poka¿¹ siê s³owa, którymi mo¿na zape³niæ
zaznaczone pole wciskaj¹c przycisk Insert.
Jest te¿ funkcja solve, która dzia³a tylko wtedy gdy krzy¿ówka jest zape³niona wy³¹cznie s³owami nale¿¹cymi
do aktualnie u¿ywanego s³ownika. Funkcja solve wykorzystuje algorytm o bardzo du¿ej z³o¿onoœci obliczeniowej
i mo¿e bardzo d³ugo siê wykonywaæ. Interfejs programu siê zawiesza do póki funckja solve nie zakoñczy dzia³ania.
Wiêkszoœæ przyk³adowych krzy¿ówek daje siê zape³niæ jedynie s³owami ze s³ownika eng.lst.

Modu³y:
S¹ dwa modu³y - GUI i krzy¿ówka.
GUI jest zawarty w pliku main.c i s³u¿y do sterowania modu³em 'krzy¿ówka' oraz wyœwietla stan modu³u krzy¿ówka.
Modu³ krzy¿ówka jest podzielony na:
board, solver i words.
board - definicja struktury danych krzy¿ówka oraz operacje do modyfikacji jej stanu
solver - pozwala na zape³nienie krzy¿ówki s³owami
words - dostarcza s³ownik wyko¿ystywany przez solver i GUI
