#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <BleKeyboard.h>
#include "config.h"

#define DNS_PORT 53
#define MAX_SCRIPTS 20

DNSServer dnsServer;
AsyncWebServer server(80);
BleKeyboard keyboard("T-Watch", "LilyGo", 100);
TTGOClass *ttgo;

bool serverActive = false;
bool bleConnected = false;
String scriptNames[MAX_SCRIPTS];
int scriptCount = 0;
String queuedScript = "";

void clearScreen() {
  ttgo->tft->fillScreen(TFT_BLACK);
}

void updateTimeDisplay() {
  RTC_Date rlc = ttgo->rtc->getDateTime();
  
  clearScreen();
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->setTextSize(4);
  
  int x = 60;
  int y = 104;
  ttgo->tft->setCursor(x, y);
  
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", rlc.hour, rlc.minute);
  ttgo->tft->print(timeStr);
}

void loadScriptList() {
  scriptCount = 0;
  File root = LittleFS.open("/scripts");
  if (!root || !root.isDirectory()) {
    return;
  }
  
  File file = root.openNextFile();
  while (file && scriptCount < MAX_SCRIPTS) {
    if (!file.isDirectory()) {
      String name = String(file.name());
      if (name.endsWith(".txt")) {
        scriptNames[scriptCount++] = name;
      }
    }
    file = root.openNextFile();
  }
}

String generateScriptListHTML() {
  String options = "";
  for (int i = 0; i < scriptCount; i++) {
    options += "<option value=\"" + scriptNames[i] + "\">" + scriptNames[i] + "</option>";
  }
  return options;
}

void pressKey(uint8_t key) {
  keyboard.press(key);
  delay(100);
  keyboard.releaseAll();
  delay(100);
}

void typeString(String text) {
  for (int i = 0; i < text.length(); i++) {
    keyboard.print(text.charAt(i));
    delay(50);
  }
}

uint8_t getKeyCode(String key) {
  key.toUpperCase();
  if (key == "ENTER" || key == "RETURN") return KEY_RETURN;
  if (key == "TAB") return KEY_TAB;
  if (key == "BACKSPACE") return KEY_BACKSPACE;
  if (key == "DELETE") return KEY_DELETE;
  if (key == "ESC" || key == "ESCAPE") return KEY_ESC;
  if (key == "SPACE") return ' ';
  if (key == "UP" || key == "UPARROW") return KEY_UP_ARROW;
  if (key == "DOWN" || key == "DOWNARROW") return KEY_DOWN_ARROW;
  if (key == "LEFT" || key == "LEFTARROW") return KEY_LEFT_ARROW;
  if (key == "RIGHT" || key == "RIGHTARROW") return KEY_RIGHT_ARROW;
  if (key == "HOME") return KEY_HOME;
  if (key == "END") return KEY_END;
  if (key == "PAGEUP") return KEY_PAGE_UP;
  if (key == "PAGEDOWN") return KEY_PAGE_DOWN;
  if (key == "CAPSLOCK") return KEY_CAPS_LOCK;
  if (key.length() == 1) return key.charAt(0);
  return 0;
}

