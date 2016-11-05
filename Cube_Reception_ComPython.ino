#define dmx1 6
#define dmx2 7
#define activEtage 8
#define activDemux 9
#define tempsAffichageEtage 4

int Ligne[4] {2,3,4,5};

unsigned long time;

int SupMat[4][4][4];

int etat = 0;
int clignote = 0;
int clignotebis = 0;

/////////////////////////////////////////////////////////////////////////////////
//////////////////////////////FONCTIONNEMENT DE BASE/////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

//Fonction de sortie ON/OFF
void set (int led, int etat){
  if (etat){digitalWrite(led,HIGH);}
  else {digitalWrite(led,LOW);}
}

//Paramétrer une ligne à partir d'une liste de 4 bits
void pLigne (int line, int Liste[4]){
  int i;
  for(i=0;i<=3;i+=1){
    set(Ligne[i],Liste[i]);
  }
  set(activEtage,0);
  set(dmx1,line & 0b0001);
  set(dmx2,line & 0b0010);
  set(activDemux,1);
  set(activDemux,0);
}

//Paramétrer un étage complet, à partir d'une Matrice 4x4 de bits...
//...et l'afficher une fraction de seconde dans la foulée.
void pEtage (int etage, int Mat[4][4]){
  int col;
  for (col=0;col<=3;col+=1){
    pLigne(col,Mat[col]);
    set(activEtage,1);
  }
  set(dmx1,etage & 0b0001);
  set(dmx2,etage & 0b0010);
  set(activDemux,1);
  delay(tempsAffichageEtage);
  set(activDemux,0);
}

//Paramétrer tout le cube. A faire tourner en boucle systématiquement.
void pCube (int SupMat[4][4][4]){
  int etage;
  for (etage=0;etage<=3;etage+=1){
    pEtage(etage,SupMat[etage]);
  }
}

//Pour tout allumer, au cas où.
void allumeTout(int SupMat[4][4][4]){
  int etage,col,lin;
  for (etage=0;etage<=3;etage++){
    for (col=0;col<=3;col++){
      for (lin=0;lin<=3;lin++){
        SupMat[etage][col][lin]=1;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////TRAITEMENT//////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

// Communication via USB, lit 8 octets definissant, 2 lignes par 2 lignes, le cube.

void deuxLignes(int info,int Ligne1[4],int Ligne2[4]){
  int n;
  for (n=0;n<=3;n++){
    Ligne1[n] = (0 != ((1<<n)&info));
  }
  for (n=0;n<=3;n++){
    Ligne2[n] = (0 != ((1<<(n+4))&info));
  }
}

void com(int SupMat[][4][4]){
  int etage,info;
  
  if (Serial.available()>=10){
    for(etage=0;etage<=3;etage++){
      info = Serial.read();
      deuxLignes(info,SupMat[etage][0],SupMat[etage][1]);
      info = Serial.read();
      deuxLignes(info,SupMat[etage][2],SupMat[etage][3]);
    }
    
    clignote = Serial.read();
    clignotebis = Serial.read();
    
  }
}

// Au cas où, pour vérifier la matrice...
void envoie(int SupMat[][4][4]){
  int etage,col,lin;
  for (etage=0;etage<=3;etage++){
    for (col=0;col<=3;col++){
      for (lin=0;lin<=3;lin++){
        Serial.print(SupMat[etage][col][lin],DEC);
      }
    }
  }
  Serial.println();
}

// Transforme un entier entre 1 et 64 en coordonnées de SupMat et change la led d'état
void faitClignoter(int clignote, int SupMat[][4][4]){
  int etage = int((clignote-1)/16);
  int col = int((clignote-1)/4) %4;
  int lin = (clignote-1) %4;
  SupMat[etage][col][lin] = 1 - SupMat[etage][col][lin];
}

//////////////////UNE fois   Important : c'est la remise à zéro//////////////////

void setup () {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  int etage,col,lin;
  int i;
  
  for (i=0;i<=13;i+=1){
    pinMode(i,OUTPUT);
  }
  
  allumeTout(SupMat);
}

/////////////////////////////PLEIN de fois///////////////////////////////////////

void loop () {
  com(SupMat);
  if (etat <= 1 and clignote != 0){
    faitClignoter(clignote,SupMat);
  }
  if (etat == 0 and clignotebis != 0){
    faitClignoter(clignotebis,SupMat);
  }
  pCube(SupMat);
  etat = (etat + 1) %4;
}
