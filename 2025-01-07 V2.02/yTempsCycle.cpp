#include <Arduino.h>
#include "yTempsCycle.h"

/*
 *  Permet surveiller le temp de cycle sur la base des micro seconde
 *  hervé CHUTEAU pour le Bistromatik
 *  version 2.02 du 07/01/2026
 *
 *  Affichage sur le Terminal :
 *  Temps de cycle [9999]  moyen 0.016 ms  max 0.125  >0.030 25  >>0.120 1
 *
 *  Temps de cycle : Nom donné à l'instance
 *  (9999)         : nombre de tour de cycle (affichage limité à 9999)
 *  moyen 0.016 ms : temps moyen d'éxécution
 *  max 0.128      : temps max constaté
 *  >0.030 25      : Le premier seuil de 0.030 ms a été dépassé 25 fois
 *                   (on ne comptabilise pas ici les dépassements de second
 * seuil)
 *  >>0.120 1      : le second seuil de 0.120 ms a été dépassé qu'une fois
 *
 *  Création de Class : yTempsCycle TempsCycle("Temps de cycle", 5, 0.03, 0.13);
 *      "Temps de cycle" : Nom modifiable
 * 			5 :  Nombre de seconde entre deux affichage
 *      0.030 : Seuil 1 en ms
 *      0.120 : Seuil 2 en ms
 *
 *  Dans la boucle à surveiller : TempsCycle.loop();
 *
 *  TempsCycle.newTxt : nouveau affichage formaté
 *  TempsCycle.texte1 et TempsCycle.texte2 : affichage
 *
 *  Possibilité de mesurer le temps d'une partie de code en plaçant start stop
 *  TempsCycle.start();
 *  /.   votre code   ./
 *  TempsCycle.stop();
 *  (On laisse TempsCycle.loop(); dans la boucle loop() pour la gestion de
 * l'affichage)
 *
 *  Pour limiter l'impact sur le temps de cycle la mise en forme est decomposée
 *  opération par opération le serial.print se fait 6 carcatères pas 6
 *  caractères ce qui limite le temps max à 0,12ms (temps équivalent aux
 *  opérations les plus gourmantes de mise en forme)
 *
 *  sur MEGA2560  avec monitor_speed = 115200
 *         Par rapport à un Mega affichant "Hello World"
 *         RAM: 194 bytes sur 8192 bytes (2,4%)
 *              instance suivante 178 bytes (2,1%)
 *         Flash: 4422 bytes sur 253952 bytes (1,7%)
 *
 *  Temps moyen d'éxécution 0.02ms
 *  Temps maximum 0.12ms  (seulement 25 tours de cycle > 0,03ms)
 */
//
const int CHUNK_SIZE = 6;  // --- OPTIMISATION ---
const unsigned int maxAff_Compteur = 9999;
const float maxAff_Seuil_a = (float)9.999;
const float maxAff_Seuil_b = (float)99.99;
const float maxAff_Seuil_c = (float)999.9;
const float maxAff_Seuil_d = (float)99999;
const unsigned long ms_to_us = 1000ul;
const unsigned long sec_to_us = 1000ul * 1000ul;
const unsigned long sec_to_ms = 1000ul;
const unsigned long maxintervallePrintSec = 3600;
// const unsigned int seuil_U_INT = 65535;
const unsigned long seuil_U_LONG = 4294967295ul;

float f_tempVal;
bool yTempsCycle::printEnCours = false;

void ajout_au_texte(char* sTexte, PGM_P sAvant, float& sValeur, byte& sNbDeci);

byte calcul_nb_decimale(float& sValeur);

yTempsCycle::yTempsCycle(const char sNomCycle[15],
                         const unsigned int sIntervallePrintSec,
                         const float sSeuilms1, const float sSeuilms2) {
  //
  if (sIntervallePrintSec > (maxintervallePrintSec)) {
    intervallePrint_us = maxintervallePrintSec * 1000000ul;
    ;
  } else {
    intervallePrint_us = sIntervallePrintSec * 1000000ul;
  }
  //
  if (sSeuilms2 > maxAff_Seuil_d) {
    seuil2 = maxAff_Seuil_d * 1000.00;
    f_tempVal = maxAff_Seuil_d;
  } else {
    seuil2 = sSeuilms2 * 1000.00;
    f_tempVal = sSeuilms2;
  }
  //
  if (sSeuilms1 > maxAff_Seuil_d) {
    seuil1 = seuil2;
  } else {
    if (sSeuilms1 < f_tempVal) {
      seuil1 = sSeuilms1 * 1000.00;
    } else {
      seuil1 = f_tempVal * 1000.00;
    }
  }
  //
  memset(NomCycle, 0, 16);
  strncpy(NomCycle, sNomCycle, 15);
  //
  sp_limite =
      (unsigned long)seuil_U_LONG - intervallePrint_us - (unsigned long)10;
}

