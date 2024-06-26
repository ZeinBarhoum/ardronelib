SDK_PATH:=$(shell pwd)/ARDroneLib
PC_TARGET=yes
USE_LINUX=yes


ifdef MYKONOS
   include $(ARDRONE_CUSTOM_CONFIG)
   include $(ARDRONE_BUILD_CONFIG)
else
   include $(SDK_PATH)/Soft/Build/custom.makefile
   include $(SDK_PATH)/Soft/Build/config.makefile
endif

$(SDK_TARGET_DIR)

SDK_FLAGS+="USE_APP=no"
SDK_FLAGS+="USE_LIB=yes"

all: 
	@$(MAKE) -C $(SDK_PATH)/Soft/Build $(TMP_SDK_FLAGS) $(SDK_FLAGS) $(MAKECMDGOALS) USE_LINUX=yes
	@$(MAKE) -C $(SDK_PATH)/VP_SDK/Build $(TMP_SDK_FLAGS) $(SDK_FLAGS) $(MAKECMDGOALS) USE_LINUX=yes
              
install:
	mkdir -p ${INSTALL_PREFIX}
	find $(SDK_PATH)/Soft/Build -type f -name '*.a' -exec cp '{}' ${INSTALL_PREFIX} \;

clean:
	rm -rf $(SDK_PATH)/Soft/Build/targets_versions
	rm -rf $(SDK_PATH)/FFMPEG/ffmpeg 
	rm -rf $(SDK_PATH)/FFMPEG/Includes 
	rm -f $(SDK_PATH)/Soft/Common/generated_custom.h
