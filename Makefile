INSTALL_PREFIX?=../install

ifeq ($(strip $(TOOLCHAIN)),)
	BUILD_NAME?=build
	BUILD_DIR?=../cmake.bld/$(shell basename $(CURDIR))
	BUILD_PATH?=$(BUILD_DIR)/$(BUILD_NAME)
	BUILD_TYPE?=RelWithDebInfo
else
	BUILD_NAME?=build-$(TOOLCHAIN)
	BUILD_DIR?=../cmake.bld/$(shell basename $(CURDIR))
	BUILD_PATH?=$(BUILD_DIR)/$(BUILD_NAME)
	BUILD_TYPE?=RelWithDebInfo
	CMAKE_ARGS=-DCMAKE_TOOLCHAIN_FILE=$(CURDIR)/etc/$(TOOLCHAIN)-toolchain.cmake
endif

define run_cmake =
	cmake \
	-G "Ninja Multi-Config" \
	-DCMAKE_CONFIGURATION_TYPES="Debug;RelWithDebInfo;Asan;UBsan;Msan;Lsan;Tsan" \
	-DCMAKE_DEFAULT_BUILD_TYPE=$(BUILD_TYPE) \
	-DCMAKE_INSTALL_PREFIX=$(abspath $(INSTALL_PREFIX)) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
	$(CMAKE_ARGS) \
	$(CURDIR)
endef

default: build

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

$(BUILD_PATH)/CMakeCache.txt: | $(BUILD_PATH)
	cd $(BUILD_PATH) && $(run_cmake)
	-rm compile_commands.json
	ln -s $(BUILD_PATH)/compile_commands.json

build: $(BUILD_PATH)/CMakeCache.txt
	cd $(BUILD_PATH) && ninja -f build-$(BUILD_TYPE).ninja -k 0 -v

install: $(BUILD_PATH)/CMakeCache.txt
	cd $(BUILD_PATH) && ninja -f build-$(BUILD_TYPE).ninja install

ctest: $(BUILD_PATH)/CMakeCache.txt
	cd $(BUILD_PATH) && ctest

ctest_ : build
	cd $(BUILD_PATH) && ctest

test: ctest_

cmake: | $(BUILD_PATH)
	cd $(BUILD_PATH) && $(run-cmake)

clean: $(BUILD_PATH)/CMakeCache.txt
	cd $(BUILD_PATH) && ninja -f build-$(BUILD_TYPE).ninja clean

realclean:
	rm -rf $(BUILD_PATH)

.PHONY: realclean clean cmake test ctest install build default
