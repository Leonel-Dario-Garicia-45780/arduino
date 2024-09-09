import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:flutter/material.dart';

class ApiService {
  static final String url = 'https://bakend-arduino.onrender.com/api/control'; // URL del backend

  static Future<String> controlMachine(String command, {TimeOfDay? time}) async {
    final headers = {'Content-Type': 'application/json'};
    final body = {
      'command': command,
      if (time != null) 'hour': time.hour.toString().padLeft(2, '0'),
      if (time != null) 'minute': time.minute.toString().padLeft(2, '0'),
    };
    final jsonBody = json.encode(body);

    final response = await http.post(Uri.parse(url), headers: headers, body: jsonBody);

    if (response.statusCode == 200) {
      return 'Operación exitosa: ${response.body}';
    } else {
      return 'Error: ${response.statusCode}\n${response.body}';
    }
  }
}
