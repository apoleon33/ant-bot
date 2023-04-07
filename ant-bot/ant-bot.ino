/* les librairies pour l'écran */
#include <SPI.h>
#include <TFT_eSPI.h>

#define NB_CAPTEURS 3  // combien de capteurs on a

/* définitions des entrées/sorties */
#define moteur11 6  // moteur de droite
#define moteur12 7

#define moteur21 8  // moteur de gauche
#define moteur22 9

#define capteur1 27
#define capteur3 45
#define capteur5 23
const int listeCapteur[NB_CAPTEURS] = { capteur1, capteur3, capteur5 };

#define bontonJaune 4

/* touts les cas de figures qui pourront être rencontrés par les capteurs */
const int listeStop1[NB_CAPTEURS] = { 1, 1, 1 };
const int listeStop0[NB_CAPTEURS] = { 0, 0, 0 };
const int listeAvancer[NB_CAPTEURS] = { 0, 1, 0 };
const int listeGauche[NB_CAPTEURS] = { 1, 0, 0 };
const int listeDroite[NB_CAPTEURS] = { 0, 0, 1 };
const int listeAutreDroite[NB_CAPTEURS] = { 1, 1, 0 };
const int listeAutreGauche[NB_CAPTEURS] = { 0, 1, 1 };

/* 
  les statuts des boutons: comme des interrupteurs: 
  on clique une fois dessus -> ils passent à l'état haut 
  on reclique dessus -> ils passent à l'étatt bas
*/
bool statutBoutonVert = false;
bool statutBoutonJaune = false;

unsigned long timeDelayed;  // le temps écoulé entre le lancement de l'arduino et la pression du bouton vert
TFT_eSPI tft = TFT_eSPI();  // instanciation de la classe écran
/* fonctions pour faire marcher les moteurs */
void avancer() {
  /*digitalWrite(moteur11, LOW);
  digitalWrite(moteur12, HIGH);

  digitalWrite(moteur22, LOW);
  digitalWrite(moteur21, HIGH);*/

  digitalWrite(moteur11, HIGH);
  digitalWrite(moteur12, LOW);

  digitalWrite(moteur22, HIGH);
  digitalWrite(moteur21, LOW);
}
void tournerDroite() {  // testé
  digitalWrite(moteur11, LOW);
  digitalWrite(moteur12, LOW);
  digitalWrite(moteur21, LOW);
  digitalWrite(moteur22, HIGH);
}

void tournerGauche() {  // testé
  digitalWrite(moteur11, HIGH);
  digitalWrite(moteur12, LOW);
  digitalWrite(moteur21, HIGH);
  digitalWrite(moteur22, LOW);
}
void arreter() {
  digitalWrite(moteur11, LOW);
  digitalWrite(moteur12, LOW);

  digitalWrite(moteur22, LOW);
  digitalWrite(moteur21, LOW);
}
void reculer() {
  digitalWrite(moteur12, HIGH);
  digitalWrite(moteur11, LOW);

  digitalWrite(moteur21, LOW);
  digitalWrite(moteur22, HIGH);
}

int detectLine(int listeCapteurs[NB_CAPTEURS]) {
  /* fonction qui renvoie sous la forme d'une liste l'état de tout les capteurs */
  int listeDectectionCapteur[5] = { 0, 0, 0, 0, 0 };
  for (int i = 0; i < NB_CAPTEURS; i++) {
    listeDectectionCapteur[i] = digitalRead(listeCapteur[i]);
    delay(100);
  }
  return listeDectectionCapteur;
}

bool similarArray(int array1[NB_CAPTEURS], int array2[NB_CAPTEURS]) {
  /* 
  fonction pour comparer deux array array1 et array2 et dire si elles sont identiques
  par défault on dit qu'elles font la même taille chacune (le nombre de capteurs)
  source: https://bit.ly/42ChneT
  */
  for (int i = 0; i < NB_CAPTEURS - 1; i++) {
    if (array1[i] != array2[i]) {
      return false;
    }
  }
  return true;
}