void executeCommand(String cmd, String param) {
  cmd.toUpperCase();
  
  if (cmd == "STRING") {
    typeString(param);
  } else if (cmd == "DELAY") {
    delay(param.toInt());
  } else if (cmd == "ENTER") {
    pressKey(KEY_RETURN);
  } else if (cmd == "GUI" || cmd == "WINDOWS") {
    keyboard.press(KEY_LEFT_GUI);
    delay(100);
    if (param.length() > 0) {
      String upperParam = param;
      upperParam.toUpperCase();
      uint8_t key = getKeyCode(upperParam);
      if (key != 0) {
        keyboard.press(key);
        delay(100);
      }
      keyboard.releaseAll();
      delay(100);
    } else {
      keyboard.releaseAll();
      delay(50);
    }
  } else if (cmd == "ALT") {
    keyboard.press(KEY_LEFT_ALT);
    delay(100);
    if (param.length() > 0) {
      String upperParam = param;
      upperParam.toUpperCase();
      uint8_t key = getKeyCode(upperParam);
      if (key != 0) {
        keyboard.press(key);
        delay(100);
      }
      keyboard.releaseAll();
      delay(100);
    } else {
      keyboard.releaseAll();
      delay(50);
    }
  } else if (cmd == "CTRL" || cmd == "CONTROL") {
    keyboard.press(KEY_LEFT_CTRL);
    delay(100);
    if (param.length() > 0) {
      String upperParam = param;
      upperParam.toUpperCase();
      uint8_t key = getKeyCode(upperParam);
      if (key != 0) {
        keyboard.press(key);
        delay(100);
      }
      keyboard.releaseAll();
      delay(100);
    } else {
      keyboard.releaseAll();
      delay(50);
    }
  } else if (cmd == "SHIFT") {
    keyboard.press(KEY_LEFT_SHIFT);
    delay(100);
    if (param.length() > 0) {
      String upperParam = param;
      upperParam.toUpperCase();
      uint8_t key = getKeyCode(upperParam);
      if (key != 0) {
        keyboard.press(key);
        delay(100);
      }
      keyboard.releaseAll();
      delay(100);
    } else {
      keyboard.releaseAll();
      delay(50);
    }
  } else if (cmd == "TAB") {
    pressKey(KEY_TAB);
  } else if (cmd == "ESCAPE" || cmd == "ESC") {
    pressKey(KEY_ESC);
  } else if (cmd == "SPACE") {
    pressKey(' ');
  } else if (cmd == "UPARROW" || cmd == "UP") {
    pressKey(KEY_UP_ARROW);
  } else if (cmd == "DOWNARROW" || cmd == "DOWN") {
    pressKey(KEY_DOWN_ARROW);
  } else if (cmd == "LEFTARROW" || cmd == "LEFT") {
    pressKey(KEY_LEFT_ARROW);
  } else if (cmd == "RIGHTARROW" || cmd == "RIGHT") {
    pressKey(KEY_RIGHT_ARROW);
  } else if (cmd == "CAPSLOCK") {
    pressKey(KEY_CAPS_LOCK);
  } else if (cmd == "DELETE") {
    pressKey(KEY_DELETE);
  } else if (cmd == "BACKSPACE") {
    pressKey(KEY_BACKSPACE);
  } else if (cmd == "HOME") {
    pressKey(KEY_HOME);
  } else if (cmd == "END") {
    pressKey(KEY_END);
  } else if (cmd == "PAGEUP") {
    pressKey(KEY_PAGE_UP);
  } else if (cmd == "PAGEDOWN") {
    pressKey(KEY_PAGE_DOWN);
  } else if (cmd == "F1") {
    pressKey(KEY_F1);
  } else if (cmd == "F2") {
    pressKey(KEY_F2);
  } else if (cmd == "F3") {
    pressKey(KEY_F3);
  } else if (cmd == "F4") {
    pressKey(KEY_F4);
  } else if (cmd == "F5") {
    pressKey(KEY_F5);
  } else if (cmd == "F6") {
    pressKey(KEY_F6);
  } else if (cmd == "F7") {
    pressKey(KEY_F7);
  } else if (cmd == "F8") {
    pressKey(KEY_F8);
  } else if (cmd == "F9") {
    pressKey(KEY_F9);
  } else if (cmd == "F10") {
    pressKey(KEY_F10);
  } else if (cmd == "F11") {
    pressKey(KEY_F11);
  } else if (cmd == "F12") {
    pressKey(KEY_F12);
  }
}

void executeScript(String scriptPath) {
  File script = LittleFS.open("/scripts/" + scriptPath, "r");
  if (!script) {
    return;
  }
  
  delay(500);
  
  while (script.available()) {
    String line = script.readStringUntil('\n');
    line.replace("\r", "");
    line.trim();
    
    if (line.length() == 0 || line.startsWith("REM") || line.startsWith("#")) {
      continue;
    }
    
    int spaceIndex = line.indexOf(' ');
    String cmd = spaceIndex > 0 ? line.substring(0, spaceIndex) : line;
    String param = spaceIndex > 0 ? line.substring(spaceIndex + 1) : "";
    cmd.trim();
    param.trim();
    
    executeCommand(cmd, param);
  }
  
  script.close();
  keyboard.releaseAll();
}

