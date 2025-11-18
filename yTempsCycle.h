#ifndef xTEMPSCYCLE_H
#define xTEMPSCYCLE_H



class yTempsCycle
{
private:
    
    unsigned long intervallePrintSec; // temps entre 2 envois sur le terminal en Seconde
    unsigned long seuil1;             // valeur du seuil 1 en microseconde
    unsigned long seuil2;             // valeur du seuil 1 en microseconde
    //
    unsigned long microsVal; // pour ne faire l'appel à la fonction micros() qu'une fois
    // micros() 0,93µs  lecture Unsigned long 0,03µs et permet de figer la valeur pour un tour de programme
    unsigned long compteur;
    unsigned long max;
    unsigned long nbSeuil1;
    unsigned long nbSeuil2;
    //
    bool CalculActif = false;
    const unsigned long seuil_LONG = 4294967295ul;
    unsigned long limite;
    //
    unsigned long memMicros;
    unsigned long dureeCycle; // calcul du temps de cycle actuel
    //
    unsigned long compteurPrint; // nombre de tour programme
    unsigned long cumulDuree, cumulDureePrint;
    unsigned long maxPrint;
    float maxMs;
    // surveillance des temps de cycle long
    unsigned int nbSeuil1Print, nbSeuil2Print;
    float seuil1Ms, seuil2Ms;
     // temps de cycle moyen
    float moyenMs;
    //
    //
    byte printTour = 0; // c'est le moment d'imprimer

    // longueur des char array
    // static = idem pour toute les instances
    static const int NB_CHAR_CHAINE = 80;
    static const int NB_CHAR_TEXTE = NB_CHAR_CHAINE/2;
      static const int NB_CHAR_AUX = 10;
    static const int NB_CHAR_SERIAL = 10;
    static const int NB_CHAR_NOM=20;
    int IndexCharSerial, IndexAux, LenChaine;
    bool SerialPrint = false;
    
    //
    char CharChaine[NB_CHAR_CHAINE];
    char CharAux[NB_CHAR_AUX];
    char CharSerial[NB_CHAR_SERIAL + 1];
    char CharSeuil1[NB_CHAR_AUX], CharSeuil2[NB_CHAR_AUX];
   



public:
    yTempsCycle(const char sNomCycle[20] ,const unsigned int sIntervallePrintSec, const float sSeuilms1, const float sSeuilms2);
    // sIntervallePrintSec : intervalle d'affichage en sec
    // sSeuilms_1 : premier seuil en milli seconde
    // sSeuilms_2 : second seuil en milli seconde
    void loop();     // calcul à mettre dans loop
    char texte1[NB_CHAR_TEXTE]; // première chaine de caractère
    char texte2[NB_CHAR_TEXTE];
    bool newTxt = false; // Nouvelle valeur. A reseter une fois pris en compte
    char NomCycle[NB_CHAR_NOM];
   static bool printEnCours ;
};
#endif