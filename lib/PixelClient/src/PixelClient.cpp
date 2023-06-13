// Path: lib/PixelClient/src/PixelClient.cpp
#include <WiFi.h>
#include <PixelClient.hpp>

PixelFlut::Esp32PixelClient::Esp32PixelClient(WiFiClient *p_wifi_client)
{
    this->wifi_client = p_wifi_client;
}

PixelFlut::Esp32PixelClient::~Esp32PixelClient()
{
    delete wifi_client;
}

// Private methods

void PixelFlut::Esp32PixelClient::send_pixel_rgb(const unsigned int x, const unsigned int y, PixelFlut::rrggbb *color)
{
    char command_buffer[64] = {0};
    sprintf(command_buffer, "PX %d %d %02x%02x%02x\n", x, y, color->r, color->g, color->b);
    this->wifi_client->print(command_buffer);
    this->wifi_client->flush();
}

void PixelFlut::Esp32PixelClient::send_pixel_rgba(const unsigned int x, const unsigned int y, PixelFlut::rrggbbaa *color)
{
    char command_buffer[64] = {0};
    sprintf(command_buffer, "PX %d %d %02x%02x%02x%02x\n", x, y, color->r, color->g, color->b, color->a);
    this->wifi_client->print(command_buffer);
    this->wifi_client->flush();
}

void PixelFlut::Esp32PixelClient::receive_buffer(String *buffer)
{
    *buffer = this->wifi_client->readString();
}

void PixelFlut::Esp32PixelClient::send_set_offset(const unsigned int x, const unsigned int y)
{
    char command_buffer[64] = {0};
    sprintf(command_buffer, "OFFSET %d %d\n", x, y);
    this->wifi_client->print(command_buffer);
    this->wifi_client->flush();
}

// Public methods
void PixelFlut::Esp32PixelClient::command_get_help(String *help)
{
    this->wifi_client->println("HELP");
    *help = wifi_client->readString();
}

void PixelFlut::Esp32PixelClient::command_get_size(CanvasSize *size)
{
    // sending command
    this->wifi_client->println("SIZE");
    String response = wifi_client->readString();

    // skip constant prefix "SIZE "
    response = response.substring(5, response.length() - 1);

    // parsing response
    String _width = response.substring(0, response.indexOf(" "));
    String _height = response.substring(response.indexOf(" ") + 1, response.length());

    size->width = _width.toInt();
    size->height = _height.toInt();
}

void PixelFlut::Esp32PixelClient::send_picture(PictureRGB *picture, unsigned int x, unsigned int y)
{
    // how does flusing affect performance?
    for (unsigned int row = x; row < picture->height; row++)
    {
        for (unsigned int col = y; col < picture->width; col++)
        {
            PixelFlut::rrggbb *color = &picture->pixels[row][col];
            this->send_pixel_rgb(row, col, color);
        }
    }
}

void PixelFlut::Esp32PixelClient::send_picture(PictureRGBA *picture, unsigned int x, unsigned int y)
{
    // how does flusing affect performance?
    for (unsigned int row = x; row < picture->height; row++)
    {
        for (unsigned int col = y; col < picture->width; col++)
        {
            PixelFlut::rrggbbaa *color = &picture->pixels[row][col];
            this->send_pixel_rgba(row, col, color);
        }
    }
}

unsigned int PixelFlut::Esp32PixelClient::request_client_connections()
{
    this->wifi_client->println("CONNECTIONS");
    this->wifi_client->flush();
    String response = wifi_client->readString();
    return response.toInt();
}

void PixelFlut::Esp32PixelClient::send_picture(PictureRGB_raw *picture, unsigned int x, unsigned int y)
{
    PixelFlut::rrggbb color;
    uint32_t current_pixel = 0;

    // is it more efficient to 
    for (int row = 0; row < picture->height; row++)
    {
        for (int column = 0; column < picture->width; column++)
        {
            // since each entry represents a single pixel, we can just use the index and shift
            // the entry to extract the values for r,g,b
            current_pixel = picture->pixels[row + column];
            color.r = (current_pixel >> 16);
            color.g = ((current_pixel & 0x0000ff00) >> 8);
            color.b = (current_pixel & 0x000000ff);
            this->send_pixel_rgb(row, column, &color);
        }
    }
}
