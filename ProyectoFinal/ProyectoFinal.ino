#include <EEPROM.h>
#include <Servo.h>

const int analogPin = A5; // Pin del potenciómetro
const int switchPin = 8;  // Pin del interruptor (switch)
const int buttonPin = 10; // Pin del botón
Servo myservo;

int value = 0;//valor leido  del potenciometro
int position = 0;//posocion del servo
int nk = 0; // Índice para la EEPROM escribir durante la grabacion
int k = 0;  // Índice para leer desde la EEPROM reproduccion
int g;      // Estado del interruptor
int sAct, sAnt; // Estado del botón, actual y anterior
bool recording = false;
bool repro = false;
unsigned long uno = 0, dos = 0, lapso= 150, total = 60000;// Tiempo del inicio de la grabación
//uno - almacena tiempo de inicio de grabacion/reproduccion
// dos - maneja el lapso entra grabaciones/reproducciones consecutivas
void setup() {
  myservo.attach(9);
  pinMode(buttonPin, INPUT);
  pinMode(switchPin, INPUT);
  Serial.begin(9600);
}

void loop(){
  g = digitalRead(switchPin); // Leer el estado del interruptor
  sAnt = sAct;//actualizan el estado del boton
  sAct = digitalRead(buttonPin);
  bool pb = sAct != sAnt && sAct == 1;//determina si el boton ha sido presionado
  value = analogRead(analogPin);//lee el valor del potenciometro
  position = map(value, 0, 1023, 0, 180);//mapea el valor a un rango de 0 a 180 grados
  myservo.write(position);
  if (g == 1 && pb) { // Manejo de la grabación
    if (!recording) {
      repro = false;
      recording = true;
      uno = millis();
      dos = millis();
      nk = 0;//para comenzar a escribir desde el inicio
      Serial.println("Inicio de grabacion");
    } else {
      recording = false;
      Serial.println("Grabacion detenida manualmente");
    }
  }
  if(g == 0 && pb){//reproduccion
    if(!repro){
    	recording = false;
    	k = 0;//para comenzar a leer desde el inicio
      	uno = millis();
      	dos = millis();
    	repro = true;
    	Serial.println("Inicio de reproduccion");
    }else{
    	repro = false;
      	Serial.println("Reproduccion detenida manualmente");
    }
    
  }
  //Grabar o reproducir
  if(recording){
  	if(millis() - uno < total ){//verificacion de que no ha excedido el total
      if(millis() - dos >= lapso){//si ha pasado el lapso entre grabaciones
        // Guardar posición en EEPROM
        EEPROM.put(nk, position);//se guarda la posocion actual en la eeprom y se actualiza el indice nk
        Serial.println((String)"Posicion: " + position + " guardada en dir: " + nk);
        nk += sizeof(int);//para la siguiente escritura
		dos = millis();//se actualiza para medir el siguiente lapso
        k = 0;
      }
  	}else{
  		Serial.println("Grabacion detenida por tiempo");
      	//grabado = uno;
    	recording = false;
  	}
  }
  
  if(repro){
    if(k < nk){//verifica si hay mas posiciones para leer
        // Revisar posición en EEPROM
        EEPROM.get(k, position);
        Serial.println((String)"Posicion: " + position + " leida de dir: " + k);
        myservo.write(position);
        k += sizeof(int);
      	delay(150);
  	}else{
  		Serial.println("Reproduccion detenida por tiempo");
    	//grabado = uno;
      	repro = false;
  	}
  }
}