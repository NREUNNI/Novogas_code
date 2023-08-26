#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Martinez 2.4G";
const char* password = "lancenter2022";

void GET_metodo(String URL_GET);
void POST_metodo(String URL_POST);


bool con_init=0;
int capacidad =0;
String entrada_serial="";


void setup() {
  Serial.begin(9600);
  // Conéctate a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
}


void loop() {
  String url_post = "https://novogas-backend.up.railway.app/api/smart-valve/iot/update/aaa/";
  String url_get = "https://novogas-backend.up.railway.app/api/smart-valve/iot/aaa";

  bool con_init=0;
  int capacidad =0;

  String entrada_serial;

  if (Serial.available()>0)
  {
    entrada_serial=Serial.read();
  }
  if(entrada_serial="1"){
    GET_metodo(url_get);
    if(con_init==true && capacidad>0){
        Serial.println("medición de flujo iniciado !!!");
    }
  }
  
  
  POST_metodo(url_post);
  delay(2000);
}

void GET_metodo(String URL_GET) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    DynamicJsonDocument jsonDoc(1024);

    String url = URL_GET;

    http.begin(url);

    int httpCode = http.GET();

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Respuesta de la API GET:");
      Serial.println(response);

      // Deserializar la respuesta JSON
      DeserializationError error = deserializeJson(jsonDoc, response);
      if (error) {
        Serial.print("Error al deserializar JSON: ");
        Serial.println(error.c_str());
        return;
      }

      // Extraer las variables "canBeUsed" y "balace"
      bool canBeUsed = jsonDoc["result"]["canBeUsed"];
      const char* availableCapacity = jsonDoc["result"]["availableCapacity"];
      int disponible= atoi(availableCapacity);
      con_init=canBeUsed;
      capacidad=disponible;
      // Imprimir las variables extraídas
     // Serial.print("canBeUsed: ");
      //Serial.println(canBeUsed);
      //Serial.print("availableCapacity: ");
      //Serial.println(disponible);
    } else {
      Serial.println("Error en la solicitud GET");
    }

    http.end();
  }

  delay(5000);
}

void POST_metodo(String URL_POST) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    int lectura_sen=random(0,10);
    String url = URL_POST;

    DynamicJsonDocument jsonDoc(1024);
    jsonDoc["consumption"] = lectura_sen;
    jsonDoc["isOn"] = true;


    String jsonStr;
    serializeJson(jsonDoc, jsonStr);

    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(jsonStr);

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Respuesta de la API POST:");
      Serial.println(response);
    } else {
      Serial.println("Error en la solicitud POST");
    }

    http.end();
  }

  delay(5000);
}



/*
void desempaquta(char respuesta)
{
  // Tu JSON de ejemplo
  char json = respuesta;//"{\"message\":\"Iot result\",\"result\":{\"canBeUsed\":true,\"balace\":\"50.00\",\"availableCapacity\":\"9700\",\"capacity\":\"10000.00\",\"porcent\":\"97\"},\"timeStamp\":\"2023-08-21T03:27:59.757Z\",\"code\":200,\"error\":null}";

  // Crear un objeto JSON
  DynamicJsonDocument doc(1024);
  
  // Deserializar el JSON
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("Error al deserializar JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Extraer las variables
  bool canBeUsed = doc["result"]["canBeUsed"];
  const char* balance = doc["result"]["balace"];
  const char* availableCapacity = doc["result"]["availableCapacity"];

  // Imprimir las variables extraídas
  Serial.println("canBeUsed: " + String(canBeUsed));
  Serial.println("balance: " + String(balance));
  Serial.println("availableCapacity: " + String(availableCapacity));
}
*/