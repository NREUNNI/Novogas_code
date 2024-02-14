#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SmartValve.h>

SmartValve &*smartValve = SmartValve::getInstance('gggg');

// Configuración de la red WiFi
const char *ssid = "";
const char *password = "";
const char *mqtt_server = "";   // Reemplaza con la dirección de tu broker MQTT
const int mqtt_port = 12676;    // Puerto predeterminado
const char *mqtt_username = ""; // Cambia esto por el nombre de usuario de tu broker MQTT
const char *mqtt_password = ""; // Cambia esto por la contraseña de tu broker MQTT

const char *ID_DIVICE = smartValve.getId(); // ID unico para cada dispositivo

// Iniciar dispositivo en nube
const char *inizializate = "request/inizializate";

String inizializate_response_topic = "response/inizializate/" + String(ID_DIVICE);
const char *inizializate_response = inizializate_response_topic.c_str();

// Tópicos de suscripción
String respuesta_topic = "response/param/" + String(ID_DIVICE);
const char *respuesta = respuesta_topic.c_str();
const char *validacion = "response/consumption";

// Tópicos de publicación
const char *consulta = "request/param";     // enviamos el qr =aaaa
const char *send_data = "send/consumption"; // envío datos flujo, flujo acumulado y tempertura
const char *off_valve = "request/off"; // apagar el dispositivo (IMPORTATE MANDARLO A APAGAR CUANDO YA NO HAY FLUJO DE CONSUMO)

// Tópicos de inicialización

// variables de maquina de estado
bool star_used = true;
bool used_validate = false;
bool running_status = false;
bool close_status = false;

// variables de actualización: se recibe desde la DB para realizar ponderaciones y comprobación

WiFiClient espClient;
PubSubClient client(espClient);

// Función de callback para manejar mensajes MQTT
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensaje recibido en el tópico: ");
  Serial.println(topic);
  Serial.print("Contenido del mensaje: ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }



  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Convertir el payload a String
  String payloadStr = String((char *)payload);

  if (strcmp(topic, respuesta) == 0)
  {
    // Crear un objeto JSON para analizar el payload
    DynamicJsonDocument doc(256); // Ajusta el tamaño según tus necesidades
    // Deserializar el payload JSON
    DeserializationError error = deserializeJson(doc, payloadStr);
    // Verificar errores en la deserialización
    if (error)
    {
      Serial.print("Error al deserializar el JSON: ");
      Serial.println(error.c_str());
      return;
    }
    // Obtener valores del JSON

    if (topic == = inizializate_response)
    {
    smartValve.init();
    }
    bool canBeUsed = doc["data"]["canBeUsed"].as<bool>();
    double balance = doc["data"]["balace"].as<double>();

    const char *availableCapacity = doc["availableCapacity"];
    const char *capacity = doc["capacity"];
    const char *porcent = doc["porcent"];

    // Imprimir los valores
    Serial.print("canBeUsed: ");
    Serial.println(canBeUsed);
    smartValve.setCanBeUsed(canBeUsed);
    Serial.print("balace: ");
    Serial.println(balance);
    smartValve.setBalance(balance);
    Serial.print("Tipo de tópico: ");
    Serial.println(topic);
    delay(1000);

    used_validate = canBeUsed; // Modifica la variable global
  }
}

void setup()
{
  Serial.begin(9600);

  // Conexión a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conexión exitosa a WiFi");

  // Configuración del cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Conexión al broker MQTT
  while (!client.connected())
  {
    if (client.connect("ESP32_Client", mqtt_username, mqtt_password))
    {
      Serial.println("Conectado al broker MQTT");
      // Suscripción a los cuatro tópicos de interés
      client.subscribe(inizializate_response);
      client.subscribe(respuesta);
      client.subscribe(validacion);
    }
    else
    {
      Serial.println("Error en la conexión al broker MQTT, intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void loop()
{
  // si no es false
  if (!smartValve.isInicialized)
  {
    DynamicJsonDocument doc(256); // Ajusta el tamaño según tus necesidades
    // doc["qr"] = smartValve.getId(); // Aquí puedes cambiar "ID_DIVICE" por el valor deseado
    client.publish(inizializate, ID_DIVICE); // jsonMessage.c_str());
  }

  if (star_used == true && smartValve.isInicialized)
  {
    // Enviar mensaje al tópico "request/param"
    DynamicJsonDocument doc(256); // Ajusta el tamaño según tus necesidades
    doc["qr"] = ID_DIVICE;        // Aquí puedes cambiar "ID_DIVICE" por el valor deseado

    String jsonMessage;
    serializeJson(doc, jsonMessage);

    client.publish(consulta, ID_DIVICE); // jsonMessage.c_str());

    // Reiniciar la variable
    star_used = false;
  }

  // Manejo de eventos MQTT
  if (used_validate == true)
  {
    Serial.println("Enviando datos");
    running_status = true;
    used_validate = false;
    delay(5000);
  }
  int b = 10;
  while (running_status == true)
  {
    DynamicJsonDocument consumptionDoc(256);
    int a = 100;

    consumptionDoc["temp"] = 25.5;          // Cambia esto por el valor deseado
    consumptionDoc["consumption"] = 10;     // Cambia esto por el valor deseado
    consumptionDoc["consumptionTotal"] = b; // Cambia esto por el valor deseado
    // consumptionDoc["isOn"] = true;          // Cambia esto por el valor deseado
    consumptionDoc["qr"] = ID_DIVICE;       // Cambia esto por el valor deseado
    String consumptionJson;
    serializeJson(consumptionDoc, consumptionJson);
    client.publish(send_data, consumptionJson.c_str());
    b = b + 10;
    delay(5000); // Esperar 5 segundos antes de enviar el siguiente mensaje
  }
  client.loop();
}
