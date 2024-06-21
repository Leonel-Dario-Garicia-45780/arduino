int pinPulsador = 4;
int pinLed = 2;
int pulsaciones = 0; // Variable para contar las pulsaciones del pulsador

void setup() {
  pinMode(pinPulsador, INPUT);
  pinMode(pinLed, OUTPUT);
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

      // Si el número de pulsaciones es par, encender el LED; si es impar, apagar el LED
      if (pulsaciones % 2 == 0) {
        digitalWrite(pinLed, HIGH);
      } else {
        digitalWrite(pinLed, LOW);
      }
    }

    // Esperar a que el botón se suelte antes de aceptar otra pulsación
    while (digitalRead(pinPulsador) == HIGH) {
      delay(10); // Pequeño delay para evitar lecturas falsas
    }
  }
}











///! este codigo mantiene el estado del led y cambiara cuando el pin 4 reciba una señal
// int pinPulsador = 4;
// int pinLed = 2;
// int pulsaciones = 0; // Variable para contar pulsaciones del pulsador

// void setup() {
//   pinMode(pinPulsador, INPUT);
//   pinMode(pinLed, OUTPUT);

//   Serial.begin(9600); // Iniciar comunicación serial a 9600 baudios
//   Serial.println("Programa iniciado. Presiona el pulsador para alternar el LED.");
// }

// void loop() {
//   int estadoPulsador = digitalRead(pinPulsador);

//   if (estadoPulsador == HIGH) {
//     delay(50); // Espera para evitar rebotes

//     estadoPulsador = digitalRead(pinPulsador);

//     if (estadoPulsador == HIGH) {
//       pulsaciones++; // Incrementar el contador de pulsaciones

//       if (pulsaciones % 2 == 0) {
//         digitalWrite(pinLed, HIGH);
//         Serial.println("LED encendido");
//       } else {
//         digitalWrite(pinLed, LOW);
//         Serial.println("LED apagado");
//       }
//     }

//     // Esperar a que el botón se suelte antes de aceptar otra pulsación
//     while (digitalRead(pinPulsador) == HIGH) {
//       delay(10); // Pequeño delay para evitar lecturas falsas
//     }
//   }
// }









///!! este codigo enciende y apaga el led, dependiendo de cuando se mantenga presionado el pulsador mantendra el estado del led
// int pinPulsador = 4;
// int pinLed = 2;

// bool ledState = false; // Estado actual del LED
// bool lastButtonState = LOW; // Estado anterior del botón

// void setup() {
//   pinMode(pinPulsador, INPUT);
//   pinMode(pinLed, OUTPUT);
//   digitalWrite(pinLed, ledState); // Asegúrate de que el LED comienza en el estado correcto
// }

// void loop() {
//   // Leer el estado actual del botón
//   bool currentButtonState = digitalRead(pinPulsador);

//   // Detectar el cambio de estado del botón
//   if (currentButtonState == HIGH && lastButtonState == LOW) {
//     // Cambiar el estado del LED
//     ledState = !ledState;
//     digitalWrite(pinLed, ledState ? HIGH : LOW);
    
//     // Añadir un pequeño retraso para debouncing (anti-rebote)
//     delay(50);
//   }

//   // Actualizar el estado anterior del botón
//   lastButtonState = currentButtonState;
  
//   // Añadir un pequeño retraso para el ciclo de lectura
//   delay(90);
// }