void setup() {
  Serial.begin(9600);
  tft.begin();  // on lance l'écran
  pinMode(moteur11, OUTPUT);
  pinMode(moteur12, OUTPUT);
  pinMode(moteur21, OUTPUT);
  pinMode(moteur22, OUTPUT);

  /*
    À propos des couleurs on a ça de base
    TFT_BLACK       0x0000
    TFT_NAVY        0x000F
    TFT_DARKGREEN   0x03E0
    TFT_DARKCYAN    0x03EF
    TFT_MAROON      0x7800
    TFT_PURPLE      0x780F
    TFT_OLIVE       0x7BE0
    TFT_LIGHTGREY   0xC618
    TFT_DARKGREY    0x7BEF
    TFT_BLUE        0x001F
    TFT_GREEN       0x07E0
    TFT_CYAN        0x07FF
    TFT_RED         0xF800
    TFT_MAGENTA     0xF81F
    TFT_YELLOW      0xFFE0
    TFT_WHITE       0xFFFF
    TFT_ORANGE      0xFDA0
    TFT_GREENYELLOW 0xB7E0
    TFT_PINK        0xFC9F

    mais sinon on peut théoriquement mettre un nombre au pif(en hexadécimal) et voir comment ça rend
  */
  tft.fillScreen(TFT_DARKCYAN);                     // on met une belle couleure pour le fond d'écran
  tft.setTextColor(TFT_WHITE, TFT_DARKCYAN, true);  // la couleur du texte, et la couleur entre les lettres
  tft.setCursor(30, 70);
  tft.println("Veuillez cliquer sur");
  tft.setCursor(40, 90);
  tft.println("le bouton vert");

  arreter();
  timeDelayed = millis();
  tft.fillScreen(TFT_DARKCYAN);
  tft.setCursor(30, 70);
  tft.println("temp ecoule:");
  tft.setCursor(70, 120);
}

void loop() {

  String chaine = "";
  /* une fois qu'on a terminé la phase de mouvements du robot, on affiche le chrono*/
  unsigned long currentMillis = millis() - timeDelayed;  // on retire timeDelayed pour obtenir le tems écoulé depuis la pression du bouton vert, et non depuis le lancement de l'arduino
  unsigned long seconds = currentMillis / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;
  unsigned long days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  chaine += String(minutes) + "m " + String(seconds) + "s " + String(currentMillis) + "ms";

  tft.setCursor(70, 120);
  tft.print(chaine);

  // si le bouton vert a été cliqué une seule fois (ou un nombre impair de fois) -> on veut que le robot se déplace

  /* fonction qui renvoie sous la forme d'une liste l'état de tout les capteurs */
  arreter();
  int listeDectectionCapteur[NB_CAPTEURS] = { 0, 0, 0 };
  for (int i = 0; i < NB_CAPTEURS; i++) {
    listeDectectionCapteur[i] = digitalRead(listeCapteur[i]);
    Serial.print(digitalRead(listeCapteur[i]));
  }
  Serial.println();

  // premier cas de figure: seulement une ligne droite
  if (similarArray(listeDectectionCapteur, listeAvancer)) {  // on utilise la fonction définie plus haut qui renvoie vrai si les arrays sont similaires et faux dans le cas contraire
    Serial.println("on avance");
    avancer();
    delay(200);
  }

  // deuxième cas de figure: on a un angle à 90° vers la droite
  if (similarArray(listeDectectionCapteur, listeDroite)) {
    Serial.println("tournons à droite");  // on écrit aque l'on tourne à droite
    while (similarArray(listeDectectionCapteur, listeDroite) || similarArray(listeDectectionCapteur, listeAutreDroite)) {
      // arreter();

      for (int i = 0; i < NB_CAPTEURS; i++) {
        listeDectectionCapteur[i] = digitalRead(listeCapteur[i]);
        Serial.print(digitalRead(listeCapteur[i]));
      }
      Serial.println();

      tournerDroite();  // tant que les capteurs detecte la ligne a le  ²r droite on tourne
    }
  }

  // troisiéme cas de figure: on a un angle à 90° vers la gauche
  if (similarArray(listeDectectionCapteur, listeGauche)) {

    while (similarArray(listeDectectionCapteur, listeGauche) || similarArray(listeDectectionCapteur, listeAutreGauche)) {

      for (int i = 0; i < NB_CAPTEURS; i++) {
        listeDectectionCapteur[i] = digitalRead(listeCapteur[i]);
        Serial.print(digitalRead(listeCapteur[i]));
      }
      Serial.println();
      tournerGauche();
    }
  }

  // autre angle de rotation
  if (similarArray(listeDectectionCapteur, listeAutreGauche)) {

    while (similarArray(listeDectectionCapteur, listeAutreGauche) || similarArray(listeDectectionCapteur, listeGauche)) {

      for (int i = 0; i < NB_CAPTEURS; i++) {
        listeDectectionCapteur[i] = digitalRead(listeCapteur[i]);
        Serial.print(digitalRead(listeCapteur[i]));
      }
      Serial.println();
      tournerGauche();
    }
  }

  // autre angle de rotation 2
  // ()
  if (similarArray(listeDectectionCapteur, listeAutreDroite)) {

    while (similarArray(listeDectectionCapteur, listeAutreDroite) || similarArray(listeDectectionCapteur, listeDroite)) {

      for (int i = 0; i < NB_CAPTEURS; i++) {
        listeDectectionCapteur[i] = digitalRead(listeCapteur[i]);
        Serial.print(digitalRead(listeCapteur[i]));
      }
      Serial.println();
      tournerGauche();
    }
  }
  /*
    // quatriéme cas de figure: on se stop
  if (similarArray(listeDectectionCapteur, listeStop0)||(similarArray(listeDectectionCapteur, listeStop1))) {
    Serial.println("Stop !!");
    arreter();

  }   */
}
