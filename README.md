# BibFlip - Sistema IoT de Monitoreo de Cubículos

**BibFlip** es un sistema inteligente de gestión de espacios de estudio en bibliotecas que utiliza sensores IoT para detectar en tiempo real la disponibilidad de cubículos, optimizando el uso del espacio y mejorando la experiencia del usuario.

## 🔗 Simulación Wokwi

**Downloaded from:** https://wokwi.com/projects/446107411757389825

**Simulate this project on:** https://wokwi.com

---

## 🎯 User Stories Implementadas

### TS004: Implementar Firmware para Dispositivo IoT
**Como Developer**, quiero desarrollar el firmware para el dispositivo IoT que detecta la ocupación de cubículos, para transmitir datos al Edge API.

**Escenarios:**
- **Lectura de sensor correcta**: Cuando el sensor detecta presencia, el firmware genera un paquete de datos con el estado de ocupación y lo transmite al Edge API
- **Fallo de lectura del sensor**: En caso de error de hardware, el firmware registra el error sin transmitir datos

---

### TS005: Integrar Firmware con Plataforma Embebida
**Como Developer**, quiero integrar el firmware en la plataforma embebida para que pueda iniciar, procesar datos y comunicarse vía red con el Edge.

**Escenarios:**
- **Inicio correcto del sistema**: El sistema embebido arranca con configuración válida, inicializa sensores y red sin errores
- **Error de red**: Si la conexión de red falla, se lanza un mensaje de error y se reinicia el intento

---

### TS006: Implementar Recepción de Datos en Edge API
**Como Developer**, quiero implementar la lógica en el Edge API que reciba los datos enviados por los sensores IoT, para enviarlos al backend.

**Escenarios:**
- **Datos recibidos correctamente**: El Edge API recibe el paquete, lo reenvía al backend y responde con 200 OK
- **Datos con formato incorrecto**: Si el formato es inválido, el sistema registra el error y descarta el paquete

---

### TS007: Simular Sensores en Wokwi
**Como Developer**, quiero simular el comportamiento del dispositivo IoT en Wokwi, para validar la lectura del sensor y la comunicación con el Edge API.

**Escenarios:**
- **Simulación de detección de ocupación**: El circuito simulado en Wokwi detecta peso y envía correctamente el estado al Edge API
- **Sensor inactivo**: Cuando el sensor no detecta peso, no se envía ninguna actualización

---

## 🛠️ Arquitectura del Sistema

```
┌─────────────┐      ┌──────────────┐      ┌─────────────┐
│   ESP32     │─────▶│  Edge API    │─────▶│  Backend    │
│ (IoT Sensor)│      │ (FastAPI)    │      │ (Spring)    │
│  + FSR      │      │ PostgreSQL   │      │             │
└─────────────┘      └──────────────┘      └─────────────┘
```

### Componentes:
- **ESP32**: Microcontrolador con sensor de presión FSR
- **Edge API**: API intermedia para procesamiento de datos IoT
- **Backend**: Sistema principal de gestión de reservas

---

## 📋 Configuración del Dispositivo

### Hardware:
- **Sensor FSR**: Pin GPIO 34 (ADC)
- **LED indicador**: Pin GPIO 5
- **Umbral de ocupación**: 30% de presión

### Configuración de Red:
```cpp
const char* WIFI_SSID = "Wokwi-GUEST";
const char* EDGE_API_BASE = "https://bibflip-edge-api-platform.azurewebsites.net/api/v1/devices";
```

### Dispositivo:
- **Device ID**: DEVICE_005
- **Cubicle ID**: 3
- **Device Type**: chair_sensor

---

## 🚀 Funcionamiento

1. **Conexión WiFi**: El ESP32 se conecta a la red WiFi configurada
2. **Registro**: Se registra en el Edge API (o verifica si ya existe)
3. **Monitoreo continuo**: Lee el sensor FSR cada 100ms
4. **Detección de cambios**: Cuando detecta cambio de estado (ocupado/disponible)
5. **Transmisión**: Envía los datos al Edge API vía POST
6. **Sincronización**: El Edge API actualiza el estado en el backend
7. **Indicador visual**: LED encendido = ocupado, LED apagado = disponible

---

## 📊 API Endpoints Utilizados

### Registro del dispositivo:
```http
POST /api/v1/devices/register
{
  "device_id": "DEVICE_005",
  "device_type": "chair_sensor",
  "branch_id": "BRANCH_001",
  "zone": "ZONE_A",
  "position": "CUBICLE_3"
}
```

### Envío de lecturas:
```http
POST /api/v1/devices/DEVICE_005/readings
{
  "pressure": 45.2,
  "threshold": 30.0
}
```

### Consulta de estado:
```http
GET /api/v1/devices/DEVICE_005
```

---

## 🧪 Testing

### En Wokwi:
1. Abrir el proyecto en https://wokwi.com
2. Iniciar la simulación
3. Hacer clic en el sensor FSR para simular presión
4. Observar el LED cambiar de estado

### Con curl:
```bash
# Verificar estado del dispositivo
curl https://bibflip-edge-api-platform.azurewebsites.net/api/v1/devices/DEVICE_005

# Simular lectura manual
curl -X POST https://bibflip-edge-api-platform.azurewebsites.net/api/v1/devices/DEVICE_005/readings \
  -H "Content-Type: application/json" \
  -d '{"pressure": 75.0, "threshold": 30.0}'
```

---

## 📝 Indicadores Visuales del LED

- **LED encendido fijo**: Cubículo ocupado (presión ≥ 30%) 🔴
- **LED apagado**: Cubículo disponible (presión < 30%) ⚫

---

## 🔧 Tecnologías

- **Hardware**: ESP32, Sensor FSR (Force Sensitive Resistor)
- **Firmware**: C++ (Arduino Framework)
- **Comunicación**: HTTP/HTTPS, JSON
- **Edge API**: FastAPI (Python)
- **Base de datos**: PostgreSQL
- **Simulación**: Wokwi

---

## 📖 Documentación Adicional

- **Edge API Docs**: https://bibflip-edge-api-platform.azurewebsites.net/docs
- **Wokwi Docs**: https://docs.wokwi.com
- **ESP32 Docs**: https://docs.espressif.com

---

## 📄 Licencia

© 2025 BibFlip - Sistema de Gestión de Espacios de Estudio