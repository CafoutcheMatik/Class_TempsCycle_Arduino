// #pragma GCC optimize ("O0")

#include <Arduino.h>
#include "utilitaires/yTempsCycle.h"

/* ********************************
 *     P A R A M E T R A G E      */
char Param_NumVersion[] = "v0.01 16/11/2025";
// C O N S T A N T E

yTempsCycle TempsCycle("Temps de cycle", 4, 0.08, 0.15);
//

int tour;

void setup()
{

        Serial.begin(115200); // Terminal
        //
}
void loop()
{
        TempsCycle.loop();

        // Si besoin de recupérer le contenu
        /*
        if (TempsCycle.newTxt)
        {
                Serial.print(TempsCycle.texte1);
                Serial.println(TempsCycle.texte2);
                TempsCycle.newTxt = false;
        }
        */
}