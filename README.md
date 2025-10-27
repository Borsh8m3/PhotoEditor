# PhotoEditor

Modernized console photo editor for ASCII **P2** PGM files. The project started as a university assignment and was refactored into a clean, portable C17 application that is easy to showcase in a portfolio.

## Features

- Load multiple grayscale PGM images into memory.
- Display concise summaries and ASCII previews directly in the terminal.
- Rotate images by 90° steps, apply binary thresholding, add salt & pepper noise, and run a 3×3 median filter.
- Export grayscale histograms to CSV files for further analysis.
- Save edited images back to the PGM format.

## Project structure

```
PhotoEditor/
├── include/              # Public headers shared by all modules
├── src/                  # Implementation files split into logical units
├── samples/              # Example PGM files for quick experimentation
└── CMakeLists.txt        # Cross-platform build configuration
```

## Quick start

```bash
# Configure and build (out-of-source)
cmake -S . -B build
cmake --build build

# Launch the editor
./build/photo_editor
```

On Windows you can use the Visual Studio generator instead:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

## Usage

1. **Load image** – provide the path to an ASCII PGM file (several samples are available in the `samples/` directory).
2. **Remove image** – delete an image from the in-memory library.
3. **List images** – print statistics (resolution, intensity range, min/max/average) for each loaded image.
4. **Edit image** – open a nested menu where you can:
   - Review the summary again.
   - Render an ASCII art preview.
   - Rotate the image left by 90° increments.
   - Generate a histogram CSV file.
   - Apply binary thresholding.
   - Add salt & pepper noise with a configurable probability.
   - Run a 3×3 median filter to remove noise.
   - Save the modified image back to disk.

All operations are performed in-memory, so you can experiment freely before exporting the final result.

## Code quality highlights

- **Strict warnings** (`-Wall -Wextra -Wpedantic -Werror`/`/W4 /WX`) keep the codebase tidy.
- **Modular design**: image I/O, processing routines, and CLI are split into dedicated translation units.
- **Deterministic algorithms**: the salt & pepper noise generator uses a lightweight xorshift RNG for reproducible results.
- **Portfolio ready**: the repository now ships with a thorough README, sane folder layout, build scripts, and clean git history.

Feel free to fork the project, add new filters, or integrate the modules into your own image processing experiments!
