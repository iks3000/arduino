// SPANISH
//-------------------------------------------------
//------------BABUINO MIDI PEDALBOARD--------------
//-------------------------------------------------
//----Conexiones-----------------------------------
//LEDS (pins 12 y 13)-> conectar pin digital a r220ohm + led y a masa -> https://www.arduino.cc/en/uploads/Tutorial/ExampleCircuit_sch.png
//BOTONES (pins digitales 2-7)-> -> https://www.arduino.cc/en/uploads/Tutorial/button_schem.png (R=10k)
//POTENCIOMENTROS (pins analogicos 0,1,2) -> B10k lineal -> pin1 = masa / pin2 = entrada analogica / pin3 = +5v -> https://www.arduino.cc/en/uploads/Tutorial/AnalogReadSerial_sch.png
//SENSOR HALL A1302 (pin 3 analogico) -> Visto del lado NO plano = pin1 = +5v / pin2 = masa / pin3 = entrada analogica - Lado no plano = lado activo (el que apunta al iman)
//PEDAL EXPRESION STANDARD -> jack stereo -> 1(L)= + ,2(R)= pin 2 del pote ,3(GND)= -
//-------------------------------------------------
//Librerias
#include <MIDI.h> //Añade libreria MIDI (https://github.com/FortySevenEffects/arduino_midi_library)
MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI); //Create an instance of the library attached to a serial port
//----Variables------------------------------------
//LEDS
const int led_on_pin=13; //pin de el LED ON
const int led_send_pin=12; //pin de el LED SEND
int led_send_time = 50; //ms de parpadeo de el led send = tiempo que estara encendido antes de apagarse (que no sea mayor que el threshold de los buttons o pots)
//BOTONES
int buttons_number = 7; //(6 fijos + 1 externo = 7) indica el numero de botones usados (max=10), se asignaran consecutivamente del pin digital 2 al 12
int buttons_pins[11] = {2,3,4,5,6,7,8,9,10,11,12}; //los pines que seran usados consecuetivamente para asignarse a los botones, dejado libre el 0,1(rt, tx)y 13 (led)
int buttons_status[11] = {0,0,0,0,0,0,0,0,0,0,0}; //variable for reading the pushbutton status, no modificiar
int buttons_pressed[11] = {0,0,0,0,0,0,0,0,0,0,0}; //estado inicial no pulsado aun, no modificar
int buttons_CC_numbers[11] = {102,103,104,105,106,107,108,109,110,111,112}; //numero de CC de cada boton (102 to 119 are free)
int buttons_CC_values[11] = {1,1,1,1,1,1,1,1,1,1,1}; // valor de cada CC number, en este caso de botones no importa mucho creo.
int buttons_CC_channels[11] = {1,1,1,1,1,1,1,1,1,1,1}; //canal de cada CC number, no muy importante pero mejor dejar todos en el mismo.
int buttons_threshold[11] = {500,500,500,500,500,500,500,500,500,500,500}; //ms de delay entre pulsacion y pulsacion para debouncing de cada boton
//POTENCIOMETROS+PEDAL EXPRESION STANDARD
int pots_number = 4; // select number of desired analog inputs (max 6, o 5 pues el 6 se usa para en sensor hall),1,2,3 son potes + el 4 es el pedal standard / para añadir añadir y automaticamente se añade a la siguiente salida analogica(de 0 a 6)
int pedal_expresion_standard_pin = 3; //pin analogico donde esta el pedal de expresion standard
int pedal_expresion_standard_reverse = 0; //0 = lo deja como esta, 1 = lo invierte
int pots_analog_value[6] = {0,0,0,0,0,0}; // define variables for the controller data
int pots_last_analog_value[6] = {0,0,0,0,0,0}; // define the "lastValue" variables
int pots_cc_numbers[6] = {20,21,22,23,24,25}; // select the midi Controller Number for each input (20 to 31 are free)
int pots_cc_channels[6] = {1,1,1,1,1,1}; //canal de cada pote
int pots_threshold_noise[6] = {4,4,4,4,4,4}; //threshold de ruido (lectura inestable) de cada pote, a menos mas precision pero mas inestable, a mas menos precision pero mas estable.
int pots_threshold_delay[6] = {50,50,50,50,50,50}; //threshold de retraso tras mover cada pote
//SENSOR HALL/PEDAL EXPRESION
int sensor_hall_pin = 5; // pin del sensor hall
int sensor_hall_last_value = 0; // define the "lastValue"
int sensor_hall_cc_number = 85; //Numero de CC MIDI (85-90 son libres)
int sensor_hall_cc_channel = 1; //Canal del CC MIDI
int sensor_hall_threshold_noise = 3; //threshold de ruido (lectura inestable, el led send se queda parpadeando sin tocar el pedal) del sensor hall, a menos mas precision pero mas inestable, a mas menos precision pero mas estable.
int sensor_hall_threshold_delay = 50;//threshold de retraso tras mover el sensor hall/pedal
int sensor_hall_selected_mode = 4;//modo del sensor seleccionado por default, va de 1 a 4 (1 = logaritmico + natural, 2 = logaritmico + invertido, 3 = lineal + natural, 4 = lineal + invertido)
//Calibracion del sensor:
//rellenar estas variables con los valores obtenidos en cada posicion
//usar regla/pie de rey y la funcion de "utiles calibracion" para ver por el monitor serie los datos
int sensor_hall_min_value = 184;//ajuste sensor hall valor minimo = pedal cerrado/bajado
int sensor_hall_25_value = 334;//ajuste sensor hall valor al 25%
int sensor_hall_50_value = 431;//ajuste sensor hall valor al 50%
int sensor_hall_75_value = 459;//ajuste sensor hall valor al 75%
int sensor_hall_max_value = 468;//ajuste sensor hall valor maximo = pedal abierto/subido
//fin variables calibración
//----Setup----------------------------------------
void setup() {// put your setup code here, to run once:
MIDI.begin(); //Inicializa libreria MIDI
Serial.begin(115200); //Inicializa el puerto serie indicando con baudrate (DEBE SER EL MISMO QUE ESTE PUESTO EN HAIRLEES)
//LEDS
pinMode(led_on_pin,OUTPUT); //define el modo salida para el pin donde esta el LED ON
pinMode(led_send_pin,OUTPUT); //define el modo salida para el pin donde esta el LED SEND
//BOTONES
for (int i = 0; i < buttons_number; i++) { //Inicializa pins de los botones como inputs
pinMode(buttons_pins, INPUT);
}//end for
}//end setup
//----Loop-----------------------------------------
void loop() { // put your main code here, to run repeatedly:
//LEDS
digitalWrite(led_on_pin,HIGH); //enciende el LED ON
//BOTONES
for (int i = 0; i < buttons_number; i++) { //recorre todos los botones
int button_state = digitalRead(buttons_pins);//mira estado del pin actual (pulsado o no pulsado)
int button_pressed = buttons_pressed;//mira si ha sido pulsado hace poco
if (button_state == HIGH & button_pressed == 0) { //comprueba si el boton ha sido pulsado Y si no ha sido pulsado aun(debounce)
MIDI.sendControlChange(buttons_CC_numbers,buttons_CC_values,buttons_CC_channels); //envia midi CC
led_send_on_off (); //ejecuta funcion que enciende y apaga led send
buttons_pressed = 1;delay(buttons_threshold); buttons_pressed = 0; //marca como enviado (debounce) -> espera un tiempo para volver a dejarlo en 1 (debounce) -> vuelve a marcar como no pulsado para poder pulsar de nuevo (debounce)
}//end if
}//end for
//POTENCIOMETROS
for (int i =0; i < pots_number; i++) { //recorre todos los potenciomentros
pots_analog_value = analogRead(i); //lectura del potenciometro en rango de 0 to 1023
if( abs(pots_analog_value-pots_last_analog_value) > pots_threshold_noise){//Mira si ha cambiado el valor del pote = si ha sido movido / con filtrado (se pierde precision pero solo se cumple si hay mas de x de diferencia en el cambio, asi se evita que cambie si hay "baile" de datos mientras el pot esta quieto realmente)
int pot_cc_value = pots_analog_value/8;//convierte a rango MIDI 0 - 127 de forma "casera"
//int pot_cc_value = map(pots_last_analog_value, 0, 1023, 0, 127); //convierte a rango MIDI 0 - 127 usando la funcion map (da fallos, a veces se dispara valor a 127 o se queda entre 125 y 2))
if (i == pedal_expresion_standard_pin && pedal_expresion_standard_reverse == 1){ //si es el pedal de expresion y esta en reverse
MIDI.sendControlChange(pots_cc_numbers,abs(pot_cc_value-127),pots_cc_channels); //envia el comando midi actual INVERTIDO
}
else{ //sino envio normal
MIDI.sendControlChange(pots_cc_numbers,pot_cc_value,pots_cc_channels); //envia el comando midi actual
}
led_send_on_off (); //ejecuta funcion que hace parpadear led send
pots_last_analog_value = pots_analog_value; //actualiza la ultima lectura
delay(pots_threshold_delay); //Retraso para evitar pequeñas flutuaciones al tocar o golpear potes
}//end if
}//end for
//SENSOR HALL/PEDAL EXPRESION
//Valor en bruto
int sensor_hall_value = analogRead(sensor_hall_pin);//measure magnetic field, poner "= 0;//" para desactivar pedal temporalmente
//Valores en MIDI logaritmico/natural
int sensor_hall_MIDI_value = map(sensor_hall_value, sensor_hall_min_value, sensor_hall_max_value, 0, 127);//Valor MIDI
sensor_hall_MIDI_value = constrain(sensor_hall_MIDI_value, 0, 127);//Limita el valor a MIDI 0-127, por si acaso se mueve iman y/o sensor
int sensor_hall_MIDI_value_inverted = abs(sensor_hall_MIDI_value-127);//Valor MIDI invertido
//Valores en MIDI Lineal
int rango = sensor_hall_max_value-sensor_hall_min_value; //rango total de valores en que se mueve el sensor
int out[] = {sensor_hall_min_value,(sensor_hall_min_value+(rango*0.25)),(sensor_hall_min_value+(rango*0.50)),(sensor_hall_min_value+(rango*0.75)),sensor_hall_max_value};//valores lineales deseados, calculado en 5 puntos (0%,25%,50%,75%,100%) / + puntos = + preciso.
int in[] = {sensor_hall_min_value,sensor_hall_25_value,sensor_hall_50_value,sensor_hall_75_value,sensor_hall_max_value}; //valores logaritmicos reales correspondientes a los deseados anteriores
int sensor_hall_lineal_value = multiMap(sensor_hall_value, in, out, 5); // Valor lineal
int sensor_hall_lineal_MIDI_value = map(sensor_hall_lineal_value, sensor_hall_min_value, sensor_hall_max_value, 0, 127); //Valor Lineal MIDI
sensor_hall_lineal_MIDI_value = constrain(sensor_hall_lineal_MIDI_value, 0, 127);//Limita el valor a MIDI 0-127, por si acaso se mueve iman y/o sensor
int sensor_hall_lineal_MIDI_value_inverted = abs(sensor_hall_lineal_MIDI_value-127);//Valor MIDI invertido
int sensor_hall_final_MIDI_value;//variable que tendra el valor midi final dependiendo del modo seleccionado
if (sensor_hall_selected_mode == 1){sensor_hall_final_MIDI_value = sensor_hall_MIDI_value;} //modo 1 = logaritmico + natural
if (sensor_hall_selected_mode == 2){sensor_hall_final_MIDI_value = sensor_hall_MIDI_value_inverted;} // modo 2 = logaritmico + invertido
if (sensor_hall_selected_mode == 3){sensor_hall_final_MIDI_value = sensor_hall_lineal_MIDI_value;} // modo 3 = lineal + natural
if (sensor_hall_selected_mode == 4){sensor_hall_final_MIDI_value = sensor_hall_lineal_MIDI_value_inverted;} //modo 4 = lineal + invertido
if( abs(sensor_hall_value-sensor_hall_last_value) > sensor_hall_threshold_noise) { //Mira si ha cambiado el valor del sensor hall = si ha sido movido / con filtrado (se pierde precision pero solo se cumple si hay mas de x de diferencia en el cambio, asi se evita que cambie si hay "baile" de datos mientras el sensor esta quieto realmente)
MIDI.sendControlChange(sensor_hall_cc_number,sensor_hall_final_MIDI_value,sensor_hall_cc_channel); //envia midi CC
//MIDI.sendControlChange(sensor_hall_selected_mode,sensor_hall_selected_mode,sensor_hall_selected_mode);//test para saber el modo, descomenttar para ver en hairless el modo seleccionado
led_send_on_off (); //ejecuta funcion que hace parpadear led send
sensor_hall_last_value = sensor_hall_value; //actualiza ultimo valor
delay(sensor_hall_threshold_delay); //Retraso para evitar pequeñas flutuaciones al tocar ,soltar o golpear pedal.
}//end if
//Utiles calibracion, descomentar para ver en el monitor serie los datos para calibrar
/*Serial.print("Value reading: ");Serial.println(sensor_hall_value);
Serial.print("MIDI reading: ");Serial.println(sensor_hall_MIDI_value);
Serial.print("MIDI reading -: ");Serial.println(sensor_hall_MIDI_value_inverted);
Serial.print("Value reading lineal: ");Serial.println(sensor_hall_lineal_value);
Serial.print("MIDI reading lineal MIDI: ");Serial.println(sensor_hall_lineal_MIDI_value);
Serial.print("MIDI reading lineal MIDI -: ");Serial.println(sensor_hall_lineal_MIDI_value_inverted);
Serial.println();delay(1000);*/
}//end loop
//----Funciones globales----------------------------
void led_send_on_off (){ //funcion para encender el led send por breve tiempo y apagar
digitalWrite(led_send_pin,HIGH); //enciende LED
delay (led_send_time); // tiempo de parpadeo
digitalWrite(led_send_pin,LOW); //apaga LED
}
//Funcion multimap, para convertir valores de logaritmico a lineal -> https://playground.arduino.cc/Main/MultiMap/
int multiMap(int val, int* _in, int* _out, uint8_t size)
{
// take care the value is within range
// val = constrain(val, _in[0], _in[size-1]);
if (val <= _in[0]) return _out[0];
if (val >= _in[size-1]) return _out[size-1];
// search right interval
uint8_t pos = 1; // _in[0] allready tested
while(val > _in[pos]) pos++;
// this will handle all exact "points" in the _in array
if (val == _in[pos]) return _out[pos];
// interpolate in the right segment for the rest
return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}
