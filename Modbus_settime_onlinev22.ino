#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266WebServer.h>
#include <ModbusIP_ESP8266.h>
#include <vector>
#include <LittleFS.h>
#include <Adafruit_SSD1306.h>

// Inisialisasi OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "MS-PNP-P";
const char* password = "88888999";

// Inisialisasi ModbusIP
ModbusIP mb;

// GPIO untuk LED built-in
const int LED_PIN1 = 14; // GPIO14 (D5)
const int LED_PIN2 = 12; // GPIO12 (D6)
const int LED_PIN3 = 13; // GPIO13 (D7)
const int LED_PIN4 = 16; // GPIO15 (D0)
const int Touch_pin1 = 1; //GPIO1(TX)

const int COIL_LED1 = 50; //modbus addres 50
const int COIL_LED2 = 51; //modbus addres 51
const int COIL_LED3 = 52; //modbus addres 52
const int COIL_LED4 = 53; //modbus addres 53


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000);

// Web Server
ESP8266WebServer server(80);

struct Schedule {
  int startHour, startMinute;
  int endHour, endMinute;
};

//schedule masing" coil
std::vector<Schedule> schedules1;
std::vector<Schedule> schedules2;
std::vector<Schedule> schedules3;
std::vector<Schedule> schedules4;

void setup() {
  Serial.begin(115200);

    // Inisialisasi OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED tidak ditemukan!"));
    while (true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Tampilkan proses loading
  displayLoadingAnimation();

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected. IP Address: " + WiFi.localIP().toString());

  // Tampilkan informasi koneksi WiFi dan Modbus
  displayConnectedInfo();

  if (!LittleFS.begin()) {
    Serial.println("Gagal menginisialisasi LittleFS!");
    return;
  }

  pinMode(LED_PIN1, OUTPUT);
  digitalWrite(LED_PIN1, HIGH);
  pinMode(LED_PIN2, OUTPUT);
  digitalWrite(LED_PIN2, HIGH);
  pinMode(LED_PIN3, OUTPUT);
  digitalWrite(LED_PIN3, HIGH);
  pinMode(LED_PIN4, OUTPUT);
  digitalWrite(LED_PIN4, HIGH);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi...");
  }
  Serial.println("WiFi Tersambung!");

  timeClient.begin();
  server.on("/", handleWebPage);
  server.on("/set_time", handleSetTime);
  server.on("/delete_time", handleDeleteTime);
  server.on("/time", handleTime);
  server.on("/coil_status", handleCoilStatus);
  server.on("/coil_status2", handleCoilStatus2);
  server.on("/coil_status3", handleCoilStatus3);
  server.on("/coil_status4", handleCoilStatus4);

  mb.server();
  mb.addCoil(COIL_LED1);
  mb.addCoil(COIL_LED2);
  mb.addCoil(COIL_LED3);
  mb.addCoil(COIL_LED4);
  server.begin();

  loadSchedules();
  Serial.println("Server dimulai!");
}

void loop() {
  timeClient.update();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentTotalMinutes = currentHour * 60 + currentMinute;

  // Cek status untuk setiap coil secara independen
  bool isActive1 = false;
  bool isActive2 = false;
  bool isActive3 = false;
  bool isActive4 = false;

  for (auto& schedule : schedules1) {
    int startTotalMinutes = schedule.startHour * 60 + schedule.startMinute;
    int endTotalMinutes = schedule.endHour * 60 + schedule.endMinute;
    if ((startTotalMinutes <= endTotalMinutes && currentTotalMinutes >= startTotalMinutes && currentTotalMinutes < endTotalMinutes) ||
        (startTotalMinutes > endTotalMinutes && (currentTotalMinutes >= startTotalMinutes || currentTotalMinutes < endTotalMinutes))) {
      isActive1 = true;
      break;
    }
  }

  for (auto& schedule : schedules2) {
    int startTotalMinutes = schedule.startHour * 60 + schedule.startMinute;
    int endTotalMinutes = schedule.endHour * 60 + schedule.endMinute;
    if ((startTotalMinutes <= endTotalMinutes && currentTotalMinutes >= startTotalMinutes && currentTotalMinutes < endTotalMinutes) ||
        (startTotalMinutes > endTotalMinutes && (currentTotalMinutes >= startTotalMinutes || currentTotalMinutes < endTotalMinutes))) {
      isActive2 = true;
      break;
    }
  }

  for (auto& schedule : schedules3) {
    int startTotalMinutes = schedule.startHour * 60 + schedule.startMinute;
    int endTotalMinutes = schedule.endHour * 60 + schedule.endMinute;
    if ((startTotalMinutes <= endTotalMinutes && currentTotalMinutes >= startTotalMinutes && currentTotalMinutes < endTotalMinutes) ||
        (startTotalMinutes > endTotalMinutes && (currentTotalMinutes >= startTotalMinutes || currentTotalMinutes < endTotalMinutes))) {
      isActive3 = true;
      break;
    }
  }

  for (auto& schedule : schedules4) {
    int startTotalMinutes = schedule.startHour * 60 + schedule.startMinute;
    int endTotalMinutes = schedule.endHour * 60 + schedule.endMinute;
    if ((startTotalMinutes <= endTotalMinutes && currentTotalMinutes >= startTotalMinutes && currentTotalMinutes < endTotalMinutes) ||
        (startTotalMinutes > endTotalMinutes && (currentTotalMinutes >= startTotalMinutes || currentTotalMinutes < endTotalMinutes))) {
      isActive4 = true;
      break;
    }
  }

  // Atur status masing-masing coil
  mb.Coil(COIL_LED1, isActive1);
  digitalWrite(LED_PIN1, isActive1 ? LOW : HIGH);

  mb.Coil(COIL_LED2, isActive2);
  digitalWrite(LED_PIN2, isActive2 ? LOW : HIGH);

  mb.Coil(COIL_LED3, isActive3);
  digitalWrite(LED_PIN3, isActive3 ? LOW : HIGH);

  mb.Coil(COIL_LED4, isActive4);
  digitalWrite(LED_PIN4, isActive4 ? LOW : HIGH);

  mb.task();
  server.handleClient();
  clockOLED();
}

