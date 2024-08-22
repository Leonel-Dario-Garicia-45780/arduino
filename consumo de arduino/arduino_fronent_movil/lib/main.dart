
// import 'package:flutter/material.dart';
// import 'package:http/http.dart' as http;
// import 'dart:convert';

// void main() {
//   runApp(MyApp());
// }

// class MyApp extends StatelessWidget {
//   @override
//   Widget build(BuildContext context) {
//     return MaterialApp(
//       debugShowCheckedModeBanner: false,
//       title: 'Control de Máquina',
//       theme: ThemeData(
//         primarySwatch: Colors.blue,
//       ),
//       home: MyHomePage(),
//     );
//   }
// }

// class MyHomePage extends StatefulWidget {
//   @override
//   _MyHomePageState createState() => _MyHomePageState();
// }

// class _MyHomePageState extends State<MyHomePage> {
//   Future<void> controlarMaquina(String command) async {
//     // final url = 'http://192.168.1.101/control';  // Reemplaza con la IP correcta de tu ESP32
//     final url = 'http://192.168.167.253/control';  // Reemplaza con la IP correcta de tu ESP32
//     final headers = {
//       'Content-Type': 'application/json',
//     };
//     final body = {
//       'command': command,
//     };
//     final jsonBody = json.encode(body);

//     try {
//       final response = await http.post(
//         Uri.parse(url),
//         headers: headers,
//         body: jsonBody,
//       );

//       if (response.statusCode == 200) {
//         // Éxito
//         print('Respuesta del servidor:');
//         print(response.body);
//       } else {
//         // Error en la solicitud
//         print('Error en la solicitud: ${response.statusCode}');
//         print('Cuerpo de la respuesta: ${response.body}');
//       }
//     } catch (e) {
//       // Error de conexión
//       print('Error de conexión: $e');
//     }
//   }

//   @override
//   Widget build(BuildContext context) {

//     return Scaffold(

//       appBar: AppBar(
//         title: Text('Control de Máquina'),
//       ),
//       body: Center(
//         child: Column(
//           mainAxisAlignment: MainAxisAlignment.center,
//           children: <Widget>[
//             ElevatedButton(
//               onPressed: () {
//                 controlarMaquina('on');
//               },
//               child: Text('Encender Máquina'),
//             ),
//             SizedBox(height: 20),
//             ElevatedButton(
//               onPressed: () {
//                 controlarMaquina('off');
//               },
//               child: Text('Apagar Máquina'),
//             ),
//           ],
//         ),
//       ),
//     );
//   }
// }






















//! esto es con el bakend

import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Control de Máquina',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: MyHomePage(),
    );
  }
}

class MyHomePage extends StatefulWidget {
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  TimeOfDay? _selectedTime;
  String _responseMessage = ''; // Variable para almacenar la respuesta del servidor
// res.json({message:'maquina apagada exitsosamente'})
  Future<void> controlarMaquina(String command, {TimeOfDay? time}) async {
     final url = 'https://bakend-arduino.onrender.com/api/control'; // Actualiza con la URL correcta del backend (web)
    //final url = 'http://localhost:3000/api/control'; // Actualiza con la URL correcta del backend (local)
    final headers = {
      'Content-Type': 'application/json',
    };
    final body = {
      'command': command,
      if (time != null) 'hour': time.hour.toString().padLeft(2, '0'), // Agregar la hora
      if (time != null) 'minute': time.minute.toString().padLeft(2, '0'), // Agregar los minutos
    };
    final jsonBody = json.encode(body);

    try {
      final response = await http.post(
        Uri.parse(url),
        headers: headers,
        body: jsonBody,
      );

      if (response.statusCode == 200) {
        // Éxito: Actualiza el mensaje de respuesta
        setState(() {
          _responseMessage = 'Operación exitosa: ${response.body}';
        });
      } else {
        // Error en la solicitud: Actualiza el mensaje de error
        setState(() {
          _responseMessage = 'Error en la solicitud: ${response.statusCode}\n${response.body}';
        });
      }
    } catch (e) {
      // Error de conexión: Actualiza el mensaje de error
      setState(() {
        _responseMessage = 'Error de conexión: $e';
      });
    }
  }

  Future<void> _selectTime(BuildContext context) async {
    final TimeOfDay? picked = await showTimePicker(
      context: context,
      initialTime: TimeOfDay.now(),
    );
    if (picked != null && picked != _selectedTime) {
      setState(() {
        _selectedTime = picked;
      });
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Control de Máquina'),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            // Primer contenedor para los botones de encender y apagar
            Container(
              padding: EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: Colors.grey[200], // Color de fondo gris claro
                borderRadius: BorderRadius.circular(15),
              ),
              child: Column(
                children: [
                  ElevatedButton(
                    onPressed: () {
                      controlarMaquina('on');
                    },
                    child: Text('Encender Máquina'),
                    style: ElevatedButton.styleFrom(
                      foregroundColor: Colors.white, // Color del texto
                      backgroundColor: Colors.green, // Color del botón
                      padding: EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(20),
                      ),
                    ),
                  ),
                  SizedBox(height: 20),
                  ElevatedButton(
                    onPressed: () {
                      controlarMaquina('off');
                    },
                    child: Text('Apagar Máquina'),
                    style: ElevatedButton.styleFrom(
                      foregroundColor: Colors.white, // Color del texto
                      backgroundColor: Colors.red, // Color del botón
                      padding: EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(20),
                      ),
                    ),
                  ),
                ],
              ),
            ),

            SizedBox(height: 30), // Espacio entre los contenedores

            // Segundo contenedor para el botón de selección de hora y programar apagado
            Container(
              padding: EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: Colors.grey[200], // Color de fondo gris claro
                borderRadius: BorderRadius.circular(15),
              ),
              child: Column(
                children: [
                  ElevatedButton(
                    onPressed: () {
                      _selectTime(context);
                    },
                    child: Text(
                      _selectedTime == null
                          ? 'Seleccionar Hora de Apagado'
                          : 'Hora Seleccionada: ${_selectedTime!.format(context)}',
                    ),
                    style: ElevatedButton.styleFrom(
                      foregroundColor: Colors.white, // Color del texto
                      backgroundColor: Colors.blue, // Color del botón
                      padding: EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                      shape: RoundedRectangleBorder(
                        borderRadius: BorderRadius.circular(20),
                      ),
                    ),
                  ),
                  SizedBox(height: 20),
                  if (_selectedTime != null)
                    ElevatedButton(
                      onPressed: () {
                        controlarMaquina('off', time: _selectedTime);
                      },
                      child: Text('Programar Apagado a las ${_selectedTime!.format(context)}'),
                      style: ElevatedButton.styleFrom(
                        foregroundColor: Colors.white, // Color del texto
                        backgroundColor: Colors.red, // Color del botón
                        padding: EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                        shape: RoundedRectangleBorder(
                          borderRadius: BorderRadius.circular(20),
                        ),
                      ),
                    ),
                ],
              ),
            ),

            SizedBox(height: 30), // Espacio entre el contenedor y el mensaje

            // Caja para mostrar la respuesta del servidor
            if (_responseMessage.isNotEmpty)
              Container(
                padding: EdgeInsets.all(20),
                decoration: BoxDecoration(
                  color: Colors.grey[300], // Color de fondo gris claro
                  borderRadius: BorderRadius.circular(15),
                ),
                child: Text(
                  _responseMessage,
                  style: TextStyle(
                    color: Colors.black,
                    fontSize: 16,
                  ),
                  textAlign: TextAlign.center,
                ),
              ),
          ],
        ),
      ),
    );
  }
}



















