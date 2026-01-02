#include <Arduino.h>
#include "yTempsCycle.h"

/*
 *  Permet surveiller le temp de cycle sur la base des micro seconde
 *  hervé CHUTEAU pour le Bistromatik
 *  version 2.01 du 02/01/2026
 *
 *  Affichage sur le Terminal :
 *  Temps de cycle (ms) moyen 0.02  max 0.12  >0.03 25  >>0.12 0
 *
 *  Création de Class : yTempsCycle TempsCycle("Temps de cycle", 5, 0.03, 0.12);
 *        	"Temps de cycle" : Nom modifiable
 * 			5  :  Nombre de seconde entre deux affichage
 * 			0.03 : Seuil 1 en ms
 *      0.12 : Seuil 2 en ms
 *
 *  Dans la boucle à surveiller : TempsCycle.loop();
 *
 *  TempsCycle.newTxt : nouveau affichage formaté
 *  TempsCycle.texte1 et TempsCycle.texte2 : affichage
 *
 *  Pour limiter l'impact sur le temps de cycle la mise en forme est decomposée
 * opération par opération le serial.print se fait 6 carcatères pas 6
 * caractères ce qui limite le temps max à 0,12ms (temps équivalent aux
 * opérations les plus gourmantes de mise en forme)
 *
 *  sur MEGA2560  avec monitor_speed = 115200
 *         Par rapport à un Mega affichant "Hello World"
 *         RAM: 188 bytes sur 8192 bytes (2,3%)
 *              instance suivante 165 bytes (2%)
 *         Flash: 4240 bytes sur 253952 bytes (1,7%)
 *
 *  Temps moyen d'éxécution 0.02ms
 *  Temps maximum 0.12ms  (seulement 25 tours de cycle > 0,03ms)
 */
float f_tempVal;
bool yTempsCycle::printEnCours = false;

yTempsCycle::yTempsCycle(const char sNomCycle[15],
                         const unsigned int sIntervallePrintSec,
                         const float sSeuilms1, const float sSeuilms2) {
  intervallePrintSec = sIntervallePrintSec;
  if (sSeuilms2 > maxSeuil) {
    seuil2 = maxSeuil * 1000.00;
    f_tempVal = maxSeuil;
  } else {
    seuil2 = sSeuilms2 * 1000.00;
    f_tempVal = sSeuilms2;
  }
  //
  if (sSeuilms1 > maxSeuil) {
    seuil1 = seuil2;
  } else {
    if (sSeuilms1 < f_tempVal) {
      seuil1 = sSeuilms1 * 1000.00;
    } else {
      seuil1 = f_tempVal * 1000.00;
    }
  }
  strcpy(NomCycle, sNomCycle);
}

void ajout_au_texte(char* sTexte, PGM_P sAvant, float sValeur, byte sNbDeci);

