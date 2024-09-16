import 'package:flutter/material.dart';

class MachineControls extends StatelessWidget {
  final Function(String command) onControl;
  final Function() onTimePicked;
  final TimeOfDay? selectedTime;
  final Function() onProgramShutdown;

  MachineControls({
    required this.onControl,
    required this.onTimePicked,
    this.selectedTime,
    required this.onProgramShutdown,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        // Contenedor para agrupar los botones de encender y apagar
        Container(
          padding: const EdgeInsets.all(20),
          decoration: BoxDecoration(
            color: Colors.grey[200], // Fondo gris claro
            borderRadius: BorderRadius.circular(15),
          ),
          child: Column(
            children: [
              ElevatedButton(
                onPressed: () => onControl('on'),
                child: Text('Encender Máquina'),
                style: ElevatedButton.styleFrom(
                  foregroundColor: Colors.white,
                  backgroundColor: Colors.green,
                  padding: EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(20),
                  ),
                ),
              ),
              SizedBox(height: 20),
              ElevatedButton(
                onPressed: () => onControl('off'),
                child: Text('Apagar Máquina'),
                style: ElevatedButton.styleFrom(
                  foregroundColor: Colors.white,
                  backgroundColor: Colors.red,
                  padding: EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(20),
                  ),
                ),
              ),
            ],
          ),
        ),

        // Espaciado entre secciones
        SizedBox(height: 30),

        // Contenedor para la selección de hora de apagado
        Container(
          padding: const EdgeInsets.all(20),
          decoration: BoxDecoration(
            color: Colors.grey[200], // Fondo gris claro
            borderRadius: BorderRadius.circular(15),
          ),
          child: Column(
            children: [
              ElevatedButton(
                onPressed: onTimePicked,
                child: Text(
                  selectedTime == null
                      ? 'Seleccionar Hora de Apagado'
                      : 'Hora Seleccionada: ${selectedTime!.format(context)}',
                ),
                style: ElevatedButton.styleFrom(
                  foregroundColor: Colors.white,
                  backgroundColor: Colors.blue,
                  padding: EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                  shape: RoundedRectangleBorder(
                    borderRadius: BorderRadius.circular(20),
                  ),
                ),
              ),

              SizedBox(height: 20),

              // Botón para programar el apagado a la hora seleccionada
              if (selectedTime != null)
                ElevatedButton(
                  onPressed: onProgramShutdown,
                  child: Text('Programar Apagado a las ${selectedTime!.format(context)}'),
                  style: ElevatedButton.styleFrom(
                    foregroundColor: Colors.white,
                    backgroundColor: Colors.red,
                    padding: EdgeInsets.symmetric(horizontal: 30, vertical: 15),
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(20),
                    ),
                  ),
                ),
            ],
          ),
        ),
      ],
    );
  }
}