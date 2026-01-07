//#pragma GCC optimize ("O0")

#include <Arduino.h>
#include "yTempsCycle.h"

//
yTempsCycle TempsCycle("tps de cycle", 5, .030, 0.13);
//
void setup() {
  Serial.begin(112500);
  while (!Serial);
  Serial.println("Hello World");
}
//
void loop() {
  //
    TempsCycle.loop();
  //
  //
  // Si besoin de mesurer le temps d'exécution d'une partie de code
  //    TempsCycle.start();
  //    /.   votre code   ./
  //    TempsCycle.stop();
  // Laisser TempsCycle.loop() dans la boucle loop
  // C'est lui qui gère l'affichage
  //
  /*
    if ((millis() - memMillis) > 1000) {
      //
     TempsCycle.start();
     
      //
      Serial.println("1234567890");
      //
      TempsCycle.stop();
      //
      memMillis = millis();
      delay(20);
    }
  */
  //
  //  Si besoin de recupérer le contenu en plus de l'affichage intégré
  /*
  if (TempsCycle.newTxt)
  {
          Serial.print(TempsCycle.texte1);
          Serial.println(TempsCycle.texte2);
          TempsCycle.newTxt = false;
  }
  */
}