void setupServer() {
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("/");
  });
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'><style>";
    html += "body{font-family:Arial;max-width:600px;margin:20px auto;padding:20px}";
    html += "h1{color:#333}h2{font-size:1.3em;margin-top:30px}form{margin:20px 0}";
    html += "input,select,button,textarea{width:100%;padding:10px;margin:10px 0;font-size:16px;box-sizing:border-box}";
    html += "textarea{font-family:monospace;min-height:200px;resize:vertical}";
    html += "button{background:#007bff;color:white;border:none;cursor:pointer;border-radius:5px}";
    html += "button:hover{background:#0056b3}.status{padding:10px;margin:10px 0;border-radius:5px}";
    html += ".connected{background:#d4edda;color:#155724}.disconnected{background:#f8d7da;color:#721c24}";
    html += "</style></head><body><h1>BadUSB Control</h1><div class='status ";
    html += bleConnected ? "connected'>BLE: Connected" : "disconnected'>BLE: Not Connected";
    html += "</div><h2>Create Script (Text)</h2><form action='/create' method='post'>";
    html += "<input type='text' name='filename' placeholder='Script name (without .txt)' required>";
    html += "<textarea name='content' placeholder='Enter Rubber Ducky script here...' required></textarea>";
    html += "<button type='submit'>Create Script</button></form>";
    html += "<h2>Upload Script (File)</h2><form action='/upload' method='post' enctype='multipart/form-data'>";
    html += "<input type='file' name='file' accept='.txt' required><button type='submit'>Upload File</button></form>";
    html += "<h2>Execute Script</h2><form action='/execute' method='post'>";
    html += "<select name='script' required><option value=''>Select a script</option>";
    html += generateScriptListHTML();
    html += "</select><button type='submit'>Execute</button></form></body></html>";
    
    request->send(200, "text/html", html);
  });
  
  server.on("/create", HTTP_POST, [](AsyncWebServerRequest *request) {
    String filename = "";
    String content = "";
    
    if (request->hasParam("filename", true)) {
      filename = request->getParam("filename", true)->value();
    }
    if (request->hasParam("content", true)) {
      content = request->getParam("content", true)->value();
    }
    
    if (filename.length() > 0 && content.length() > 0) {
      if (!filename.endsWith(".txt")) {
        filename += ".txt";
      }
      
      if (!LittleFS.exists("/scripts")) {
        LittleFS.mkdir("/scripts");
      }
      
      File file = LittleFS.open("/scripts/" + filename, "w");
      if (file) {
        file.print(content);
        file.close();
        loadScriptList();
        request->send(200, "text/html", "<html><body><h2>Script Created</h2><a href='/'>Back</a></body></html>");
      } else {
        request->send(500, "text/html", "<html><body><h2>Error</h2><a href='/'>Back</a></body></html>");
      }
    } else {
      request->send(400, "text/html", "<html><body><h2>Missing Data</h2><a href='/'>Back</a></body></html>");
    }
  });
  
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<html><body><h2>Upload Complete</h2><a href='/'>Back</a></body></html>");
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    static File uploadFile;
    
    if (index == 0) {
      if (!LittleFS.exists("/scripts")) {
        LittleFS.mkdir("/scripts");
      }
      uploadFile = LittleFS.open("/scripts/" + filename, "w");
    }
    
    if (uploadFile) {
      uploadFile.write(data, len);
    }
    
    if (final) {
      if (uploadFile) {
        uploadFile.close();
      }
      loadScriptList();
    }
  });
  
  server.on("/execute", HTTP_POST, [](AsyncWebServerRequest *request) {
    String scriptName = "";
    if (request->hasParam("script", true)) {
      scriptName = request->getParam("script", true)->value();
    }
    
    if (scriptName.length() > 0 && bleConnected) {
      queuedScript = scriptName;
      request->send(200, "text/html", "<html><body><h2>Queued! WiFi will disconnect briefly.</h2></body></html>");
    } else {
      request->send(200, "text/html", "<html><body><h2>Error: BLE Not Connected</h2><a href='/'>Back</a></body></html>");
    }
  });
  
  server.begin();
  serverActive = true;
}

void startAP() {
  WiFi.mode(WIFI_AP);
  
  IPAddress local_ip(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.softAPConfig(local_ip, local_ip, subnet);
  WiFi.softAP("416846831");
  
  dnsServer.start(DNS_PORT, "*", local_ip);
  setupServer();
}

void setup() {
  Serial.begin(115200);
  
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  ttgo->rtc->check();
  
  clearScreen();
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->setTextSize(2);
  ttgo->tft->setCursor(10, 10);
  ttgo->tft->print("Initializing...");
  
  if (!LittleFS.begin(true)) {
    clearScreen();
    ttgo->tft->setCursor(10, 10);
    ttgo->tft->print("LittleFS Failed");
    while(1) delay(1000);
  }
  
  if (!LittleFS.exists("/scripts")) {
    LittleFS.mkdir("/scripts");
  }
  
  loadScriptList();
  
  keyboard.begin();
  
  startAP();
  
  delay(1000);
  updateTimeDisplay();
}

void loop() {
  if (serverActive) {
    dnsServer.processNextRequest();
  }
  
  bleConnected = keyboard.isConnected();
  
  if (queuedScript.length() > 0) {
    String scriptToRun = queuedScript;
    queuedScript = "";
    
    server.end();
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    serverActive = false;
    
    delay(1000);
    executeScript(scriptToRun);
    delay(1000);
    
    startAP();
  }
  
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 60000) {
    updateTimeDisplay();
    lastUpdate = millis();
  }
}
