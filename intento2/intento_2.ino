int pinPulsador = 4;
int pinLed = 2;
int pulsaciones = 0; // Variable para contar las pulsaciones del pulsador

void setup() {
  pinMode(pinPulsador, INPUT);
  pinMode(pinLed, OUTPUT);

  Serial.begin(9600); // Iniciar comunicación serial a 9600 baudios
  Serial.println("Esperando pulsaciones del botón...");
}

void loop() {
  // Leer el estado actual del pulsador
  int estadoPulsador = digitalRead(pinPulsador);

  // Verificar si el pulsador ha sido presionado (flanco ascendente)
  if (estadoPulsador == HIGH) {
    // Esperar un breve momento para evitar rebotes
    delay(50);

    // Verificar nuevamente el estado del pulsador después del delay
    estadoPulsador = digitalRead(pinPulsador);

    if (estadoPulsador == HIGH) {
      pulsaciones++; // Incrementar el contador de pulsaciones

      // Imprimir mensaje en el monitor serial
      Serial.print("Pulsación detectada. Pulsaciones totales: ");
      Serial.println(pulsaciones);

      // Si el número de pulsaciones es par, encender el LED; si es impar, apagar el LED
      if (pulsaciones % 2 == 0) {
        digitalWrite(pinLed, HIGH);
        Serial.println("LED encendido");
      } else {
        digitalWrite(pinLed, LOW);
        Serial.println("LED apagado");
      }
    }

    // Esperar a que el botón se suelte antes de aceptar otra pulsación
    while (digitalRead(pinPulsador) == HIGH) {
      delay(10); // Pequeño delay para evitar lecturas falsas
    }
  }
}
