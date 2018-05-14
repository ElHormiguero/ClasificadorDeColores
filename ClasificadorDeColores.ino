/* Robot que ordena caramelos por colores movido por dos servomotores y un sensor TCS3200 de color incorporando un interrupor y un led RGB
   Autor: Javier Vargas. El Hormiguero 14/05/2018
   Basado en proyecto de Dejan Nedelkovsk: https://howtomechatronics.com/projects/arduino-color-sorter-project/
   https://creativecommons.org/licenses/by/4.0/
*/

//-- PINES
#define pinServoArriba 9
#define pinServoAbajo 10
#define pinR 3
#define pinG 5
#define pinB 6
#define S0 A0
#define S1 A1
#define S2 A2
#define S3 A3
#define sensorOut 2
#define pinInterruptor 13

//-- CONFIGURACION
//Angulos
#define AngRecoger 168
#define AngSensor 122
#define AngCaer 82
#define AngRojo 52
#define AngNaranja 66
#define AngVerde 82
#define AngAmarillo 98
#define AngMarron 116
#define AngAzul 132
//Velocidad
#define VelLento 20 //Tiempo en ms por cada grado de movimiento lento
#define VelRapido 2 //Tiempo en ms por cada grado de movimiento rapido
//Sensor
#define Nmedidas 10 //Numero de medidas del sensor (se toma el color que salga mas veces)
#define MargenRGB 5 //Margen permitido entorno al la tonalidad RGB indicada
#define TimeOut 3 //Tiempo en segundos tras los cuales mueve el servo de arriba al no detectar el color

//Tonalidad de colores RGB 
int RGBrojo[] = {64, 90, 71}; 
int RGBnaranja[] = {57, 85, 73}; 
int RGBverde[] = {67, 78, 71}; 
int RGBamarillo[] = {55, 72, 69};
int RGBmarron[] = {69, 90, 73}; 
int RGBazul[] = {71, 83, 63};
int RGBnada[] = {59, 80, 64};

#include <Servo.h>
Servo ServoArriba;
Servo ServoAbajo;

int color = 0;
boolean Vacio = 0;
unsigned long m = 0;

void setup() {
  Serial.begin(9600);

  //PinMode
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  pinMode(pinInterruptor, INPUT_PULLUP);

  //Frecuencia del sensor escalada al 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  //Servos
  ServoArriba.attach(pinServoArriba);
  ServoAbajo.attach(pinServoAbajo);
  ServoArriba.write(AngRecoger);
  ServoAbajo.write(AngRojo);

  //RGB apagado
  RBG(0, 0, 0);
}

////////////////////////////
/////////////LOOP//////////
///////////////////////////

void loop() {
  //Espera mientras el interruptor esté apagado
  while (!InterruptorON()) {
    RBG(100, 100, 100);
    MovServoArriba(VelRapido, AngRecoger);
    delay(500);
    RBG(0, 0, 0);
    delay(500);
  }

  //Recoger
  MovServoArriba(VelRapido, AngRecoger);
  delay(1000);
  //Colocar en la posicion del sensor
  MovServoArriba(VelLento, AngSensor);
  delay(100);
  //Lectura del sensor
  RBG(0, 0, 0);
  LecturaColor();
  m = millis() + TimeOut*1000;
  while (color == 0 && InterruptorON()) {
    LecturaColor();
    //Recolocar si no detecta nada en TimeOut segundos
    if (millis() > m) {
      MovServoArriba(VelLento, AngSensor + 20);
      MovServoArriba(VelLento, AngSensor);
      m = millis() + TimeOut*1000;
    }
  }
  //Mover carril en fucnion del color
  switch (color) {
    case 1:
      RBG(255, 0, 0);
      MovServoAbajo(VelRapido, AngRojo);
      break;
    case 2:
      RBG(255, 50, 0);
      MovServoAbajo(VelRapido, AngNaranja);
      break;
    case 3:
      RBG(0, 255, 0);
      MovServoAbajo(VelRapido, AngVerde);
      break;
    case 4:
      RBG(255, 128, 0);
      MovServoAbajo(VelRapido, AngAmarillo);
      break;
    case 5:
      RBG(128, 20, 0);
      MovServoAbajo(VelRapido, AngMarron);
      break;
    case 6:
      RBG(0, 0, 255);
      MovServoAbajo(VelRapido, AngAzul);
      break;
    case 7:
      RBG(255, 255, 255);
      delay(500);
      RBG(0, 0, 0);
      Vacio = 1; //No hay más emanems
      break;
  }
  //Dejar caer en el carril
  if (!Vacio) {
    MovServoArriba(VelLento, AngCaer);
    delay(200);
  }
  //Esperar un tiempo a comprobar si se ha recargado
  else {
    delay(3000);
    Vacio = 0;
  }

}

///////////////////////////
///////////////////////////
///////////////////////////

void LecturaColor() {
  int M[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //Vector de colores
  Serial.println("");
  Serial.println("Sensor de color RGB: ");
  for (int i = 0; i < Nmedidas; i++) {
    // Filtro rojo
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    int R = pulseIn(sensorOut, LOW);
    delay(50);

    // Filtro verde
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    int G = pulseIn(sensorOut, LOW);
    delay(50);

    // Filtro azul
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    int B = pulseIn(sensorOut, LOW);
    delay(50);
    Serial.print("Muestra ");
    Serial.print(i+1);
    Serial.print(": ");
    Serial.print(R);
    Serial.print(" / ");
    Serial.print(G);
    Serial.print(" / ");
    Serial.println(B);

    //Color
    if (RGBok(R, G, B, RGBrojo)) color = 1; //Rojo
    else if (RGBok(R, G, B, RGBnaranja)) color = 2; //Naranja
    else if (RGBok(R, G, B, RGBverde)) color = 3; //Verde
    else if (RGBok(R, G, B, RGBamarillo)) color = 4; //Amarillo
    else if (RGBok(R, G, B, RGBmarron)) color = 5; //Marron
    else if (RGBok(R, G, B, RGBazul)) color = 6; //Azul
    else if (RGBok(R, G, B, RGBnada)) color = 7; //Nada
    else color = 0;
    M[color]++;
  }

  //Color obtenido más veces
  color = 0;
  int count = 0;
  for (int i = 0; i < 8; i++) {
    if (M[i] > count) {
      color = i;
      count = M[i];
    }
  }
  Serial.print("Color ");
  Serial.print(color);
  Serial.print(" obtenido ");
  Serial.print(count);
  Serial.println(" veces");
}

boolean RGBok(int R, int G, int B, int RGBset[3]) {
  if (R > RGBset[0] - MargenRGB && R < RGBset[0] + MargenRGB && G > RGBset[1] - MargenRGB && G < RGBset[1] + MargenRGB && B > RGBset[2] - MargenRGB && B < RGBset[2] + MargenRGB) return 1;
  else return 0;
}

void MovServoArriba(int d, int angulo) {
  static int AngActual = AngRecoger;
  while (AngActual != angulo) {
    if (angulo > AngActual) AngActual++;
    else AngActual--;
    ServoArriba.write(AngActual);
    delay(d);
  }
}

void MovServoAbajo(int d, int angulo) {
  static int AngActual = AngRojo;
  while (AngActual != angulo) {
    if (angulo > AngActual) AngActual++;
    else AngActual--;
    ServoAbajo.write(AngActual);
    delay(d);
  }
}

void RBG(byte R, byte G, byte B) {
  analogWrite(pinR, R);
  analogWrite(pinG, G);
  analogWrite(pinB, B);
}

boolean InterruptorON() {
  return !digitalRead(pinInterruptor);
}

