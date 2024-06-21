int pinPulsador=4;

int pinLed=2;

void setup() {
  // put your setup code here, to run once:

    // !!ensallo 1    enciende y apaga luz azul del pi 2  (toca mantener presionado el boton de BOOT  y luego se presiona el boton de EN)
      //pinMode(2,OUTPUT);

    // !! ensallo 2
    pinMode(pinPulsador, INPUT);
    pinMode(pinLed, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
    //!! ensallo 2
{
    if(digitalRead(pinPulsador)== HIGH ){

      digitalWrite(pinLed, HIGH); 
    }
    else{

      digitalWrite(pinLed, LOW); 
    }
    delay(100);
}

    // !!ensallo 1    enciende y apaga luz azul del pi 2  (toca mantener presionado el boton de BOOT  y luego se presiona el boton de EN)
      //  digitalWrite(2,HIGH);
      //  delay(3000);
      //  digitalWrite(2,LOW);
      //  delay(1000);

}
