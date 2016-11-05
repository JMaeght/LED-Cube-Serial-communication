#define dmx1 6
#define dmx2 7
#define activEtage 8
#define activDemux 9
#define tempsAffichageEtage 4

int Ligne[4] {2,3,4,5};

unsigned long time;
unsigned long tempsActualisation = 200;

int SupMat[4][4][4];

int etat = 0;

char programme;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////FONCTIONNEMENT DE BASE//////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////FONCTIONS////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//Allume toutes les led ou les éteint toutes
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

void eteintTout(int SupMat[4][4][4]){
  int etage,col,lin;
  for (etage=0;etage<=3;etage++){
    for (col=0;col<=3;col++){
      for (lin=0;lin<=3;lin++){
        SupMat[etage][col][lin]=0;
      }
    }
  }
}

/////////////////////////////////////MURS////////////////////////////////////

void murLin(int SupMat[4][4][4]){
  int etage,col,lin;

  if (etat <= 3){
    for(etage=0;etage<=3;etage++){
      for(col=0;col<=3;col++){
        for(lin=0;lin<=3;lin++){
          if(lin==etat){
            SupMat[etage][col][lin]=1;
          }
          else{
            SupMat[etage][col][lin]=0;
          }
        }
      }
    }
  }
  else{
    for(etage=0;etage<=3;etage++){
      for(col=0;col<=3;col++){
        for(lin=0;lin<=3;lin++){
          if(lin==7-etat){
            SupMat[etage][col][lin]=1;
          }
          else{
            SupMat[etage][col][lin]=0;
          }
        }
      }
    }
  }
  
  etat = (etat + 1) %8;;
}

void murCol(int SupMat[4][4][4]){
  int etage,col,lin;

  if (etat <= 3){
    for(etage=0;etage<=3;etage++){
      for(col=0;col<=3;col++){
        for(lin=0;lin<=3;lin++){
          if(col==etat){
            SupMat[etage][col][lin]=1;
          }
          else{
            SupMat[etage][col][lin]=0;
          }
        }
      }
    }
  }
  else{
    for(etage=0;etage<=3;etage++){
      for(col=0;col<=3;col++){
        for(lin=0;lin<=3;lin++){
          if(col==7-etat){
            SupMat[etage][col][lin]=1;
          }
          else{
            SupMat[etage][col][lin]=0;
          }
        }
      }
    }
  }
  
  etat = (etat + 1) %8;;
}

void murHoriz(int SupMat[4][4][4]){
  int etage,col,lin;

  if (etat <= 3){
    for(etage=0;etage<=3;etage++){
      for(col=0;col<=3;col++){
        for(lin=0;lin<=3;lin++){
          if(etage==etat){
            SupMat[etage][col][lin]=1;
          }
          else{
            SupMat[etage][col][lin]=0;
          }
        }
      }
    }
  }
  else{
    for(etage=0;etage<=3;etage++){
      for(col=0;col<=3;col++){
        for(lin=0;lin<=3;lin++){
          if(etage==7-etat){
            SupMat[etage][col][lin]=1;
          }
          else{
            SupMat[etage][col][lin]=0;
          }
        }
      }
    }
  }
  
  etat = (etat + 1) %8;;
}


/////////////////////////////////////PLUIE///////////////////////////////////

//Fonction de la pluie qui tombe
void tombelapluie(int SupMat[4][4][4]){
  int etage,col,lin;
  for(etage=0;etage<=2;etage++){
    for(col=0;col<=3;col++){
      for(lin=0;lin<=3;lin++){
        SupMat[etage][col][lin]=SupMat[etage+1][col][lin];
      }
    }
  }
}

//Fonction nuage qui fait apparaitre la pluie
void nuage(int SupMat[4][4][4],int pourcent){
  int col,lin,val;
  
  for(col=0;col<=3;col++){
    for(lin=0;lin<=3;lin++){
      if(random(100)<=pourcent){
        val = 1;
      }
      else{
        val = 0;
      }
      SupMat[3][col][lin] = val;
    }
  }
}

//Fonction à appeler pour faire tomber la pluie
void pluie(int SupMat[][4][4], int pourcent){
  tombelapluie(SupMat);
  nuage(SupMat,pourcent);
}


/////////////////////////////////////RECEPTION SERIAL///////////////////////////////////

// Communication via USB, lit 16 octets de 1 ou de 0. (Non utilisé ici)

void com(int SupMat[][4][4]){
  int etage,col,lin;
  
  if (Serial.available()>=16){
    for(etage=0;etage<=3;etage++){
      for(col=0;col<=3;col++){
        for(lin=0;lin<=3;lin++){
          
          SupMat[etage][col][lin]= Serial.read() - 48;
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////TRAITEMENT///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void setup () {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  time = 0;
  
  int etage,col,lin;
  int i;
  
  for (i=0;i<=13;i+=1){
    pinMode(i,OUTPUT);
  }
  
  eteintTout(SupMat);
}



void loop () {
  if (Serial.available()){
    programme = Serial.read();
    Serial.println(programme,DEC);
  }
    
  if (millis() > time + tempsActualisation){
    if (programme == 0){murHoriz(SupMat);}
    if (programme == 1){murLin(SupMat);}
    if (programme == 2){murCol(SupMat);}
    if (programme == 3){pluie(SupMat,10);}
    time = millis();
  }
  pCube(SupMat);
}
