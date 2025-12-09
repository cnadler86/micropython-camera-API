#!/bin/bash
set -e

# Default values
IDF_PATH_DEFAULT="$HOME/esp/esp-idf"
MICROPYTHON_PATH=""
IDF_PATH="$IDF_PATH_DEFAULT"
BOARD="ESP32_GENERIC_S3"
BOARD_VARIANT=""
CAMERA_MODEL=""
BUILD_DIR="build-mp_camera"

# Parse arguments
usage() {
    echo "Usage: $0 -m <micropython_path> [-i <idf_path>] [-b <board>] [-v <board_variant>] [-c <camera_model>]"
    echo ""
    echo "Options:"
    echo "  -m <path>    Path to MicroPython directory (required)"
    echo "  -i <path>    Path to ESP-IDF directory (optional, default: $IDF_PATH_DEFAULT)"
    echo "  -b <board>   Board name (optional, e.g. ESP32_GENERIC_S3, default: $BOARD)"
    echo "  -v <variant> Board variant (optional, e.g. SPIRAM_OCT)"
    echo "  -c <model>   Camera model (optional, e.g. FREENOVE_ESP32S3_CAM)"
    echo "  -h           Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 -m ~/privat/micropython"
    echo "  $0 -m ~/privat/micropython -i ~/esp/esp-idf"
    echo "  $0 -m ~/privat/micropython -b ESP32_GENERIC_S3"
    echo "  $0 -m ~/privat/micropython -b ESP32_GENERIC_S3 -v SPIRAM_OCT"
    echo "  $0 -m ~/privat/micropython -b ESP32_GENERIC_S3 -c FREENOVE_ESP32S3_CAM"
    exit 1
}

# Parse command line options
while getopts "m:i:b:v:c:h" opt; do
    case $opt in
        m) MICROPYTHON_PATH="$OPTARG" ;;
        i) IDF_PATH="$OPTARG" ;;
        b) BOARD="$OPTARG" ;;
        v) BOARD_VARIANT="$OPTARG" ;;
        c) CAMERA_MODEL="$OPTARG" ;;
        h) usage ;;
        *) usage ;;
    esac
done

# Check required arguments
if [ -z "$MICROPYTHON_PATH" ]; then
    echo "Error: MicroPython path is required (-m option)"
    echo ""
    usage
fi

# Validate paths
if [ ! -d "$MICROPYTHON_PATH" ]; then
    echo "Error: MicroPython directory not found: $MICROPYTHON_PATH"
    exit 1
fi

if [ ! -d "$IDF_PATH" ]; then
    echo "Error: ESP-IDF directory not found: $IDF_PATH"
    exit 1
fi

if [ ! -f "$IDF_PATH/export.sh" ]; then
    echo "Error: ESP-IDF export.sh not found in: $IDF_PATH"
    exit 1
fi

# Get absolute paths
MICROPYTHON_PATH=$(realpath "$MICROPYTHON_PATH")
IDF_PATH=$(realpath "$IDF_PATH")
MODULE_PATH=$(dirname "$(realpath "$0")")

echo "=========================================="
echo "Building MicroPython with Camera Module"
echo "=========================================="
echo "MicroPython path: $MICROPYTHON_PATH"
echo "ESP-IDF path:     $IDF_PATH"
echo "Module path:      $MODULE_PATH"
if [ -n "$BOARD" ]; then
    echo "Board:            $BOARD"
    BUILD_DIR="${BUILD_DIR}-${BOARD}"
    if [ -n "$BOARD_VARIANT" ]; then
        echo "Board variant:    $BOARD_VARIANT"
        BUILD_DIR="${BUILD_DIR}_${BOARD_VARIANT}"
    fi
fi
if [ -n "$CAMERA_MODEL" ]; then
    echo "Camera model:     $CAMERA_MODEL"
fi
echo "Build directory:  $BUILD_DIR"
echo "=========================================="
echo ""

# Source ESP-IDF environment
echo "Setting up ESP-IDF environment..."
source "$IDF_PATH/export.sh"

# Change to MicroPython ESP32 port directory
cd "$MICROPYTHON_PATH/ports/esp32"

# Build idf.py command with optional parameters
IDF_CMD="idf.py -B $BUILD_DIR"

if [ -n "$BOARD" ]; then
    IDF_CMD="$IDF_CMD -D MICROPY_BOARD=$BOARD"
fi

if [ -n "$BOARD_VARIANT" ]; then
    IDF_CMD="$IDF_CMD -D MICROPY_BOARD_VARIANT=$BOARD_VARIANT"
fi

if [ -n "$CAMERA_MODEL" ]; then
    IDF_CMD="$IDF_CMD -D MICROPY_CAMERA_MODEL=$CAMERA_MODEL"
fi

IDF_CMD="$IDF_CMD -D USER_C_MODULES=$MODULE_PATH/micropython.cmake"
IDF_CMD="$IDF_CMD -D EXTRA_COMPONENT_DIRS=$MODULE_PATH"
IDF_CMD="$IDF_CMD build"

# Build MicroPython with IR Learn module
echo ""
echo "Building MicroPython..."
echo "Command: $IDF_CMD"
eval $IDF_CMD

# Create firmware images
echo ""
echo "Creating firmware images..."
cd "$BUILD_DIR"

python "$MICROPYTHON_PATH/ports/esp32/makeimg.py" \
    sdkconfig \
    bootloader/bootloader.bin \
    partition_table/partition-table.bin \
    micropython.bin \
    firmware.bin \
    micropython.uf2

echo ""
echo "Build completed successfully!"
echo "Firmware files in: $MICROPYTHON_PATH/ports/esp32/$BUILD_DIR"
echo "=========================================="

# Clean up build directory
cd "$MODULE_PATH"
rm -rf build
