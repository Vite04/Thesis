#include <Arduino.h>
#include <WiFi.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "lwip/ip6_addr.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"

const char* ssid     = "[REDACTED]";
const char* password = "[REDACTED]";

static esp_netif_t* sta_netif = nullptr;

esp_ip6_addr_t g_ll_addr;
esp_ip6_addr_t g_global_addr;
bool g_has_ll     = false;
bool g_has_global = false;

// =========================
// Counters
// =========================
uint32_t http_hits_total      = 0;
uint32_t hidden_admin_hits    = 0;
uint32_t telnet_connections   = 0;
uint32_t telnet_login_success = 0;
uint32_t telnet_login_failure = 0;

// =========================
// Timestamp helper (ms since boot)
// =========================
String ts() {
  return "[" + String(millis()) + "ms]";
}

// =========================
// Helpers
// =========================
String ipv6ToString(const esp_ip6_addr_t& addr) {
  char buf[40];
  ip6addr_ntoa_r((const ip6_addr_t*)&addr, buf, sizeof(buf));
  return String(buf);
}

String sockaddrIn6ToString(const sockaddr_in6* addr) {
  char buf[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET6, &(addr->sin6_addr), buf, sizeof(buf));
  return String(buf);
}

void printIPv6(const esp_ip6_addr_t& addr) {
  Serial.println(ipv6ToString(addr));
}

String readLineFromSocket(int clientSock) {
  String line = "";
  char c;
  while (true) {
    int n = recv(clientSock, &c, 1, 0);
    if (n <= 0) break;
    if (c == '\r') continue;
    if (c == '\n') break;
    line += c;
    if (line.length() > 200) break;
  }
  return line;
}

void printCounters() {
  Serial.println("=== COUNTERS ===");
  Serial.println("  http_hits_total:      " + String(http_hits_total));
  Serial.println("  hidden_admin_hits:    " + String(hidden_admin_hits));
  Serial.println("  telnet_connections:   " + String(telnet_connections));
  Serial.println("  telnet_login_success: " + String(telnet_login_success));
  Serial.println("  telnet_login_failure: " + String(telnet_login_failure));
  Serial.println("================");
}

// =========================
// IPv6 Event Handler
// =========================
void onGotIPv6(void* arg, esp_event_base_t event_base,
               int32_t event_id, void* event_data) {
  if (event_base == IP_EVENT && event_id == IP_EVENT_GOT_IP6) {
    ip_event_got_ip6_t* event = (ip_event_got_ip6_t*)event_data;
    Serial.print(ts() + " [IPv6] Address acquired: ");
    printIPv6(event->ip6_info.ip);

    if (sta_netif != nullptr) {
      if (esp_netif_get_ip6_linklocal(sta_netif, &g_ll_addr) == ESP_OK)
        g_has_ll = true;
      if (esp_netif_get_ip6_global(sta_netif, &g_global_addr) == ESP_OK)
        g_has_global = true;
    }
  }
}

