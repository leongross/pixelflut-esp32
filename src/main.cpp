#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// https://stackoverflow.com/questions/74551259/esp32-send-a-simple-tcp-message-and-receive-the-response
// http://94.45.233.1:1337/
// https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
// https://www.upesy.com/blogs/tutorials/how-to-connect-wifi-acces-point-with-esp32#

typedef struct
{
  const char *ssid;
  const char *password;
} WiFiConnection;

class Picture
{
private:
  unsigned int width;
  unsigned int height;

public:
  unsigned int **pixels;
};

char *encryptio_type_to_str(wifi_auth_mode_t type);

const unsigned long BAUD_RATE = 9600;
constexpr unsigned int WIFI_CONNECTION_ATTEMPTS = 10;

WiFiConnection freifunk = {
    .ssid = "freifunk.karlsruhe.de",
    .password = NULL};

WiFiConnection gpn = {
    .ssid = "GPN21-open",
    .password = ""};

// const char *PIXEL_IP = "94.45.233.1";
// const char *PIXEL_IP = "94.45.226.122"; // known-good

const char *PIXEL_IP = "94.45.246.31"; // this laptop local ip in GPN21-open
const unsigned int PIXEL_PORT = 1337;
// const char *PIXEL_DOMAIN = "pixel.local";

// const unsigned int PIXEL_PORT = 1234;
// const char *PIXEL_DOMAIN = "gpn-flut.poeschl.xyz";

void send_pxiel(WiFiClient *client, unsigned int x, unsigned int y, unsigned int hex)
{
  char buffer[32] = {0};
  sprintf(buffer, "PX %d %d %06x\n", x, y, hex);
  // Serial.printf("[*] [send_pixel] sending '%s'\n", buffer);
  client->print(buffer);
  client->flush();
}

void set_offset(WiFiClient *client, unsigned int x, unsigned int y)
{
  char buffer[32] = {0};
  sprintf(buffer, "OFFSET %d %d\n", x, y);
  client->print(buffer);
  client->flush();
}

void request_pixel(WiFiClient *client, unsigned int x, unsigned int y)
{
  char buffer[32] = {0};
  sprintf(buffer, "PX %d %d\n", x, y);
  client->print(buffer);
  client->flush();
}

void request_resolution(WiFiClient *client)
{
  client->print("SIZE\n");
  client->flush();
}

void request_help(WiFiClient *client)
{
  Serial.println("[*] 'HELP'\n");
  client->print("HELP\n");
  client->flush();
}

void request_client_connection_count(WiFiClient *client)
{
  client->print("CONNECTIONS\n");
  client->flush();
}

void get_response(WiFiClient *client)
{
  Serial.println("[*] Response:");
  String line = client->readString();
  Serial.println(line);
}

void scan_network()
{
  int n = WiFi.scanNetworks();
  Serial.println("[*] WiFi Scan done");

  if (n == 0)
  {
    Serial.println("[*] No networks found");
  }
  else
  {
    Serial.print("[*] Found ");
    Serial.print(n);
    Serial.println(" networks");
    for (int i = 0; i < n; ++i)
    {
      Serial.print("[*] ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(") ");
      Serial.println(encryptio_type_to_str(WiFi.encryptionType(i)));
      delay(10);
    }
  }
}

char *encryptio_type_to_str(wifi_auth_mode_t type)
{
  switch (type)
  {
  case WIFI_AUTH_OPEN:
  {
    return (char *)"WIFI_AUTH_OPEN";
    break;
  }
  case WIFI_AUTH_WEP:
  {
    return (char *)"WIFI_AUTH_WEP";
    break;
  }
  case WIFI_AUTH_WPA_PSK:
  {
    return (char *)"WIFI_AUTH_WPA_PSK";
    break;
  }
  case WIFI_AUTH_WPA2_PSK:
  {
    return (char *)"WIFI_AUTH_WPA2_PSK";
    break;
  }
  case WIFI_AUTH_WPA_WPA2_PSK:
  {
    return (char *)"WIFI_AUTH_WPA_WPA2_PSK";
    break;
  }
  case WIFI_AUTH_WPA2_ENTERPRISE:
  {
    return (char *)"WIFI_AUTH_WPA2_ENTERPRISE";
    break;
  }
  default:
    return (char *)"UNKNOWN";
    break;
  }
}

void setup()
{
  Serial.begin(BAUD_RATE);
  WiFiConnection *WIFI = &gpn;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  delay(2000);
  scan_network();

  Serial.printf("[*] Connecting to WiFi '%s':%s\n", WIFI->ssid, WIFI->password);

  // if (WIFI->password == NULL)
  // {
  //   WiFi.begin(WIFI->ssid);
  // }
  // else
  // {
  //   WiFi.begin(WIFI->ssid, WIFI->password);
  // }

  // WiFi.begin(WIFI->ssid, (char *)NULL);
  WiFi.begin(WIFI->ssid);

  for (int i = 0; i < WIFI_CONNECTION_ATTEMPTS; i++)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      break;
    }
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("[+] Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFiClient client;
    Serial.printf("[*] Connecting to pixelflut with '%s:%d'\n", PIXEL_IP, PIXEL_PORT);
    // client.connect(PIXEL_DOMAIN, PIXEL_PORT) 94.45.246.31;
    client.connect(PIXEL_IP, PIXEL_PORT);

    if (client.connected())
    {
      Serial.println("[+] WiFiClient connected");
    }
    else
    {
      Serial.println("[-] WiFiClient not connected");
      exit(1);
    }

    // polling
    // while (!client.available())
    // {
    //   // Serial.println("[*] polling ...");
    //   // delay(1);
    // }
    Serial.println("[+] Done polling, client is available");

    // testing all the functions once
    // Dimensions: 800 x 600
    for (int i = 200; i < 400; i++)
    {
      for (int j = 200; j < 400; j++)
      {
        send_pxiel(&client, i, j, random(0, 0xFFFFFF));
      }
    }
    request_resolution(&client);
    get_response(&client);

    // request_client_connection_count(&client);
    // printf("[*] Connected\n");
    // get_response(&client);
  }
  else
  {
    Serial.printf("[*] WiFi not connected. Tried %d attempts.\n", WIFI_CONNECTION_ATTEMPTS);
    exit(1);
  }
}