void yTempsCycle::loop() {
  //
  unsigned long sp_microsVal;
  unsigned long sp_dureeCycle;
  //
  //
  sp_microsVal = micros();
  //
  if (valCompteur < seuil_U_LONG) {
    valCompteur++;
  }

  // ******************    C A L C U L   A C T I F
  if (!CalculActif) {
    if (sp_microsVal < sp_limite) {
      CalculActif = true;
      ValTpsMax = 0ul;
      nbSeuil1 = 0;
      nbSeuil2 = 0;
      valCompteur = 0ul;
      cumulDuree = 0ul;
      memMicros = 0ul;
    }
  } else {
    // calcul max/seuils
    if (memMicros > 0ul) {
      sp_dureeCycle = sp_microsVal - memMicros;
      cumulDuree += sp_dureeCycle;
      if (!modeSartStop) {
        if (sp_dureeCycle > ValTpsMax) {
          ValTpsMax = sp_dureeCycle;
        }
        if (sp_dureeCycle > seuil2) {
          if (nbSeuil2 < maxAff_Compteur) {  // On limite à 9999
            nbSeuil2++;
          }
        } else {
          if (sp_dureeCycle > seuil1) {
            if (nbSeuil1 < maxAff_Compteur) {
              nbSeuil1++;
            }
          }
        }
      }
    }

    //
    if (sp_microsVal > sp_limite) {
      CalculActif = false;  // limite de débordement
    } else {
      memMicros = sp_microsVal;  // pour ne pas perdre une micro seconde
    }
    gestionAffichage();
  }
}
// ******************    S T A R T
void yTempsCycle::start() {
  modeSartStop = true;
  memMicrosStartStop = micros();
}
//
// ******************    S T O P
void yTempsCycle::stop() {
  unsigned long sp_microsVal;
  unsigned long sp_dureeCycle;
  //
  sp_microsVal = micros();
  //
  if (valCompteurStartStop < seuil_U_LONG) {
    valCompteurStartStop++;
  }
  //
  if ((memMicros > 0ul) && CalculActif && modeSartStop) {
    sp_dureeCycle = sp_microsVal - memMicrosStartStop;
    cumulDureeStartStop += sp_dureeCycle;
    if (modeSartStop) {
      if (sp_dureeCycle > ValTpsMax) {
        ValTpsMax = sp_dureeCycle;
      }
      if (sp_dureeCycle > seuil2) {
        if (nbSeuil2 < maxAff_Compteur) {  // On limite à 9999
          nbSeuil2++;
        }
      } else {
        if (sp_dureeCycle > seuil1) {
          if (nbSeuil1 < maxAff_Compteur) {
            nbSeuil1++;
          }
        }
      }
    }
  }
}
//
//
// ******************    G E S T I O N    A F F I C H A G E
void yTempsCycle::gestionAffichage() {
  //
  float auxFloat;
  //
  if ((cumulDuree > intervallePrint_us) && (printTour < 1)) {
    printTour = 1;
  }
  //
  switch (printTour) {
    case 1:  // ******************    Capture des valeurs

      if (!modeSartStop) {
        valCompteurPrint = valCompteur;
        cumulDureePrint = cumulDuree;
      } else {
        valCompteurPrint = valCompteurStartStop;
        cumulDureePrint = cumulDureeStartStop;
      }
      valTpsMaxPrint = ValTpsMax;
      nbSeuil1Print = nbSeuil1;
      nbSeuil2Print = nbSeuil2;
      // Reset des compteurs
      ValTpsMax = 0ul;
      nbSeuil1 = 0;
      nbSeuil2 = 0;
      valCompteur = 0ul;
      cumulDuree = 0ul;
      valCompteurStartStop = 0ul;
      cumulDureeStartStop = 0ul;
      //

      //
      printTour++;
      break;
    case 2:  // ******************    TEXTE 1 : "Nom"
      strcpy(texte1, NomCycle);
      printTour++;
      break;
    case 3:  // ******************    TEXTE 1 : nombre de tour
      if ((valCompteurPrint > maxAff_Compteur)) {
        f_tempVal = maxAff_Compteur;
      } else if (valCompteurPrint > 0) {
        f_tempVal = valCompteurPrint;
      }
      nb_decimale = 0;
      ajout_au_texte(texte1, PSTR(" ("), f_tempVal, nb_decimale);
      printTour++;
      break;
    case 4:  // ******************    TEXTE 1 : "  moyen " + Valeur
      if ((valCompteurPrint > 0)) {
        f_tempVal =
            ((float)cumulDureePrint / (float)valCompteurPrint) / (float)1000.00;
      } else {
        f_tempVal = 0;
      }
      printTour++;
      break;
    case 5:
      if (f_tempVal > maxAff_Seuil_d) {
        f_tempVal = maxAff_Seuil_d;
      }
      nb_decimale = calcul_nb_decimale(f_tempVal);
      printTour++;
      break;
    case 6:
      if (valCompteurPrint > 0) {
        ajout_au_texte(texte1, PSTR(")  moyen "), f_tempVal, nb_decimale);
        printTour++;
      } else {
        strcat_P(texte1, PSTR(")"));
        texte2[0] = '\0';
        printTour = 15;
      }
      break;
    case 7:  // ******************    TEXTE 2 : "  max " + Valeur
      f_tempVal = (float)valTpsMaxPrint / (float)1000.00;
      nb_decimale = calcul_nb_decimale(f_tempVal);
      texte2[0] = '\0';
      printTour++;
      break;
    case 8:
      ajout_au_texte(texte2, PSTR(" ms  max "), f_tempVal, nb_decimale);
      printTour++;
      break;
    case 9:
      f_tempVal = (float)seuil1 / (float)1000;
      nb_decimale = calcul_nb_decimale(f_tempVal);
      printTour++;
      break;
    case 10:
      ajout_au_texte(texte2, PSTR("  >"), f_tempVal, nb_decimale);
      printTour++;
      break;
    case 11:
      nb_decimale = 0;
      auxFloat = (float)nbSeuil1Print;
      ajout_au_texte(texte2, PSTR(" "), auxFloat, nb_decimale);
      printTour++;
      break;
    case 12:
      f_tempVal = (float)seuil2 / (float)1000;
      nb_decimale = calcul_nb_decimale(f_tempVal);
      printTour++;
      break;
    case 13:
      ajout_au_texte(texte2, PSTR("  >>"), f_tempVal, nb_decimale);
      printTour++;
      break;
    case 14:
      // dtostrf(nbSeuil2Print, 1, 0, conversionBuf);
      nb_decimale = 0;
      auxFloat = (float)nbSeuil2Print;
      ajout_au_texte(texte2, PSTR(" "), auxFloat, nb_decimale);
      printTour++;
      break;
      //
    case 15:  // ****************    GESTION ENVOI
      newTxt = true;
      ptrPrintCurrent = texte1;
      lenPrintRemaining = strlen(texte1);
      etapePrintString = 0;
      printTour++;
      break;

    case 16:  // Mutex
      if (!printEnCours) {
        printEnCours = true;
        printTour++;
      }
      break;

    case 17:  // Envoi Série
      if (Serial.availableForWrite() >= CHUNK_SIZE) {
        int aEnvoyer;
        if (lenPrintRemaining > CHUNK_SIZE) {
          aEnvoyer = CHUNK_SIZE;
        } else {
          aEnvoyer = lenPrintRemaining;
        }

        if (aEnvoyer > 0) {
          Serial.write(ptrPrintCurrent, aEnvoyer);
          ptrPrintCurrent += aEnvoyer;
          lenPrintRemaining -= aEnvoyer;
        }
        if (lenPrintRemaining <= 0) {
          if (etapePrintString == 0) {
            etapePrintString = 1;
            ptrPrintCurrent = texte2;
            lenPrintRemaining = strlen(texte2);
          } else {
            Serial.println();
            printEnCours = false;
            newTxt = false;
            printTour = 0;
          }
        }
      }
      break;

    default:
      printTour = 0;
      printEnCours = false;
      break;
  }
}

void ajout_au_texte(char* sTexte, PGM_P sAvant, float& sValeur, byte& sNbDeci) {
  char sp_conversionBuf[6];
  strcat_P(sTexte, sAvant);
  dtostrf(sValeur, 1, sNbDeci, sp_conversionBuf);
  strcat(sTexte, sp_conversionBuf);
}

byte calcul_nb_decimale(float& sValeur) {
  byte val_return = 0;
  if (sValeur > maxAff_Seuil_c) {
    val_return = 0;
  } else {
    if (sValeur > maxAff_Seuil_b) {
      val_return = 1;
    } else {
      if (sValeur > maxAff_Seuil_a) {
        val_return = 2;
      } else {
        val_return = 3;
      }
    }
  }
  return val_return;
};
