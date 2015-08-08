#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"
// Mac unica de cada EthernetShield (deben cambiarla)
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x4E, 0xD7 };

// Descomentar esta linea si se desea un IP fijo
//IPAddress ip(192,168,239,162);

// Descomentar esta linea para asignar un DNS fijo
//IPAddress myDns(99,209,172,101);

// Inicializa la instancia client
EthernetClient client;

// Direccion del servidor
char server[] = "www.desarrollo.mads96.cl";

// Variable de tiempo de la ultima conexion en milisegundos
unsigned long ultimaConexion = 0;          
// Estado de la ultima conexion
boolean ultimoEstado = false;
// Intervalo en milisegundos entre conexiones
const unsigned long intervaloConexion = 1000;   
/////////////////////////////////////////conexion/////////////////////////////////
// Datos del sensor

DHT dht1(2,DHT22); //sensor 1

 //fin DHT
float h1;
float t1;

float tierra1;
float tierrax1;

void setup() {
  // Inicializa puerto serial
  Serial.begin(9600);
  Serial.println("#### Desarrollo MADS96.cl ####");
  Serial.println("Monitoreo ARDUINO");
  // Espera 1 segundo para que se inicie la tarjeta Ethernet
  delay(1000);
  // Si deseas usar un ip fijo y un DNS fijo descomentar esta linea y comentar la linea 39
   //Ethernet.begin(mac, ip);//para dns agregarn(mac, ip, MyDns)
  // Inicializa la tarjeta ethernet mediante DHCP
  Ethernet.begin(mac);
  // Imprime la direccion IP de la tarjeta
  Serial.print("Direccion IP: ");
  Serial.println(Ethernet.localIP());
}
// Loop principal
void loop() {
  //DHT
   delay(2000);
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
 
  tierrax1 = analogRead(0);
  tierra1=((100*tierrax1)/1024);
  
  if (isnan(h1) || isnan(t1)) {
    Serial.println("Falla al leer el sensor DHT!");
    }
  //fin DHT
  
  // Si hay datos que llegan por la conexion los envia a la puerta serial
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // Si no hay conexion de red y se conecto correctamente la ultima vez
  // detiene el cliente Ehternet
  if (!client.connected() && ultimoEstado) {
    Serial.println();
    Serial.println("Desconectando...");
    client.stop();
  }

  // Si no esta conectado y han pasado X segundos (intervaloConexion) 
  // despues de la ultima conexion envia datos al servidor
  if(!client.connected() && (millis() - ultimaConexion > intervaloConexion)) {
    httpRequest();
  }
  // Actualiza la variable ultimoEstado 
  ultimoEstado = client.connected();
}
// Fin del loop principal

// Realiza la conexion http al servidor
void httpRequest() {
  // Se conecta al servidor en el puerto 80 (web)
  if (client.connect(server, 80)) {
    // Envia el dato al puerto serial 
/////////////////////////////////////////////////MONITOREO POR CONSOLA!!//////////////////////////////////////
    Serial.print("GET /sensorarduino.php");
Serial.print("?iduser=01");
Serial.print("&t1=15");
//client.print(t1);
    Serial.print("&h1=14");
  //  client.print(h1);
      Serial.print("&t2=13");
    //  client.print(t2);
        Serial.print("&h2=12");
      //  client.print(h2);
          Serial.print("&ht1=11");
        //  client.print(tierra1);
            Serial.print("&ht2=10");
          //   client.print(tierra2);
    Serial.println(" HTTP/1.1");
    Serial.print("Host: ");
    Serial.println(server);
    Serial.println("User-Agent: Arduino-Ethernet");
    Serial.println("Connection: close");
    Serial.println();

  //FIN MONITOREO POR CONSOLA!!//////////////////////////////////////
    
    // Envia el requerimiento al servidor via GET
    Serial.println("Iniciando conexion...");
   client.print("GET /sensorarduino.php");
  
  client.print("?iduser=01");

  client.print("&t1=");
  client.print(t1);

  client.print("&h1=");
  client.print(h1);
    
  client.print("&ht1=");
  client.print(tierra1);
    
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("User-Agent: Arduino-Ethernet");
    client.println("Connection: close");
    client.println();

    // Actualiza el tiempo en milisegundos de la ultima conexion
    ultimaConexion = millis();
  } 
  else {
    // Si la conexion fallo se desconecta
    Serial.println("Error al conectarse al servidor");
    Serial.println("Desconectando...");
    client.stop();
  }
}
