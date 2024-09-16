import 'package:flutter/material.dart';
import '../controllers/machine_controller.dart';
import '../widgets/machine_controls.dart';

class MyHomePage extends StatefulWidget {
  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  TimeOfDay? _selectedTime;
  String _responseMessage = '';

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

            MachineControls(
              onControl: (command) async {
                final response = await MachineController.controlMachine(command);
                setState(() {
                  _responseMessage = response;
                });
              },
              onTimePicked: () async {
                _selectTime(context);
              },
              selectedTime: _selectedTime,
              onProgramShutdown: () async {
                if (_selectedTime != null) {
                  final response = await MachineController.controlMachine('off', time: _selectedTime);
                  setState(() {
                    _responseMessage = response;
                  });
                }
              },
            ),
            
            SizedBox(height: 30),
            
            if (_responseMessage.isNotEmpty)
              Container(
                padding: EdgeInsets.all(20),
                decoration: BoxDecoration(
                  color: Colors.grey[300],
                  borderRadius: BorderRadius.circular(15),
                ),
                child: Text(
                  _responseMessage,
                  style: TextStyle(color: Colors.black, fontSize: 16),
                  textAlign: TextAlign.center,
                ),
              ),
          ],
        ),
      ),
    );
  }
}
