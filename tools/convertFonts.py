import freetype
from PIL import Image
import numpy as np

# Function to convert a single character into a bitmap array
def char_to_bitmap(char, font, width=8, height=8):
    # Load the character into the font
    font.load_char(char)
    
    # Get the bitmap of the character
    bitmap = font.glyph.bitmap
    rows, cols = bitmap.rows, bitmap.width
    
    # Convert the bitmap to a numpy array
    img = np.zeros((rows, cols), dtype=np.uint8)
    if bitmap.buffer:
        img = np.array(bitmap.buffer).reshape((rows, cols))
    
    # Normalize the image to binary (0 or 1)
    img = np.where(img > 0, 1, 0)
    
    # Scale the image to fit the specified width and height
    scaled_img = Image.fromarray(img.astype(np.uint8) * 255).resize((width, height), Image.Resampling.LANCZOS)
    scaled_img = scaled_img.convert('1')  # Convert to black and white
    
    # Convert pixel data into a list of 8-bit values (each row)
    pixels = np.array(scaled_img)
    bitmap_array = []
    for row in range(height):
        byte_value = 0
        for col in range(width):
            byte_value |= (1 if pixels[row, col] != 0 else 0) << (width - 1 - col)
        bitmap_array.append(f"0b{bin(byte_value)[2:].zfill(width)}")
    
    return bitmap_array

# Convert TTF to the required format
def ttf_to_font_array(ttf_path, width=8, height=8):
    # Load the font
    face = freetype.Face(ttf_path)
    face.set_char_size(48 * 64)  # Set the font size (adjust as needed)
    
    font_array = {}
    
    # Convert ASCII characters from 32 to 126
    for i in range(32, 127):
        char = chr(i)
        bitmap = char_to_bitmap(char, face, width, height)
        font_array[i] = bitmap
    
    return font_array

# Convert the font array into a C header file
def generate_c_array(font_array, width, height):
    c_array = f"uint8_t font[128][{height}] = {{\n"
    for i in range(32, 127):
        char = chr(i)
        bitmap = font_array[i]
        c_array += f"    [{i}] = {{ {', '.join(bitmap)} }}, // '{char}'\n"
    c_array += "};\n"
    return c_array

# Main function
def main():
    ttf_path = "/home/aayush/Downloads/Hack-Regular.ttf"  # Path to your TTF file
    width = 8  # Desired font width
    height = 8  # Desired font height
    
    # Generate font array
    font_array = ttf_to_font_array(ttf_path, width, height)
    c_array = generate_c_array(font_array, width, height)
    
    # Save to a header file
    with open("font_array.h", "w") as f:
        f.write("#pragma once\n")
        f.write(c_array)
    
    print("C font array generated successfully:")
    print(c_array)

if __name__ == "__main__":
    main()
