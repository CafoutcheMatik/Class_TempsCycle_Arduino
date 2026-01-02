# Class_TempsCycle_Arduino

Permet surveiller le temp de cycle sur la base des micro seconde  
hervé CHUTEAU pour le Bistromatik  
version 2.01 du 02/01/2026  

Affichage sur le Terminal :  
Temps de cycle (ms) moyen 0.02  max 0.12  >0.03 25  >>0.12 0

Création de Class : yTempsCycle TempsCycle("Temps de cycle", 5, 0.03, 0.12)  
"Temps de cycle" : Nom modifiable  
			  5  :  Nombre de seconde entre deux affichage  
			0.03 : Seuil 1 en ms  
            0.12 : Seuil 2 en ms  

Dans la boucle à surveiller : TempsCycle.loop();  

TempsCycle.newTxt : nouveau affichage formaté  
TempsCycle.texte1 et TempsCycle.texte2 : affichage  

Pour limiter l'impact sur le temps de cycle la mise en forme est decomposée opération par opération le serial.print se fait 6 carcatères pas 6
caractères ce qui limite le temps max à 0,12ms (temps équivalent aux opérations les plus gourmantes de mise en forme)

sur MEGA2560  avec monitor_speed = 115200  
       Par rapport à un Mega affichant "Hello World"  
       RAM: 188 bytes sur 8192 bytes (2,3%)  
             instance suivante 165 bytes (2%)  
       Flash: 4240 bytes sur 253952 bytes (1,7%)  

Temps moyen d'éxécution 0.02ms  
Temps maximum 0.12ms  (seulement 25 tours de cycle > 0,03ms)  

