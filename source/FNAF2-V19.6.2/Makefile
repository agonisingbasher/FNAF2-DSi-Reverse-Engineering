.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment; install devkitPro nds-dev first")
endif

include $(DEVKITARM)/ds_rules

DEV_BUILD ?= 0
DISABLE_STARTUP_LOG ?= 1
TARGET := FNAF2-V19.6.2-DSi$(if $(filter 1,$(DEV_BUILD)),-DEV,-NORMAL)$(if $(filter 0,$(DISABLE_STARTUP_LOG)),-LOG,)
ROOTDIR ?= $(CURDIR)
override export _ARM7_ELF := -7 $(ROOTDIR)/ds7_maine.elf
BUILD ?= build$(if $(filter 0,$(DISABLE_STARTUP_LOG)),-log,)
SOURCES := source
NITRO_FILES := $(ROOTDIR)/nitrofiles
override export _ADDFILES := -d $(NITRO_FILES)
DATA :=
INCLUDES := assets
GAME_TITLE := FNAF2 V19.6.2 $(if $(filter 1,$(DEV_BUILD)),DEV,NORMAL)
GAME_SUBTITLE1 := FNaF2 Office Prototype
GAME_SUBTITLE2 := By FelipeAlexandre10
GAME_ICON := $(ROOTDIR)/assets/icon.bmp

ARCH := -march=armv5te -mtune=arm946e-s -mthumb
CFLAGS := -g -Wall -Wextra -Werror -Os -flto -ffunction-sections -fdata-sections $(ARCH)
CFLAGS += $(INCLUDE) -DARM9
ifeq ($(DEV_BUILD),1)
CFLAGS += -DDEV_BUILD
endif
CXXFLAGS := $(CFLAGS) -std=gnu++17 -fno-exceptions -fno-rtti -Wno-ignored-qualifiers
ASFLAGS := -g $(ARCH)
LDFLAGS = -specs=ds_arm9.specs -g -flto $(ARCH) -Wl,-Map,$(notdir $*.map) -Wl,--undefined=rom_alignment_padding
ifeq ($(DISABLE_STARTUP_LOG),1)
CFLAGS += -DDISABLE_STARTUP_LOG
LIBS := -lfilesystem -lfat -lnds9
else
LIBS := -lfilesystem -lfat -lnds9
endif
LIBDIRS := $(LIBNDS)

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export ROOTDIR := $(CURDIR)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) $(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
ifeq ($(strip $(CPPFILES)),)
export LD := $(CC)
else
export LD := $(CXX)
endif
export OFILES := $(addsuffix .o,$(BINFILES)) $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) $(foreach dir,$(LIBDIRS),-I$(dir)/include) -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: all clean title-assets $(BUILD)
all: $(BUILD)
	python3 assets/finalize_dsi_rom.py $(TARGET).nds
title-assets:
	python3 assets/build_runtime_title_assets.py
	python3 assets/build_qol_ui.py
	python3 assets/build_gameover_crt.py
	python3 assets/build_victory_industrial.py
	python3 assets/build_v194_victory.py
	python3 assets/build_contract_form.py
	python3 assets/build_warning_start.py
$(BUILD): title-assets
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).nds $(TARGET).ds.gba
else
DEPENDS := $(OFILES:.o=.d)
$(OUTPUT).nds: $(OUTPUT).elf
$(OUTPUT).nds: $(GAME_ICON) $(ROOTDIR)/Makefile $(wildcard $(NITRO_FILES)/*)
$(OUTPUT).nds: $(GAME_ICON) $(ROOTDIR)/Makefile $(wildcard $(NITRO_FILES)/*)
$(OUTPUT).elf: $(OFILES)
%.bin.o: %.bin
	@$(bin2o)
-include $(DEPENDS)
endif
