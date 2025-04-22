"""Module to convert an `.png` into an `.ico`, while having the option to remove one color (e.g. background) from the image.
"""
import argparse
import os
from PIL import Image

def hex_to_rgb(hex_color):
    """Convert HEX color (e.g., #ff0000) to an (R, G, B) tuple."""
    hex_color = hex_color.lstrip("#")
    if len(hex_color) != 6:
        raise ValueError("Hex color must be in the format #RRGGBB")
    return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))

def remove_color_and_save_as_ico(input_png, output_ico, target_color, tolerance=0):
    """Removes a specific color from a PNG and saves it as an ICO file."""
    img = Image.open(input_png).convert("RGBA")
    data = img.getdata()

    new_data = []
    for item in data:
        r, g, b, _ = item
        if all(abs(c1 - c2) <= tolerance for c1, c2 in zip((r, g, b), target_color)):
            new_data.append((r, g, b, 0))
        else:
            new_data.append(item)

    img.putdata(new_data)
    img.save(output_ico, format="ICO")

def process_folder(hex_color, folder, tolerance=0):
    """Processes all PNG files in a folder and converts them to ICO."""
    target_color = hex_to_rgb(hex_color)

    if not os.path.isdir(folder):
        print(f"Error: Folder '{folder}' does not exist.")
        return

    png_files = [f for f in os.listdir(folder) if f.lower().endswith(".png")]
    if not png_files:
        print("No PNG files found in the folder.")
        return

    for png_file in png_files:
        input_path = os.path.join(folder, png_file)
        output_path = os.path.join(folder, os.path.splitext(png_file)[0] + ".ico")
        remove_color_and_save_as_ico(input_path, output_path, target_color, tolerance)
        print(f"Converted: {png_file} -> {os.path.basename(output_path)}")

def make_resources_rc(folder):
    """Generates a resource.rc file with the list of ICO files in the folder."""
    project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    folder_path = os.path.join(project_root, folder)

    rc_file_path = os.path.join(folder_path, "resource.rc")

    with open(rc_file_path, "w") as f:
        f.write("#include \"resource.h\"\n\n")
        f.write("ICON_ID ICON \"include/src/icon.ico\"\n\n")

        for i, ico_file in enumerate(sorted(f for f in os.listdir(folder_path) if f.lower().endswith(".ico"))):
            relative_path = os.path.relpath(os.path.join(folder_path, ico_file), project_root).replace(os.sep, "/")
            f.write(f"ANIMATION{i} ICON \"{relative_path}\"\n")

def make_resources_h(folder):
    """Generates a resource.h file with the list of ICO files in the folder."""
    with open(os.path.join(folder, "resource.h"), "w") as f:
        f.write("#ifndef RESOURCE_H\n#define RESOURCE_H\n\n")
        f.write("#define ICON_ID 101\n\n")
        for i, _ in enumerate(sorted(f for f in os.listdir(folder) if f.lower().endswith(".ico"))):
            f.write(f"#define ANIMATION{i} {102 + i}\n")
        f.write("\n#endif\n")

def main():
    parser = argparse.ArgumentParser(description="Convert PNG to ICO while removing a specific color.")
    parser.add_argument("-c", "--color", required=True, help="Target color to remove in HEX format (e.g., #FF0000)")
    parser.add_argument("-f", "--folder", required=True, help="Folder containing PNG files to convert")
    parser.add_argument("--tolerance", type=int, default=0, help="Color tolerance (default 0 for exact match)")

    args = parser.parse_args()
    process_folder(args.color, args.folder, args.tolerance)
    make_resources_rc(args.folder)
    make_resources_h(args.folder)

if __name__ == "__main__":
    main()
