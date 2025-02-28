#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

Preferences preferences;
WebServer server(80);

const char* apSSID = "ESP32-Setup";  // AP mode SSID
const char* apPassword = "12345678"; // AP mode Password

void handleRoot() {
    String html = "<html><body>";
    html += "<h2>WiFi Setup</h2>";
    html += "<form action='/save' method='POST'>";
    html += "SSID: <input type='text' name='ssid'><br>";
    html += "Password: <input type='password' name='password'><br>";
    html += "<input type='submit' value='Save'>";
    html += "</form></body></html>";

    server.send(200, "text/html", html);
}

void handleSave() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();

    server.send(200, "text/html", "<h3>WiFi Credentials Saved! Rebooting...</h3>");
    delay(2000);
    ESP.restart();
}

void setup() {
    Serial.begin(115200);
    
    preferences.begin("wifi", true);
    String savedSSID = preferences.getString("ssid", "");
    String savedPassword = preferences.getString("password", "");
    preferences.end();

    if (savedSSID.length() > 0) {
        Serial.println("Connecting to saved WiFi...");
        WiFi.begin(savedSSID.c_str(), savedPassword.c_str());

        int retry = 0;
        while (WiFi.status() != WL_CONNECTED && retry < 20) {
            delay(1000);
            Serial.print(".");
            retry++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\nConnected to WiFi: " + savedSSID);
            Serial.println("IP Address: " + WiFi.localIP().toString());
            return;
        }
    }

    Serial.println("\nStarting Access Point...");
    WiFi.softAP(apSSID, apPassword);
    Serial.println("AP IP: " + WiFi.softAPIP().toString());

    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();
}

void loop() {
    server.handleClient();
}