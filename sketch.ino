/*
  ESP32 - IoT Chair Sensor for BibFlip
  Syncs with Edge API and Backend
  CONFIGURADO PARA: DEVICE_005 + CUBICLE_ID 3
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Config WiFi
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// Config API - Edge API URL (CAMBIADO A AZURE)
const char* EDGE_API_BASE = "https://bibflip-edge-api-platform.azurewebsites.net/api/v1/devices";

// DEVICE_005 ya está registrado y asignado a cubicle_id 3
const int CUBICLE_ID = 3;
const String DEVICE_ID = "DEVICE_005";  // ← CAMBIADO de DEVICE_006

// Hardware
const int FSR = 34;
const int LED = 5;
const int UMBRAL = 30;

// Variables
bool wifi_ok = false;
bool registrado = false;
int presion = 0;
bool ocupado = false;
unsigned long t_lect = 0;
unsigned long t_env = 0;

// Intervalos
const int LEER_CADA = 100;      // Leer sensor cada 100ms
const int ENVIAR_CADA = 2000;   // Enviar cada 2 segundos

void setup() {
  Serial.begin(115200);
  delay(500);
  
  Serial.println("\n========================================");
  Serial.println("ESP32 BibFlip Chair Sensor");
  Serial.println("========================================");
  Serial.print("Cubicle ID: ");
  Serial.println(CUBICLE_ID);
  Serial.print("Device ID: ");
  Serial.println(DEVICE_ID);
  Serial.println("Edge API: Azure (HTTPS)");
  Serial.println("========================================");
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  
  // WiFi connection
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.setAutoReconnect(true);
  
  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED && cnt < 20) {
    delay(250);
    Serial.print(".");
    cnt++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    wifi_ok = true;
    Serial.println("WiFi Connected");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("========================================");
    
    // Register device (o verificar si ya existe)
    delay(1000);
    registrar();
  } else {
    Serial.println("WiFi Connection Failed");
    Serial.println("========================================");
  }
  
  t_lect = millis();
  t_env = millis();
}

void registrar() {
  if (!wifi_ok) return;
  
  Serial.println("\n========================================");
  Serial.println("Checking Device Registration in Edge API");
  Serial.println("========================================");
  
  HTTPClient http;
  String url = String(EDGE_API_BASE) + "/register";
  
  // Para HTTPS en Wokwi, necesitamos configurar certificados
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(20000);
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  
  String json = "{\"device_id\":\"" + DEVICE_ID + 
                "\",\"device_type\":\"chair_sensor\"," +
                "\"branch_id\":\"BRANCH_001\"," +
                "\"zone\":\"ZONE_A\"," +
                "\"position\":\"CUBICLE_" + String(CUBICLE_ID) + "\"}";
  
  Serial.print("URL: ");
  Serial.println(url);
  Serial.print("Payload: ");
  Serial.println(json);
  Serial.println("Sending request...");
  
  int code = http.POST(json);
  
  Serial.print("Response Code: ");
  Serial.println(code);
  
  if (code == 201) {
    registrado = true;
    Serial.println("✓ Device registered successfully (NEW)");
  } else if (code == 200) {
    registrado = true;
    Serial.println("✓ Device already exists (USING EXISTING)");
  } else if (code == 400) {
    registrado = true;
    Serial.println("✓ Device exists (proceeding anyway)");
  } else {
    Serial.print("✗ Registration failed with code: ");
    Serial.println(code);
    String response = http.getString();
    Serial.print("Response: ");
    Serial.println(response);
  }
  
  Serial.println("========================================\n");
  http.end();
}

void enviar(int val) {
  if (!wifi_ok || !registrado) return;
  
  HTTPClient http;
  String url = String(EDGE_API_BASE) + "/" + DEVICE_ID + "/readings";
  
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);
  
  String json = "{\"pressure\":" + String(val) + 
                ",\"threshold\":" + String(UMBRAL) + "}";
  
  int code = http.POST(json);
  
  Serial.print("Pressure: ");
  Serial.print(val);
  Serial.print("% | Status: ");
  Serial.print(val >= UMBRAL ? "OCCUPIED" : "AVAILABLE");
  Serial.print(" | Response: ");
  Serial.println(code == 200 ? "OK" : "FAIL");
  
  if (code != 200) {
    String response = http.getString();
    Serial.print("Error response: ");
    Serial.println(response);
  }
  
  http.end();
}

void loop() {
  unsigned long now = millis();
  
  // Leer sensor frecuentemente
  if (now - t_lect >= LEER_CADA) {
    t_lect = now;
    
    // Leer y mapear
    int adc = analogRead(FSR);
    presion = map(adc, 0, 4095, 0, 100);
    
    // Suavizado
    static int prev = 0;
    presion = (presion * 0.8) + (prev * 0.2);
    prev = presion;
    
    bool nuevo = (presion >= UMBRAL);
    
    // Detectar cambio de estado
    if (nuevo != ocupado) {
      ocupado = nuevo;
      
      digitalWrite(LED, ocupado ? HIGH : LOW);
      
      Serial.println("\n========================================");
      Serial.print("STATUS CHANGE: ");
      Serial.println(ocupado ? "OCCUPIED" : "AVAILABLE");
      Serial.print("Pressure: ");
      Serial.print(presion);
      Serial.println("%");
      Serial.print("Cubicle ID: ");
      Serial.println(CUBICLE_ID);
      Serial.println("========================================");
      
      // Enviar inmediatamente al cambiar
      enviar(presion);
    }
  }
}