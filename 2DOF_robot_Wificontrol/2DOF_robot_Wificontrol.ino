#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <ESPmDNS.h>

// Wi-Fi
const char* WIFI_SSID = "Orange_Swiatlowod_B9F0";
const char* WIFI_PASS = "YxGi5hSMNv2nj6W9za";
const char* AP_SSID   = "ESP32-Servo";
const char* AP_PASS   = "12345678";

// Servos
const int SERVO1_PIN = 18, SERVO2_PIN = 19;
int usMin = 500, usMax = 2500;
Servo s1, s2;
volatile int angle1 = 90, angle2 = 90;

WebServer server(80);
const int LED_PIN = 2;

static inline int clamp(int x,int a,int b){ return x<a?a:(x>b?b:x); }
static inline int angleToUs(int deg){ return map(clamp(deg,0,180),0,180,usMin,usMax); }
static inline void blink(){ pinMode(LED_PIN,OUTPUT); digitalWrite(LED_PIN,!digitalRead(LED_PIN)); }

// ------------ HTML: form + test links (no JS required) -------------
const char PAGE[] PROGMEM = R"HTML(
<!doctype html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ESP32 — Two Servo Sliders</title>
<style>
 body{font-family:system-ui,Segoe UI,Roboto,Arial;margin:24px;max-width:720px}
 .card{border:1px solid #ddd;border-radius:8px;padding:16px;margin:12px 0}
 .row{display:flex;align-items:center;gap:12px}
 input[type=range]{width:100%}
 .val{min-width:60px;text-align:right}
 button{padding:10px 16px;border:1px solid #0a7c2f;background:#0db24b;color:#fff;border-radius:8px;font-weight:600}
 a.btn{display:inline-block;margin-right:8px;margin-top:8px;padding:8px 12px;background:#eee;border-radius:6px;text-decoration:none;color:#000;border:1px solid #ccc}
</style></head><body>
<h2>ESP32 – Two Servo Sliders</h2>

<div class="card">
  <b>Quick test links (no JavaScript):</b><br>
  <a class="btn" href="/set?s1=0&s2=0">Both 0°</a>
  <a class="btn" href="/set?s1=90&s2=90">Both 90°</a>
  <a class="btn" href="/set?s1=180&s2=180">Both 180°</a>
</div>

<form class="card" action="/set" method="get">
  <div class="row">
    <label for="s1"><b>Servo 1</b> (GPIO18)</label>
    <input id="s1" name="s1" type="range" min="0" max="180" value="90" step="1" oninput="v1.textContent=this.value">
    <div class="val"><span id="v1">90</span>°</div>
  </div>
  <div class="row" style="margin-top:12px">
    <label for="s2"><b>Servo 2</b> (GPIO19)</label>
    <input id="s2" name="s2" type="range" min="0" max="180" value="90" step="1" oninput="v2.textContent=this.value">
    <div class="val"><span id="v2">90</span>°</div>
  </div>
  <div style="margin-top:16px">
    <button type="submit">Send</button>
  </div>
</form>

<div class="card">
  <b>Status:</b> <a class="btn" href="/status">/status</a> (JSON)
</div>
</body></html>
)HTML";

// ------------ Handlers ------------
void handleRoot(){ Serial.println("[HTTP] GET / (root)"); server.send_P(200,"text/html",PAGE); }

void handleSet(){
  Serial.printf("[HTTP] GET /set  args=%d\n", server.args());
  for(uint8_t i=0;i<server.args();++i)
    Serial.printf("  %s=%s\n", server.argName(i).c_str(), server.arg(i).c_str());

  if(!server.hasArg("s1") || !server.hasArg("s2")){
    server.send(400,"text/plain","Missing s1/s2"); return;
  }
  int a1 = clamp(server.arg("s1").toInt(),0,180);
  int a2 = clamp(server.arg("s2").toInt(),0,180);

  angle1=a1; angle2=a2;
  s1.write(angle1); s2.write(angle2);
  Serial.printf("[SET] S1=%3d° (%4dus), S2=%3d° (%4dus)\n",
                angle1, angleToUs(angle1), angle2, angleToUs(angle2));
  blink();

  // simple response page
  char buf[160];
  snprintf(buf,sizeof(buf),
    "OK\nS1=%d (us~%d)\nS2=%d (us~%d)\nBack: <a href='/'>home</a>",
    angle1, angleToUs(angle1), angle2, angleToUs(angle2));
  server.send(200,"text/html",buf);
}

void handleStatus(){
  char buf[64];
  snprintf(buf,sizeof(buf),"{\"s1\":%d,\"s2\":%d}", angle1, angle2);
  server.send(200,"application/json",buf);
}

void handleNotFound(){
  Serial.printf("[HTTP] 404 %s\n", server.uri().c_str());
  server.send(404,"text/plain","Not found");
}

void startHTTP(){
  server.on("/", HTTP_GET, handleRoot);
  server.on("/set", HTTP_GET, handleSet);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void setup(){
  pinMode(LED_PIN, OUTPUT); digitalWrite(LED_PIN, LOW);
  Serial.begin(115200); delay(500);
  Serial.println("\nBooting…");
  ESP32PWM::allocateTimer(0); ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2); ESP32PWM::allocateTimer(3);
  s1.setPeriodHertz(50); s2.setPeriodHertz(50);
  s1.attach(SERVO1_PIN, usMin, usMax); s2.attach(SERVO2_PIN, usMin, usMax);
  s1.write(angle1); s2.write(angle2);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Connecting to '%s'…\n", WIFI_SSID);
  uint32_t t0=millis(); while(WiFi.status()!=WL_CONNECTED && millis()-t0<20000){ delay(400); Serial.print('.'); }
  Serial.println();
  if(WiFi.status()==WL_CONNECTED){
    Serial.print("Wi-Fi IP: "); Serial.println(WiFi.localIP());
    if(MDNS.begin("esp32")) Serial.println("mDNS: http://esp32.local");
    startHTTP();
  }else{
    Serial.println("STA failed → AP mode");
    WiFi.mode(WIFI_AP); WiFi.softAP(AP_SSID, AP_PASS);
    Serial.print("AP IP: "); Serial.println(WiFi.softAPIP());
    startHTTP();
  }
}

void loop(){ server.handleClient(); }