void saveSchedules() {//save masing" schedule
  File file = LittleFS.open("/schedule.txt", "w");
  if (!file) {
    Serial.println("Gagal menyimpan jadwal!");
    return;
  }

  for (auto& schedule : schedules1) {
    file.printf("1 %d %d %d %d\n", schedule.startHour, schedule.startMinute, schedule.endHour, schedule.endMinute);
  }
  for (auto& schedule : schedules2) {
    file.printf("2 %d %d %d %d\n", schedule.startHour, schedule.startMinute, schedule.endHour, schedule.endMinute);
  }
  for (auto& schedule : schedules3) {
    file.printf("3 %d %d %d %d\n", schedule.startHour, schedule.startMinute, schedule.endHour, schedule.endMinute);
  }
  for (auto& schedule : schedules4) {
    file.printf("4 %d %d %d %d\n", schedule.startHour, schedule.startMinute, schedule.endHour, schedule.endMinute);
  }

  file.close();
}

void loadSchedules() {
  File file = LittleFS.open("/schedule.txt", "r");
  if (!file) {
    Serial.println("File schedule.txt tidak ditemukan.");
    return;
  }

  schedules1.clear();
  schedules2.clear();
  schedules3.clear();
  schedules4.clear();
  
  while (file.available()) {
    String line = file.readStringUntil('\n');
    Schedule sched;
    int coil;
    
    char buf[25];  // Buffer cukup besar untuk format "c hh mm hh mm"
    line.toCharArray(buf, sizeof(buf));

    if (sscanf(buf, "%d %d %d %d %d", &coil, &sched.startHour, &sched.startMinute, &sched.endHour, &sched.endMinute) == 5) {
      switch (coil) {
        case 1: schedules1.push_back(sched); break;
        case 2: schedules2.push_back(sched); break;
        case 3: schedules3.push_back(sched); break;
        case 4: schedules4.push_back(sched); break;
        default: Serial.println("ID coil tidak valid dalam file."); break;
      }
    }
  }

  file.close();
}


//calling coil
String getCoilStatus() {
  return mb.Coil(COIL_LED1) ? "Aktif" : "Nonaktif";
}
String getCoilStatus2() {
  return mb.Coil(COIL_LED2) ? "Aktif" : "Nonaktif";
}
String getCoilStatus3() {
  return mb.Coil(COIL_LED3) ? "Aktif" : "Nonaktif";
}
String getCoilStatus4() {
  return mb.Coil(COIL_LED4) ? "Aktif" : "Nonaktif";
}

void handleCoilStatus() {
  server.send(200, "text/plain", getCoilStatus());
}
void handleCoilStatus2() {
  server.send(200, "text/plain", getCoilStatus2());
}
void handleCoilStatus3() {
  server.send(200, "text/plain", getCoilStatus3());
}
void handleCoilStatus4() {
  server.send(200, "text/plain", getCoilStatus4());
}

String getTimeString() {
  timeClient.update();
  return String(timeClient.getHours()) + ":" + String(timeClient.getMinutes()) + ":" + String(timeClient.getSeconds());
}

void handleTime() {
  server.send(200, "text/plain", getTimeString());
}


void handleSetTime() {
  if (server.hasArg("coil") && server.hasArg("startHour") && server.hasArg("startMinute") && 
      server.hasArg("endHour") && server.hasArg("endMinute")) {
      
    int coil = server.arg("coil").toInt();
    Schedule newSchedule = {
      server.arg("startHour").toInt(), server.arg("startMinute").toInt(),
      server.arg("endHour").toInt(), server.arg("endMinute").toInt()
    };

    switch (coil) {
      case 1: schedules1.push_back(newSchedule); break;
      case 2: schedules2.push_back(newSchedule); break;
      case 3: schedules3.push_back(newSchedule); break;
      case 4: schedules4.push_back(newSchedule); break;
      default: Serial.println("Coil tidak valid."); return;
    }

    saveSchedules(); // Simpan ke file
    Serial.println("Jadwal ditambahkan.");
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDeleteTime() {
  if (server.hasArg("coil") && server.hasArg("id")) {
    int coil = server.arg("coil").toInt();
    int id = server.arg("id").toInt();
    
    switch (coil) {
      case 1: if (id >= 0 && id < schedules1.size()) schedules1.erase(schedules1.begin() + id); break;
      case 2: if (id >= 0 && id < schedules2.size()) schedules2.erase(schedules2.begin() + id); break;
      case 3: if (id >= 0 && id < schedules3.size()) schedules3.erase(schedules3.begin() + id); break;
      case 4: if (id >= 0 && id < schedules4.size()) schedules4.erase(schedules4.begin() + id); break;
      default: Serial.println("Coil tidak valid."); return;
    }
    
    saveSchedules();
    Serial.println("Jadwal dihapus.");
  }
  server.sendHeader("Location", "/");
  server.send(303);
}



