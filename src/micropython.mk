CAMERA_MOD_DIR := $(USERMOD_DIR)
SRC_USERMOD_C += $(addprefix $(CAMERA_MOD_DIR)/, modcamera_api.c)
SRC_USERMOD_LIB_C += $(addprefix $(CAMERA_MOD_DIR)/, modcamera.c)
CFLAGS_USERMOD += -I$(CAMERA_MOD_DIR)