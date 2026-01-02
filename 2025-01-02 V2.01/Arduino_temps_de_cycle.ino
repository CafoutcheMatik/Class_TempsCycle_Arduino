// #pragma GCC optimize ("O0")

#include <Arduino.h>
#include "yTempsCycle.h"
//
yTempsCycle TempsCycle("Temps de cycle", 5, .03, 0.12);

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
  // delay (10);
  //
  // Si besoin de recupérer le contenu en plus de l'affichage intégré
  /*
  if (TempsCycle.newTxt)
  {
          Serial.print(TempsCycle.texte1);
          Serial.println(TempsCycle.texte2);
          TempsCycle.newTxt = false;
  }
  */
}