// =========================
// HTTP Server (port 80)
// =========================
void httpServerTask(void* pvParameters) {
  int serverSock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (serverSock < 0) {
    Serial.println(ts() + " [HTTP] Failed to create IPv6 socket");
    vTaskDelete(NULL);
    return;
  }

  int opt = 1;
  setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in6 serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin6_family = AF_INET6;
  serverAddr.sin6_addr   = in6addr_any;
  serverAddr.sin6_port   = htons(80);

  if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    Serial.println(ts() + " [HTTP] bind() failed");
    close(serverSock);
    vTaskDelete(NULL);
    return;
  }

  if (listen(serverSock, 5) < 0) {
    Serial.println(ts() + " [HTTP] listen() failed");
    close(serverSock);
    vTaskDelete(NULL);
    return;
  }

  Serial.println(ts() + " [HTTP] Listening on port 80 (IPv6)");

  while (true) {
    sockaddr_in6 clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSock < 0) { delay(10); continue; }

    String remoteIP     = sockaddrIn6ToString(&clientAddr);
    uint16_t remotePort = ntohs(clientAddr.sin6_port);

    char buffer[1024];
    int len = recv(clientSock, buffer, sizeof(buffer) - 1, 0);

    if (len > 0) {
      buffer[len] = '\0';
      String request   = String(buffer);
      int lineEnd      = request.indexOf("\r\n");
      String firstLine = (lineEnd > 0) ? request.substring(0, lineEnd) : request;

      String path = "/";
      if (firstLine.startsWith("GET ")) {
        int secondSpace = firstLine.indexOf(' ', 4);
        if (secondSpace > 4) path = firstLine.substring(4, secondSpace);
      }

      http_hits_total++;
      Serial.println(ts() + " [HTTP] src=" + remoteIP
                     + " port=" + String(remotePort)
                     + " path=" + path);

      String body;
      String status = "HTTP/1.1 200 OK\r\n";

      if (path == "/") {
        body  = "<html><body>";
        body += "<h1>ESP32 IoT Device Dashboard</h1>";
        body += "<p>Status: Online</p>";
        body += "<p>Device: ESP32</p>";
        body += "<p>Service: IPv6 test node</p>";
        body += "<p>Firmware: 1.0-demo</p>";
        body += "<p>MAC: " + WiFi.macAddress() + "</p>";
        if (g_has_global) body += "<p>Global IPv6: " + ipv6ToString(g_global_addr) + "</p>";
        if (g_has_ll)     body += "<p>Link-local: "  + ipv6ToString(g_ll_addr)     + "</p>";
        body += "<hr>";
        body += "<p>HTTP hits: "          + String(http_hits_total)    + "</p>";
        body += "<p>Hidden admin hits: "  + String(hidden_admin_hits)  + "</p>";
        body += "<p>Telnet connections: " + String(telnet_connections) + "</p>";
        body += "</body></html>";
      }
      else if (path == "/hidden_admin") {
        hidden_admin_hits++;
        Serial.println(ts() + " [HTTP] *** HIDDEN ADMIN accessed from " + remoteIP + " ***");
        body  = "<html><body>";
        body += "<h1>Hidden Admin Interface</h1>";
        body += "<p>Diagnostic Mode: ENABLED</p>";
        body += "<p>Debug Access: TRUE</p>";
        body += "<p>Warning: undocumented endpoint</p>";
        body += "<p>MAC: " + WiFi.macAddress() + "</p>";
        body += "</body></html>";
      }
      else {
        status = "HTTP/1.1 404 Not Found\r\n";
        body   = "<html><body><h1>404 Not Found</h1></body></html>";
      }

      String response = status
        + "Content-Type: text/html\r\n"
        + "Content-Length: " + String(body.length()) + "\r\n"
        + "Connection: close\r\n\r\n"
        + body;

      send(clientSock, response.c_str(), response.length(), 0);
    }

    close(clientSock);
    printCounters();
  }
}

