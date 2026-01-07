#ifndef xTEMPSCYCLE_H
#define xTEMPSCYCLE_H

#include <Arduino.h>  // Nécessaire pour byte, Serial, etc.

class yTempsCycle {
 private:
  unsigned long intervallePrint_us;
  unsigned long seuil1;
  unsigned long seuil2;
  //
  unsigned long valCompteur;
  unsigned long valCompteurStartStop;
  unsigned long ValTpsMax;
  unsigned int nbSeuil1;
  unsigned int nbSeuil2;

  bool modeSartStop = false;
  //
  bool CalculActif = false;
  //
  // pour calcul durée
  unsigned long memMicros, cumulDuree;
  // pour calcul durée Start Stop
  unsigned long memMicrosStartStop, cumulDureeStartStop;

  // Variables figées pour l'impression
  unsigned long valCompteurPrint;
  unsigned long cumulDureePrint;
  unsigned long valTpsMaxPrint;
  unsigned int nbSeuil1Print, nbSeuil2Print;
  // Machine à état
  byte printTour = 0;
  byte nb_decimale = 0;
  unsigned long sp_limite;

  //

  // Variables de gestion d'envoi Serial
  char* ptrPrintCurrent;
  int lenPrintRemaining;
  byte etapePrintString;  // 0: envoi texte1, 1: envoi texte2

  // Tampon de travail (remplace CharAux, CharSeuil, etc.)
  char conversionBuf[6];  // forcer à 6
                          // byte etapeConversion;
                          //
  void gestionAffichage();

 public:
  yTempsCycle(const char sNomCycle[15], const unsigned int sIntervallePrintSec,
              const float sSeuilms1, const float sSeuilms2);

  void loop();
  void start();
  void stop();

  // CES VARIABLES SONT CONSERVÉES POUR ACCÈS EXTERNE
  char texte1[41];  // Taille suffisante pour une ligne
  char texte2[41];
  char NomCycle[16];    // Taille du nom
  bool newTxt = false;  // Indique qu'un nouveau texte est prêt
  static bool printEnCours;
};

#endif