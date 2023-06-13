#include <stdint.h>
#include <WiFi.h>

#pragma once
namespace PixelFlut
{
    // Helper Structures
    typedef struct
    {
        unsigned int width;
        unsigned int height;
    } CanvasSize;

    typedef struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } rrggbb;

    typedef struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } rrggbbaa;

    typedef struct
    {
        rrggbb **pixels;
        unsigned int width;
        unsigned int height;
    } PictureRGB;

    typedef struct
    {
        rrggbbaa **pixels;
        unsigned int width;
        unsigned int height;
    } PictureRGBA;


    typedef struct
    {
        uint32_t *pixels;
        unsigned int width;
        unsigned int height;
    } PictureRGB_raw;

    // Pixelflut Definitions
    class PixelClient
    {
    private:
        virtual void send_pixel_rgb(const unsigned int x, const unsigned int y, rrggbb *color) = 0;
        virtual void send_pixel_rgba(const unsigned int x, const unsigned int y, rrggbbaa *color) = 0;
        virtual void send_set_offset(const unsigned int x, const unsigned int y) = 0;
        virtual void receive_buffer(String *buffer) = 0;

    public:
        virtual void command_get_help(String *help) = 0;
        virtual void command_get_size(CanvasSize *size) = 0;

        virtual void send_picture(PictureRGB *picture, unsigned int x, unsigned int y) = 0;
        virtual void send_picture(PictureRGBA *picture, unsigned int x, unsigned int y) = 0;
        virtual void send_picture(uint32_t *pixel_array, unsigned int x, unsigned int y);
        virtual unsigned int request_client_connections() = 0;
    };

    class Esp32PixelClient : public PixelClient
    {
    private:
        WiFiClient *wifi_client;

        void send_pixel_rgb(const unsigned int x, const unsigned int y, rrggbb *color);
        void send_pixel_rgba(const unsigned int x, const unsigned int y, rrggbbaa *color);
        void send_set_offset(const unsigned int x, const unsigned int y);
        void receive_buffer(String *buffer);

    public:
        Esp32PixelClient(WiFiClient *p_wifi_client);
        ~Esp32PixelClient();

        void command_get_help(String *help);
        void command_get_size(CanvasSize *size);

        void send_picture(PictureRGB *picture, unsigned int x, unsigned int y);
        void send_picture(PictureRGBA *picture, unsigned int x, unsigned int y);
        void send_picture(PictureRGB_raw *pixel_array, unsigned int x, unsigned int y);

        unsigned int request_client_connections();
    };
}
