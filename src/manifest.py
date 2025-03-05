# Include the board's default manifest.
include("$(PORT_DIR)/boards/manifest.py")
# Add custom driver
module("acamera.py")