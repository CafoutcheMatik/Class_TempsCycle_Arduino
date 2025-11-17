# Class_TempsCycle_Arduino

Permet surveiller le temp de cycle sur la base des micro seconde
hervé CHUTEAU pour le Bistromatik
version 1.01 du 17/11/2025

Affichage sur le Terminal : Temps de cycle   moyen 0.02   max 0.15   >0.08 9   >>0.15 1

Création de Class : yTempsCycle TempsCycle("Temps de cycle", 4, 0.08, 0.15);
 	"Temps de cycle" : Nom modifiable
		4  :  Nombre de seconde entre deux affichage
		0.08 : Seuil 1 en ms	
 	0.15 : Seuil 2 en ms
 
Dans la boucle à surveiller : TempsCycle.loop();

TempsCycle.newTxt : nouveau affichage formaté
TempsCycle.texte1 et TempsCycle.texte2 : affichage 

Pour limiter l'impact sur le temps de cycle la mise en forme est decomposée opération par opération
le serial.print se fait 10 carcatères pas 10 caractères ce qui limite le temps max à 0,15ms
(temps équivalent aux opérations les plus gourmantes de mise en forme)

sur MEGA2560  avec monitor_speed = 115200
       Par rapport à un Mega affichant "Hello World"
       RAM: 645 bytes from 8192 bytes
       Flash: 4372 bytes from 253952 bytes
Temps mouyen d'éxécution 0.02ms
Temps maximum 0.12ms  (seulement 10 tours de cycle > 8ms)

