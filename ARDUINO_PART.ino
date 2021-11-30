#include <Servo.h>  // librería para los servomotores
#include <SPI.h>

volatile char buf [20]; 
volatile int pos;
volatile bool active;
String str;

// servos
Servo servo_base, servo_arriba;
int sb = 50, sb_max = 179, sb_min = 1;
int sa = 30, sa_max = 170, sa_min = 10;

// pines de las fotoresistencias
int fr_izq_sup = A2;
int fr_der_sup = A4;
int fr_der_inf = A5;
int fr_izq_inf = A3;


void setup() {

  // configuramos el pin y la posición del servo que está en la base
  servo_base.attach(3);
  servo_base.write(50);

  // configuramos el pin y la posición del servo que está en la parte de arriba
  servo_arriba.attach(4);
 servo_arriba.write(30);

  Serial.begin(115200);                                     
    SPCR |= bit(SPE);                                         //ATMEGA328P/Arduino in slave mode
    pinMode(MISO, OUTPUT);                                    //Configure MISO as output, SlaveOut
    pinMode(9, OUTPUT);  //LED Test
    SPI.attachInterrupt();  
}
// SPI interrupt routine
ISR (SPI_STC_vect)
{
byte c = SPDR;

  if (c == 2){
    digitalWrite(9,HIGH);
  }
  if (c == 1)  // starting new sequence?
    {
    digitalWrite(9,LOW);
    active = true;
    pos = 0;
    SPDR = buf [pos++];   // send first byte
    return;
    }

  if (!active)
    {
    SPDR = 0;
    return;
    }

  SPDR = buf [pos];
  if (buf [pos] == 0 || ++pos >= sizeof (buf)){
    active = false;}
}  // end of interrupt

void loop() {

  // lecturas analógicas de las fotoresistencias
  int izq_sup = analogRead(fr_izq_sup);
  int der_sup = analogRead(fr_der_sup);
  int der_inf = analogRead(fr_der_inf);
  int izq_inf = analogRead(fr_izq_inf);
  
  // sacamos valores promedios de arriba, abajo, izquierda y derecha
  int prom_arriba = (der_sup + izq_sup) / 2;
  int prom_abajo = (der_inf + izq_inf) / 2;
  int prom_izquierda = (izq_sup + izq_inf) / 2;
  int prom_derecha = (der_sup + der_inf) / 2;

  // sacamos las diferencias entre arriba y abajo e izquierda y derecha.
  int dif_aa = prom_arriba - prom_abajo;
  int dif_id = prom_izquierda - prom_derecha;

  // calculamos las posiciones de los servomotores
  if (dif_aa < -80 || dif_aa > 80) {
    if (prom_arriba < prom_abajo) {
      sa = ++sa;
      if (sa > sa_max) {
        sa = sa_max;
      }
    } else {
      sa = --sa;
      if (sa < sa_min) {
        sa = sa_min;
      }
    }
    if (prom_arriba == prom_abajo) {
      delay(2000);
    }
    servo_arriba.write(sa);
  } 
  if (dif_id < -80 || dif_id > 80) {
    if (prom_izquierda < prom_derecha) {
      sb = ++sb;
      if (sb > sb_max) {
        sb = sb_max;
      }
    } else {
      sb = --sb;
      if (sb < sb_min) {
        sb = sb_min;
      }
    }
    if (prom_izquierda == prom_derecha) {
      delay(2000);
    }
    servo_base.write(sb);
  }

  Serial.print("Izq sup -> ");
  Serial.println(izq_sup);
  Serial.print("Der sup -> ");
  Serial.println(der_sup);
  Serial.print("Der int -> ");
  Serial.println(der_inf);
  Serial.print("Izq int -> ");
  Serial.println(izq_inf);
  Serial.print("Ángulo - servo_abajo -> ");
  Serial.println(sb);
  Serial.print("Ángulo - servo_arriba -> ");
  Serial.println(sa);
  Serial.println("- - - - - - - - - - - - - - -");
  // delay de 10 ms
  delay(10);
  str = String(izq_sup)+String(",")+String(izq_inf)+String(",")+String(der_sup)+String(",")+String(der_inf)+String(",")+String(sb)+String(",")+String(sa);
  str.toCharArray(buf,20);
  //itoa(der_inf, buf, 10);
}
