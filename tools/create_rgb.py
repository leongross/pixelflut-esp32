#!/usr/bin/env python3
from PIL import Image
import sys


def main():
    # Path to the input JPEG image
    if len(sys.argv) != 3:
        print("Usage: create_rgb.py <input_image_path> <array_name>")
        return

    input_image_path = sys.argv[1]
    output_path = "include/pixel_array.h"

    # Open the image using Pillow
    image = Image.open(input_image_path)

    # Convert the image to RGB mode
    rgb_image = image.convert("RGB")

    # Get the width and height of the image
    width, height = rgb_image.size
    c_array = "constexpr unsigned int PIXEL_WIDTH = " + str(width) + ";\n"
    c_array += "constexpr unsigned int PIXEL_HEIGHT = " + str(height) + ";\n\n"

    # Get the raw RGB pixel values
    pixel_values = list(rgb_image.getdata())

    # Create the C array string
    c_array += "#include <stdint.h>\n\nuint32_t PIXEL_ARRAY[] = {\n"

    for r, g, b in pixel_values:
        # Combine the RGB values into a single uint32_t value
        rgb_value = (r << 16) | (g << 8) | b
        c_array += f"    0x{rgb_value:08X},\n"

    # Close the C array string
    c_array += "};"

    # Save the C array to a file
    try:
        with open(output_path, "w") as output_file:
            output_file.write(c_array)

        print(f"Successfully wrote to file {output_path}")
    except IOError:
        print("Error: Could not write to file")
        return


if __name__ == "__main__":
    main()