void yTempsCycle::loop() {
  //
  unsigned long sp_microsVal;
  unsigned long sp_limite;
  unsigned long sp_dureeCycle;
  //
  sp_microsVal = micros();
  sp_limite = (unsigned long)seuil_U_LONG -
              ((unsigned long)intervallePrintSec * 1000000ul) -
              (unsigned long)10;
  //
  if (compteur < seuil_U_LONG) {
    compteur++;
  }

  // ******************    C A L C U L   A C T I F
  if (!CalculActif) {
    if (sp_microsVal < sp_limite) {
      CalculActif = true;
      max = 0ul;
      nbSeuil1 = 0;
      nbSeuil2 = 0;
      compteur = 0ul;
      cumulDuree = 0ul;
      memMicros = 0ul;
    }
  } else {
    // calcul max/seuils
    if (memMicros > 0ul) {
      sp_dureeCycle = sp_microsVal - memMicros;
      cumulDuree += sp_dureeCycle;
      if (sp_dureeCycle > max) {
        if (sp_dureeCycle <= seuil_U_INT) {
          max = sp_dureeCycle;
        } else {
          max = seuil_U_INT;
        }
      }
      if (sp_dureeCycle > seuil2) {
        if (nbSeuil2 < maxAffCompteur) {  // On limite à 9999
          nbSeuil2++;
        }
      } else {
        if (sp_dureeCycle > seuil1) {
          if (nbSeuil1 < maxAffCompteur) {
            nbSeuil1++;
          }
        }
      }
    }
    //
    if ((cumulDuree > ((unsigned long)intervallePrintSec * 1000000ul)) &&
        (printTour < 1)) {
      printTour = 1;
    }
    //
    switch (printTour) {
      case 1:  // ******************    Capture des valeurs
        compteurPrint = compteur;
        cumulDureePrint = cumulDuree;
        maxPrint = max;
        nbSeuil1Print = nbSeuil1;
        nbSeuil2Print = nbSeuil2;
        // Reset des compteurs
        max = 0ul;
        nbSeuil1 = 0;
        nbSeuil2 = 0;
        compteur = 0ul;
        cumulDuree = 0ul;
        //
        printTour++;
        break;
      case 2:  // ******************    TEXTE 1 : "Nom" 
        strcpy(texte1, NomCycle);
        printTour++;
        break;
      case 3:  // ******************    TEXTE 1 : "  moyen " + Valeur
        if ((compteurPrint > 0)) {
          f_tempVal = ((float)cumulDureePrint / (float)compteurPrint) / 1000.00;
        } else {
          f_tempVal = 0;
        }
        if (f_tempVal > maxAffSeuil) {
          f_tempVal = maxAffSeuil;
        }
        printTour++;
        break;
      case 4:
        ajout_au_texte(texte1, PSTR(" (ms) moyen "), f_tempVal, 2);
        printTour++;
        break;
      case 5:// ******************    TEXTE 2 : "  max " + Valeur
        f_tempVal = (float)maxPrint / 1000.00;
        texte2[0] = '\0';
        printTour++;
        break;
      case 6:
        ajout_au_texte(texte2, PSTR("  max "), f_tempVal, 2);  
        printTour++;
        break;
      case 7:
        f_tempVal = (float)seuil1 / (float)1000;
        printTour++;
        break;
      case 8:
        ajout_au_texte(texte2, PSTR("  >"), f_tempVal, 2);
        printTour++;
        break;
      case 9:
        ajout_au_texte(texte2, PSTR(" "), (float)nbSeuil1Print, 0);
        printTour++;
        break;
      case 10:
        f_tempVal = (float)seuil2 / (float)1000;
        printTour++;
        break;
      case 11:
        ajout_au_texte(texte2, PSTR("  >>"), f_tempVal, 2);
        printTour++;
        break;
      case 12:
        // dtostrf(nbSeuil2Print, 1, 0, conversionBuf);
        ajout_au_texte(texte2, PSTR(" "), (float)nbSeuil2Print, 0);
        printTour++;
        break;
      case 13:  // ****************    GESTION ENVOI
        newTxt = true;
        ptrPrintCurrent = texte1;
        lenPrintRemaining = strlen(texte1);
        etapePrintString = 0;
        printTour++;
        break;

      case 14:  // Mutex
        if (!printEnCours) {
          printEnCours = true;
          printTour++;
        }
        break;

      case 15:  // Envoi Série
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
  //
  if (sp_microsVal > sp_limite) {
    CalculActif = false;  // limite de débordement
  } else {
    memMicros = sp_microsVal;  // pour ne pas perdre une micro seconde
  }
}
void ajout_au_texte(char* sTexte, PGM_P sAvant, float sValeur, byte sNbDeci) {
  char sp_conversionBuf[6];
  strcat_P(sTexte, sAvant);
  dtostrf(sValeur, 1, sNbDeci, sp_conversionBuf);
  strcat(sTexte, sp_conversionBuf);
}
