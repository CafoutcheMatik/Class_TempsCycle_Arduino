#ifndef xTEMPSCYCLE_H
#define xTEMPSCYCLE_H

#include <Arduino.h>  // Nécessaire pour byte, Serial, etc.

class yTempsCycle {
 private:
  unsigned int intervallePrintSec;
  unsigned int seuil1;
  unsigned int seuil2;
//
  unsigned long compteur;
  unsigned int max;
  unsigned int nbSeuil1;
  unsigned int nbSeuil2;
//
  bool CalculActif = false;
  const unsigned int seuil_U_INT = 65535;
  const unsigned long seuil_U_LONG = 4294967295ul;
  //
  // pour calcul durée
  unsigned long memMicros;
 // Variables figées pour l'impression
  unsigned long compteurPrint;
  unsigned long cumulDuree, cumulDureePrint;
  unsigned int maxPrint;
  unsigned int nbSeuil1Print, nbSeuil2Print;
  // Machine à état
  byte printTour = 0;
  // --- OPTIMISATION ---
  const int CHUNK_SIZE = 6;
  const unsigned int maxAffCompteur = 9999;
  const float maxSeuil = 65535/1000;
  const float maxAffSeuil = 999.99;

  // Variables de gestion d'envoi Serial
  char* ptrPrintCurrent;
  int lenPrintRemaining;
  byte etapePrintString;  // 0: envoi texte1, 1: envoi texte2

  // Tampon de travail (remplace CharAux, CharSeuil, etc.)
 char conversionBuf[6];   // forcer à 6
  //byte etapeConversion;
 public:
  yTempsCycle(const char sNomCycle[20], const unsigned int sIntervallePrintSec,
              const float sSeuilms1, const float sSeuilms2);

  void loop();

  // CES VARIABLES SONT CONSERVÉES POUR ACCÈS EXTERNE
  char texte1[40];   // Taille suffisante pour une ligne
  char texte2[40];
  char NomCycle[15];  // Taille du nom
  bool newTxt = false;  // Indique qu'un nouveau texte est prêt
  static bool printEnCours;
};

#endif