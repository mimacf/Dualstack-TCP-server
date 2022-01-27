1.	Zadatak
Tema: IPv6 – Fast downloader client/dual stack TCP server zasnovana arhitektura – Vižner enkripcija / dekripcija pri slanju i prijemu.
Po ugledu na programe za brzi prenos datoteka realizovati aplikacije IPv4 klijenta, IPv6 klijenta i dual stack severa (prima poruke zadate pozicije, u zadatoj dužini) koristeći TCP protokol. 
Klijent po jednoj vezi prenosi deo datoteke. Formiranjem, više istovremenih veza prenosi se cela datoteka, i ubrzava se proces prenosa. 
Server mora da simulira ograničenje brzine pomoću jedne i više veza – grafički prikazati rezultate analize u dokumentaciji. 
Server istovremeno opslužuje više klijenata.
Pri slanju podataka klijent enkriptuje sadržaj paketa Vižner šifrom. 
Pri prijemu server vrši odgovarajuću dekripciju. I obrnuto. 
Ključ odrediti samostalno. Originalna poruka koja se enkriptuje se sastoji samo od slova engleskog alfabeta.
