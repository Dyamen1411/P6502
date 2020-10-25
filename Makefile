PROJECT_NAME := "P6502"
EXEC_NAME := P6502
MAINTAINER_NAME := Dyamen
MAINTAINER_MAIL := dyamenx76@gmail.com
MAINTAINER := $(MAINTAINER_NAME) <$(MAINTAINER_MAIL)>
VERSION_MAJOR := 0
VERSION_MINOR := 0
VERSION_PATCH := 0
VERSION := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

# Project directories

BUILD_DIR := ./build
INCLUDE_DIR := ./include
SOURCE_DIR := ./src

# Project files

SRCS := $(wildcard $(SOURCE_DIR)/*.cpp)
OBJS := $(SRCS:$(SOURCE_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP := $(OBJS:%.o=%.d)

# Buildtool variable

CC := g++
CWERROR := all extra conversion error write-strings fatal-errors pedantic
CWARN := $(CWERROR:%=-W%)
PPDEFINE := -DVERSION=\"$(VERSION)\" -DMAINTAINER=\"$(MAINTAINER_NAME)\"
CFLAGS := -I $(INCLUDE_DIR) $(CWARN) $(PPDEFINE) -pedantic -O2
LD := g++
LDFLAGS := 

# PHONY targets

.PHONY: all clean re getmaintainer getversion

# General targets

all: $(EXEC_NAME)

clean:
	@echo "Removing the build folder"
	@rm -rf $(BUILD_DIR)
	@rm -f $(EXEC_NAME)

re: clean all

# Some informations

getmaintainer:
	@echo $(MAINTAINER)

getversion:
	@echo $(VERSION)

# Project target

$(EXEC_NAME): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

# Build rules

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -o $@ -c $<

-include $(DEP)
