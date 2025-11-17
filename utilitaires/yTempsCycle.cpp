#include <Arduino.h>
#include "yTempsCycle.h"
/*
 *  Permet surveiller le temp de cycle sur la base des micro seconde
 *  hervé CHUTEAU pour le Bistromatik
 *  version 1.01 du 17/11/2025
 * 
 *  Affichage sur le Terminal : Temps de cycle   moyen 0.02   max 0.02   >0.08 0   >>0.15 0
 * 
 *  Création de Class : yTempsCycle TempsCycle("Temps de cycle", 4, 0.08, 0.15);
 * 
 *  Dans la boucle à surveiller : TempsCycle.loop();
 * 
 *  TempsCycle.newTxt : nouveau affichage formaté
 *  TempsCycle.texte1 et TempsCycle.texte2 : affichage 
 *
 *  Pour limiter l'impact sur le temps de cycle la mise en forme est decomposée opération par opération
 *  le serial.print se fait 10 carcatères pas 10 caractères ce qui limite le temps max à 0,12ms
 *  (temps équivalent aux opérations les plus gourmantes de mise en forme)
 *
 *  sur MEGA2560  avec monitor_speed = 115200
 *         Par rapport à un Mega affichant "Hello World"
 *         RAM: 645 bytes from 8192 bytes
 *         Flash: 4372 bytes from 253952 bytes
 *
 *  Temps mouyen d'éxécution 0.02ms
 *  Temps maximum 0.12ms  (seulement 10 tours de cycle > 8ms)
 */

//
void yTempsCycle_concat_char(char *spCharChaine, unsigned int sCharSize, char *spCharAux);
void yTempsCycle_concat_const(char *spCharChaine, unsigned int sCharSize, const char *spCharAux);
bool yTempsCycle::printEnCours = false;

// Paramètres : intervalle d'affichage en sec, Seuils en milliseconde)
yTempsCycle::yTempsCycle(const char sNomCycle[NB_CHAR_NOM], const unsigned int sIntervallePrintSec, const float sSeuilms1, const float sSeuilms2)
{
	intervallePrintSec = sIntervallePrintSec * 1000000.00; // 5 sec = 5 000 000ms
	// surveillance des temps de cycle long
	if (sSeuilms1 < sSeuilms2)
	{
		seuil1 = sSeuilms1 * 1000.00; // Temps seuil 1 en μs
	}
	else
	{
		seuil1 = sSeuilms2 * 1000.00;
	}
	seuil2 = sSeuilms2 * 1000.00; // Temps seuil 2 en μs
	//
	limite = seuil_LONG - intervallePrintSec - 10;
	strcpy(NomCycle, sNomCycle);
	seuil1Ms = (float)seuil1 / 1000.00;
	seuil2Ms = (float)seuil2 / 1000.00;
	dtostrf(seuil1Ms, 4, 2, CharSeuil1);
	dtostrf(seuil2Ms, 4, 2, CharSeuil2);
}
/* *****************************
 *    T E M P S   C Y C L E    */
