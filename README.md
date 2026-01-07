# Class_TempsCycle_Arduino

   Permet surveiller le temp de cycle sur la base des micro seconde  
   hervé CHUTEAU pour le Bistromatik  
   version 2.02 du 07/01/2026  
 
   Affichage sur le Terminal :  
   Temps de cycle [9999]  moyen 0.016 ms  max 0.125  >0.030 25  >>0.120 1  
 
   Temps de cycle : Nom donné à l'instance  
   (9999)         : nombre de tour de cycle (affichage limité à 9999)  
   moyen 0.016 ms : temps moyen d'éxécution  
   max 0.128      : temps max constaté  
   \>0.030 25      : Le premier seuil de 0.030 ms a été dépassé 25 fois  
           (on ne comptabilise pas ici les dépassements de second seuil)  
   \>>0.120 1      : le second seuil de 0.120 ms a été dépassé qu'une fois  
 
   Création de Class : yTempsCycle TempsCycle("Temps de cycle", 5, 0.03, 0.13);  
       "Temps de cycle" : Nom modifiable  
  	   5 :  Nombre de seconde entre deux affichage  
       0.030 : Seuil 1 en ms  
       0.120 : Seuil 2 en ms  
 
   Dans la boucle à surveiller : TempsCycle.loop();  
 
   TempsCycle.newTxt : nouveau affichage formaté  
   TempsCycle.texte1 et TempsCycle.texte2 : affichage  
  
   Possibilité de mesurer le temps d'une partie de code en plaçant start stop  
   TempsCycle.start();  
   /.   votre code   ./  
   TempsCycle.stop();  
   (On laisse TempsCycle.loop(); dans la boucle loop() pour la gestion de l'affichage)  
 
   Pour limiter l'impact sur le temps de cycle la mise en forme est decomposée  
   opération par opération le serial.print se fait 6 carcatères pas 6  
   caractères ce qui limite le temps max à 0,12ms (temps équivalent aux  
   opérations les plus gourmantes de mise en forme)  
 
   sur MEGA2560  avec monitor_speed = 115200  
          Par rapport à un Mega affichant "Hello World"  
          RAM: 194 bytes sur 8192 bytes (2,4%)  
               instance suivante 178 bytes (2,1%)  
          Flash: 4422 bytes sur 253952 bytes (1,7%)  
 
   Temps moyen d'éxécution 0.02ms  
   Temps maximum 0.12ms  (seulement 25 tours de cycle > 0,03ms)

