import 'package:flutter/material.dart';
import '../services/api_service.dart';

class MachineController {
  static Future<String> controlMachine(String command, {TimeOfDay? time}) async {
    try {
      return await ApiService.controlMachine(command, time: time);
    } catch (e) {
      return 'Error: $e';
    }
  }
}