void yTempsCycle::loop()
{

	microsVal = micros(); // photo de registre microseconde
	compteur++;

	if (!CalculActif)
	{
		// on est au premier tour ou au retour de débordement
		if (microsVal < limite)
		{
			// on ne fait quelque chose que si inférieur limite haute (débordement)
			CalculActif = true;
			max = 0ul;
			nbSeuil1 = 0;
			nbSeuil2 = 0;
			compteur = 0ul;
			cumulDuree = 0ul;
			memMicros = 0ul;
		}
	}
	else
	{
		if (memMicros > 0ul)
		{
			dureeCycle = microsVal - memMicros;
			cumulDuree += dureeCycle;
			// calcul max et seuils
			if (dureeCycle > max)
			{
				max = dureeCycle;
			}
			if (dureeCycle > seuil2)
			{
				nbSeuil2++;
			}
			else if (dureeCycle > seuil1)
			{
				nbSeuil1++;
			}
		}
		// doit-on impprimer
		if ((cumulDuree > intervallePrintSec) && (printTour < 1))
		{
			printTour = 1; // lance les opération Serial.print
		}

		switch (printTour)
		{
		case 1:
			compteurPrint = compteur;
			cumulDureePrint = cumulDuree;
			maxPrint = max;
			nbSeuil1Print = nbSeuil1;
			nbSeuil2Print = nbSeuil2;
			//
			max = 0ul;
			nbSeuil1 = 0;
			nbSeuil2 = 0;
			compteur = 0ul;
			cumulDuree = 0ul;
			//
			SerialPrint = false;
			IndexCharSerial = 0;
			//
			printTour++;
			break;

		case 2:
			moyenMs = ((float)cumulDureePrint / (float)compteurPrint) / 1000.00;
			maxMs = (float)maxPrint / 1000.00;
			//
			printTour++;
			break;
		case 3:
			strcpy(texte1, NomCycle);
			yTempsCycle_concat_const(texte1, sizeof(texte1), PSTR("   moyen "));
			//
			printTour++;
			break;
		case 4:
			dtostrf(moyenMs, 4, 2, CharAux); // moyen
			yTempsCycle_concat_char(texte1, sizeof(texte1), CharAux);
			//
			printTour++;
			break;
		case 5:
			yTempsCycle_concat_const(texte1, sizeof(texte1), PSTR("   "));
			strcpy_P(texte2, PSTR("max "));
			//
			printTour++;
			break;
		case 6:
			dtostrf(maxMs, 4, 2, CharAux);							  // max
			yTempsCycle_concat_char(texte2, sizeof(texte2), CharAux); // max
			//
			printTour++;
			break;
		case 7:
			yTempsCycle_concat_const(texte2, sizeof(texte2), PSTR("   >"));
			yTempsCycle_concat_char(texte2, sizeof(texte2), CharSeuil1); // seuil1
			yTempsCycle_concat_const(texte2, sizeof(texte2), PSTR(" "));
			//
			printTour++;
			break;
		case 8:
			itoa(nbSeuil1Print, CharAux, 10); // seuil1
			yTempsCycle_concat_char(texte2, sizeof(texte2), CharAux);
			//
			printTour++;
			break;
		case 9:
			yTempsCycle_concat_const(texte2, sizeof(texte2), PSTR("   >>")); // seuil2
			yTempsCycle_concat_char(texte2, sizeof(texte2), CharSeuil2);	 // seuil2
			yTempsCycle_concat_const(texte2, sizeof(texte2), PSTR(" "));
			//
			printTour++;
			break;
		case 10:													  // seuil2
			itoa(nbSeuil2Print, CharAux, 10);						  // seuil2
			yTempsCycle_concat_char(texte2, sizeof(texte2), CharAux); // seuil2
			//
			printTour++;
			break;
		case 11:
			strcpy(CharChaine, texte1);
			yTempsCycle_concat_char(CharChaine, sizeof(CharChaine), texte2);
			newTxt = true;
			//
			printTour++;
			break;
		case 12:
			if (!printEnCours)
			{
				printTour++;
				printEnCours = true;
			}
			break;
		case 13:
			LenChaine = strlen(CharChaine);
			if (Serial.availableForWrite() >= NB_CHAR_SERIAL)
			{

				if (!SerialPrint)
				{

					for (int i = 0; i < NB_CHAR_SERIAL; i++)
					{
						IndexAux = IndexCharSerial + i;
						if (IndexAux < LenChaine)
						{
							CharSerial[i] = CharChaine[IndexAux];
						}
						else
						{
							CharSerial[i] = 0;
						}
					}
					IndexCharSerial += NB_CHAR_SERIAL;
					CharSerial[NB_CHAR_SERIAL] = '\0';
					SerialPrint = true;
				}
				else
				{

					Serial.write(CharSerial);
					SerialPrint = false;
				}
			}
			if ((IndexCharSerial >= LenChaine) && !SerialPrint)
			{
				Serial.println(F(""));
				IndexCharSerial = 0;
				SerialPrint = false;
				printTour++;
				printEnCours = false;
			}

			break;

		default:
			printTour = 0;
			break;
		}
	}
	//
	if (microsVal > limite)
	{
		CalculActif = false; // limite de débordement
	}
	else
	{
		memMicros = microsVal; // pour ne pas perdre une micro seconde
	}
}
/* ************************************
 *      C O N C A T E N A T I O N
 *  fait une concaténation de char array
 *  en vérifiant le débordement
 *
 */
void yTempsCycle_concat_char(char *spCharChaine, unsigned int sCharSize, char *spCharAux)
{

	if ((strlen(spCharChaine) + strlen(spCharAux)) < sCharSize)
	{
		strcat(spCharChaine, spCharAux);
	}
	else
	{
		for (unsigned int i = strlen(spCharChaine); i < (sCharSize - 1); i++)
		{
			spCharChaine[i] = '#';
		}

		spCharChaine[sCharSize - 1] = '\0';
	}
}
/* ************************************
 *      C O N C A T E N A T I O N
 * fait une concaténation de texte constant
 * en vérifiant le débordement
 */
void yTempsCycle_concat_const(char *spCharChaine, unsigned int sCharSize, const char *spCharAux)
{
	if ((strlen(spCharChaine) + strlen_P(spCharAux)) < sCharSize)
	{
		strcat_P(spCharChaine, spCharAux);
	}
	else
	{
		for (unsigned int i = strlen(spCharChaine); i < (sCharSize - 1); i++)
		{
			spCharChaine[i] = '#';
		}
		spCharChaine[sCharSize - 1] = '\0';
	}
}