// =========================
// Telnet-like TCP Service (port 23)
// =========================
void telnetServerTask(void* pvParameters) {
  int serverSock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (serverSock < 0) {
    Serial.println(ts() + " [TELNET] Failed to create IPv6 socket");
    vTaskDelete(NULL);
    return;
  }

  int opt = 1;
  setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in6 serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin6_family = AF_INET6;
  serverAddr.sin6_addr   = in6addr_any;
  serverAddr.sin6_port   = htons(23);

  if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
    Serial.println(ts() + " [TELNET] bind() failed");
    close(serverSock);
    vTaskDelete(NULL);
    return;
  }

  if (listen(serverSock, 2) < 0) {
    Serial.println(ts() + " [TELNET] listen() failed");
    close(serverSock);
    vTaskDelete(NULL);
    return;
  }

  Serial.println(ts() + " [TELNET] Listening on port 23 (IPv6)");

  while (true) {
    sockaddr_in6 clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    int clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientSock < 0) { delay(10); continue; }

    telnet_connections++;
    String remoteIP     = sockaddrIn6ToString(&clientAddr);
    uint16_t remotePort = ntohs(clientAddr.sin6_port);

    Serial.println(ts() + " [TELNET] src=" + remoteIP
                   + " port=" + String(remotePort) + " connected");

    String banner = "ESP32 Legacy Telnet Service\r\nUsername: ";
    send(clientSock, banner.c_str(), banner.length(), 0);

    String user = readLineFromSocket(clientSock);
    Serial.println(ts() + " [TELNET] src=" + remoteIP + " username=" + user);

    String askPass = "Password: ";
    send(clientSock, askPass.c_str(), askPass.length(), 0);

    String pass = readLineFromSocket(clientSock);
    Serial.println(ts() + " [TELNET] src=" + remoteIP + " password=" + pass);

    if (user == "admin" && pass == "admin") {
      telnet_login_success++;
      Serial.println(ts() + " [TELNET] src=" + remoteIP + " login=SUCCESS");

      String ok = "\r\nAccess granted\r\nType 'status' or 'exit'\r\n> ";
      send(clientSock, ok.c_str(), ok.length(), 0);

      while (true) {
        String cmd = readLineFromSocket(clientSock);
        if (cmd.length() == 0) break;

        Serial.println(ts() + " [TELNET] src=" + remoteIP + " cmd=" + cmd);

        if (cmd == "exit") {
          String bye = "Bye\r\n";
          send(clientSock, bye.c_str(), bye.length(), 0);
          break;
        } else if (cmd == "status") {
          String msg = "Device status: ONLINE\r\n";
          msg += "MAC: " + WiFi.macAddress() + "\r\n";
          if (g_has_global) msg += "Global IPv6: " + ipv6ToString(g_global_addr) + "\r\n";
          msg += "HTTP hits: "       + String(http_hits_total)      + "\r\n";
          msg += "Telnet sessions: " + String(telnet_connections)   + "\r\n";
          msg += "Login successes: " + String(telnet_login_success) + "\r\n";
          msg += "> ";
          send(clientSock, msg.c_str(), msg.length(), 0);
        } else {
          String msg = "Unknown command\r\n> ";
          send(clientSock, msg.c_str(), msg.length(), 0);
        }
      }
    } else {
      telnet_login_failure++;
      Serial.println(ts() + " [TELNET] src=" + remoteIP + " login=FAILED");
      String denied = "\r\nAccess denied\r\n";
      send(clientSock, denied.c_str(), denied.length(), 0);
    }

    close(clientSock);
    printCounters();
  }
}

// =========================
// Setup
// =========================
void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); // Keep WiFi active  prevents disconnection during experiments

  esp_event_handler_register(IP_EVENT, IP_EVENT_GOT_IP6, &onGotIPv6, nullptr);

  Serial.println("=== ESP32 IPv6 Thesis Demo ===");
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Waiting for WiFi...");
  }

  Serial.println("WiFi connected");
  Serial.print("IPv4: "); Serial.println(WiFi.localIP());
  Serial.print("MAC:  "); Serial.println(WiFi.macAddress());

  sta_netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  if (sta_netif == nullptr) {
    Serial.println("[ERROR] Could not get WIFI_STA_DEF handle");
    return;
  }

  esp_err_t err = esp_netif_create_ip6_linklocal(sta_netif);
  Serial.print("Link-local IPv6 init: ");
  Serial.println(err == ESP_OK ? "OK" : "FAILED");

  delay(3000); // Wait for SLAAC global address assignment

  xTaskCreatePinnedToCore(httpServerTask,   "httpTask",   8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(telnetServerTask, "telnetTask", 8192, NULL, 1, NULL, 1);
}

// =========================
// Loop  prints identity every 100s
// =========================
void loop() {
  if (sta_netif != nullptr) {
    esp_ip6_addr_t ll_addr, global_addr;
    esp_err_t ll_ok = esp_netif_get_ip6_linklocal(sta_netif, &ll_addr);
    esp_err_t gl_ok = esp_netif_get_ip6_global(sta_netif,    &global_addr);

    Serial.println("=== DEVICE IDENTITY ===");
    Serial.print("MAC:         "); Serial.println(WiFi.macAddress());
    Serial.print("IPv4:        "); Serial.println(WiFi.localIP());

    if (ll_ok == ESP_OK) {
      g_ll_addr = ll_addr; g_has_ll = true;
      Serial.print("Link-local:  "); printIPv6(ll_addr);
    } else {
      Serial.println("Link-local:  not yet available");
    }

    if (gl_ok == ESP_OK) {
      g_global_addr = global_addr; g_has_global = true;
      Serial.print("Global IPv6: "); printIPv6(global_addr);
    } else {
      Serial.println("Global IPv6: not yet available");
    }

    Serial.println("=======================");
    printCounters();
  }

  delay(100000);
}


