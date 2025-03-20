from PIL import Image

def convert_to_rgba4444(r, g, b, a):
    r = (r >> 4) & 0xF
    g = (g >> 4) & 0xF
    b = (b >> 4) & 0xF
    a = (a >> 4) & 0xF
    return (a << 12) | (r << 8) | (g << 4) | b

def image_to_c_array(image_path, output_path):
    image = Image.open(image_path).convert("RGBA")
    width, height = image.size
    pixels = list(image.getdata())

    with open(output_path, 'w') as f:
        f.write("#include <cstdint>\n")
        f.write("uint16_t const font_map[] = {\n")
        for y in range(height):
            for x in range(width):
                r, g, b, a = pixels[y * width + x]
                rgba4444 = convert_to_rgba4444(r, g, b, a)
                f.write(f"0x{rgba4444:04X}, ")
            f.write("\n")
        f.write("};\n")
if __name__ == "__main__":
    image_to_c_array("font_map.png", "font_map.hpp")
