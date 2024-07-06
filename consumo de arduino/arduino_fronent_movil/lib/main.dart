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
  Future<void> controlarMaquina(String command) async {
    final url = 'http://192.168.1.101/control';  // Reemplaza con la IP correcta de tu ESP32
    final headers = {
      'Content-Type': 'application/json',
    };
    final body = {
      'command': command,
    };
    final jsonBody = json.encode(body);

    try {
      final response = await http.post(
        Uri.parse(url),
        headers: headers,
        body: jsonBody,
      );

      if (response.statusCode == 200) {
        // Éxito
        print('Respuesta del servidor:');
        print(response.body);
      } else {
        // Error en la solicitud
        print('Error en la solicitud: ${response.statusCode}');
        print('Cuerpo de la respuesta: ${response.body}');
      }
    } catch (e) {
      // Error de conexión
      print('Error de conexión: $e');
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
            ElevatedButton(
              onPressed: () {
                controlarMaquina('on');
              },
              child: Text('Encender Máquina'),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () {
                controlarMaquina('off');
              },
              child: Text('Apagar Máquina'),
            ),
          ],
        ),
      ),
    );
  }
}
