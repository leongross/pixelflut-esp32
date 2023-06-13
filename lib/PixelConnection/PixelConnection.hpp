#include <Arduino.h>
#include <WiFi.h>

#pragma once

namespace PixelFlut
{
    typedef struct {
        const char *ip;
        const char *port;
    } PixelConnection;

    typedef struct
    {
        const char *ssid;
        const char *password;
    } WiFiConnection;

    char *encryption_type_to_str(wifi_auth_mode_t type)
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

    void scan_networks()
    {
        int networks = WiFi.scanNetworks();
        Serial.println("[*] WiFi Scan done");

        if (networks == 0)
        {
            Serial.println("[*] No networks found");
        }
        else
        {
            Serial.print("[*] Found ");
            Serial.print(networks);
            Serial.println(" networks");
            for (int i = 0; i < networks; ++i)
            {
                Serial.print("[*] ");
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(WiFi.SSID(i));
                Serial.print(" (");
                Serial.print(WiFi.RSSI(i));
                Serial.print(") ");
                Serial.println(encryption_type_to_str(WiFi.encryptionType(i)));
            }
        }
    }
}
