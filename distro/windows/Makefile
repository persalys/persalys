#
# Compile and create OpenTURNS installer for windows
#
# Rules dependencies make things done in the right order, however it does not
# detect if OpenTURNS sources has changed. This has to be specified manually:
# - to recompute a rules, remove the corresponding file
#   i.e. some OpenTURNS .cxx have changed:
#     rm mod-compile; make
# - to recompute every rules: make clean; make
# - to rebuild from scratch: make mrproper; make
#

###############################################################################
# adapt these following lines to your configuration :
#  OT_PREFIX  WINDEPS
# e.g. make OT_PREFIX=$PWD/ot/rc/mingw_build/install

# compiler prefix
ARCH=i686
TARGET = $(ARCH)-w64-mingw32

# OpenTurns paths
OT_PREFIX ?= $(HOME)/ot/mingw_build/install
OT_CONFIG_CMD = $(OT_PREFIX)/bin/openturns-config

OT_VERSION ?= $(shell $(OT_CONFIG_CMD) --version)

WINDEPS ?= $(OT_PREFIX)/../../distro/windows/openturns-developers-windeps

# Module paths
MOD_SRC    ?= $(PWD)/../..
MOD_BUILD  ?= $(MOD_SRC)/build-$(TARGET)
MOD_PREFIX ?= $(MOD_BUILD)/install
MODULE_VERSION = $(shell cat $(MOD_SRC)/VERSION)

# Wine
WINE_PREFIX = $(WINDEPS)/wine-$(ARCH)
export WINEPREFIX = $(WINE_PREFIX)
ifeq ($(ARCH), i686)
export WINEARCH = win32
else
export WINEARCH = win64
endif

MINGW_PREFIX = $(WINDEPS)/mingw-w64-x86_64/usr

# Python
PYBASEVER = 2.7
PYBASEVER_NODOT = $(shell echo $(PYBASEVER) | sed "s|\.||g")
PYTHON_EXECUTABLE=$(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).exe
export PYTHONHOME := $(MINGW_PREFIX)/$(TARGET)
export PYTHONPATH := $(MINGW_PREFIX)/$(TARGET)/lib/python$(PYBASEVER_NODOT)


CMAKE=cmake

# launch checktests before creating the installer (y,n)
CHECK_OT = y

VERBOSE=ON

JOBS=$(shell getconf _NPROCESSORS_ONLN)
JOBS_PYTHON=$(JOBS)
JOBS_TEST=$(JOBS)
JOBS_COTIRE=$(JOBS)
export PATH := $(MINGW_PREFIX)/bin:$(PATH)

DEBUG_OT=n
USE_OTFMI=OFF
USE_QT5=ON
ifeq ($(USE_QT5),ON)
QWT_LIB=qwt
else
QWT_LIB=qwt-qt4
endif
USE_PARAVIEW=$(USE_QT5)

###############################################################################
# rules:

.PHONY: all clean-python clean mrproper


all: mod-installer

reloc-toolchain:
	ln -sf /usr/lib/x86_64-linux-gnu/qt5/bin/moc $(MINGW_PREFIX)/$(TARGET)/lib/qt/bin/moc
	ln -sf /usr/lib/x86_64-linux-gnu/qt5/bin/rcc $(MINGW_PREFIX)/$(TARGET)/lib/qt/bin/rcc
	sed -i "s|/usr/$(TARGET)|$(MINGW_PREFIX)/$(TARGET)|g" $(MINGW_PREFIX)/$(TARGET)/lib/cmake/paraview-*/ParaViewTargets*.cmake $(MINGW_PREFIX)/$(TARGET)/lib/cmake/paraview-*/VTKConfig.cmake $(MINGW_PREFIX)/$(TARGET)/lib/cmake/paraview-*/Modules/vtk*.cmake
	touch reloc-toolchain

mod-check-compiler: reloc-toolchain
	@echo -n "Check OpenTURNS compilation dependencies: "
	@ls $(MOD_SRC) $(PYTHON_EXECUTABLE) > /dev/null
	@echo "ok"
ifeq ($(USE_OTFMI),ON)
	echo "__version__='0'" > $(MINGW_PREFIX)/$(TARGET)/lib/python$(PYBASEVER_NODOT)/otfmi.py
endif
	touch mod-check-compiler

mod-config: mod-check-compiler
	-mkdir -p $(MOD_BUILD)
	cp $(MINGW_PREFIX)/share/mingw/toolchain-$(TARGET).cmake $(MOD_BUILD)
	echo "set ( CMAKE_FIND_ROOT_PATH $(MINGW_PREFIX)/$(TARGET) )" >> $(MOD_BUILD)/toolchain-$(TARGET).cmake
	cp $(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).dll $(MOD_BUILD)
	cd $(MOD_BUILD) && rm -f CMakeCache.txt && $(CMAKE) \
	  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
	  -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4 -D_hypot=hypot" \
	  -DCMAKE_TOOLCHAIN_FILE=toolchain-$(TARGET).cmake \
	  -DCMAKE_VERBOSE_MAKEFILE=$(VERBOSE) \
	  -DOpenTURNS_DIR=$(OT_PREFIX)/lib/cmake/openturns \
	  -DOTMORRIS_DIR=$(OTMORRIS_PREFIX)/lib/cmake/otmorris \
	  -DPYTHON_INCLUDE_DIR=$(MINGW_PREFIX)/$(TARGET)/include/python$(PYBASEVER_NODOT) \
	  -DPYTHON_INCLUDE_DIR2=$(MINGW_PREFIX)/$(TARGET)/include/python$(PYBASEVER_NODOT) \
	  -DPYTHON_LIBRARY=$(MINGW_PREFIX)/$(TARGET)/lib/libpython$(PYBASEVER_NODOT).dll.a \
	  -DPYTHON_EXECUTABLE=$(PYTHON_EXECUTABLE) \
	  -DCMAKE_INSTALL_PREFIX=$(MOD_PREFIX) \
	  -DSITE_PACKAGE=Lib/site-packages \
	  -DUSE_SALOME=OFF \
	  -DUSE_SPHINX=OFF \
	  -DUSE_OTFMI=$(USE_OTFMI) \
	  -DUSE_QT5=$(USE_QT5) \
	  -DQt5Gui_DIR=$(MINGW_PREFIX)/$(TARGET)/lib/cmake/Qt5Gui \
	  -DQt5Core_DIR=$(MINGW_PREFIX)/$(TARGET)/lib/cmake/Qt5Core \
	  -DQt5Widgets_DIR=$(MINGW_PREFIX)/$(TARGET)/lib/cmake/Qt5Widgets \
	  -DQt5Test_DIR=$(MINGW_PREFIX)/$(TARGET)/lib/cmake/Qt5Test \
	  -DQt5Help_DIR=$(MINGW_PREFIX)/$(TARGET)/lib/cmake/Qt5Help \
	  -DQt5Sql_DIR=$(MINGW_PREFIX)/$(TARGET)/lib/cmake/Qt5Sql \
	  -DQt5Network_DIR=$(MINGW_PREFIX)/$(TARGET)/lib/cmake/Qt5Network \
	  -DQWT_LIBRARY=$(MINGW_PREFIX)/$(TARGET)/lib/lib$(QWT_LIB).dll.a \
	  -DUSE_PARAVIEW=$(USE_PARAVIEW) \
	  -DUSE_COTIRE=ON -DCOTIRE_MAXIMUM_NUMBER_OF_UNITY_INCLUDES="-j$(JOBS_COTIRE)" \
	$(MOD_SRC)
	touch mod-config

mod-compile: mod-config
	cd $(MOD_BUILD) && make -j$(JOBS)
	touch mod-compile

mod-install: mod-compile
	cd $(MOD_BUILD) && make install
ifeq ($(DEBUG_OT),n)
	$(TARGET)-strip --strip-unneeded $(MOD_PREFIX)/bin/*.dll
	$(TARGET)-strip -g $(MOD_PREFIX)/lib/*.a
	$(TARGET)-strip --strip-unneeded $(MOD_PREFIX)/Lib/site-packages/*/*.pyd
	$(TARGET)-strip $(MOD_PREFIX)/bin/*.exe
endif
	# otmorris
	cp $(OTMORRIS_PREFIX)/bin/libotmorris.dll $(MOD_PREFIX)/bin
	# boost libs
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libboost_regex-mt.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libboost_system-mt.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libboost_filesystem-mt.dll $(MOD_PREFIX)/bin
	# qt deps
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libpng*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libexpat*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libjpeg*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libdbus*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libsqlite*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libssl*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libcrypto*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libtiff*.dll $(MOD_PREFIX)/bin
ifeq ($(USE_QT5),ON)
	cp $(MINGW_PREFIX)/$(TARGET)/bin/*eay32.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libsqlite*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libintl*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libbz2*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libpcre-*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libpcre2*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libharfbuzz*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libfreetype*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libglib-2.0*.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libgraphite2*.dll $(MOD_PREFIX)/bin
endif
	# qt libs
ifeq ($(USE_QT5),ON)
	cp $(MINGW_PREFIX)/$(TARGET)/bin/Qt5*.dll $(MOD_PREFIX)/bin
	cp -r $(MINGW_PREFIX)/$(TARGET)/lib/qt/plugins/ $(MOD_PREFIX)/lib
else
	cp $(MINGW_PREFIX)/$(TARGET)/bin/Qt*4.dll $(MOD_PREFIX)/bin
endif
	# qwt
	cp $(MINGW_PREFIX)/$(TARGET)/bin/$(QWT_LIB).dll $(MOD_PREFIX)/bin

ifeq ($(USE_PARAVIEW),ON)
	# paraview deps
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libpugixml.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libjsoncpp.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libprotobuf.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/liblz4.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libnetcdf-11.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libnetcdf_c++-4.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libhdf5.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libhdf5_hl.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libQtTesting.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libcurl-4.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libidn2-0.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libssh2-1.dll $(MOD_PREFIX)/bin
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libunistring-2.dll $(MOD_PREFIX)/bin

	# paraview libs
	cp $(MINGW_PREFIX)/$(TARGET)/bin/libvtk*5.4.dll $(MOD_PREFIX)/bin
endif
	touch mod-install

mod-test: mod-install
ifeq ($(CHECK_OT),y)
	cd $(MOD_BUILD) && make tests -j$(JOBS)
	cp $(OT_PREFIX)/bin/*.dll $(MOD_BUILD)/lib/test
	cp $(MOD_PREFIX)/bin/*.dll $(MOD_BUILD)/lib/test
	cp $(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).dll $(MOD_BUILD)/lib/test
	cp $(OT_PREFIX)/bin/*.dll $(MOD_BUILD)/python/test
	cp $(MOD_PREFIX)/bin/*.dll $(MOD_BUILD)/python/test
	cp $(MINGW_PREFIX)/$(TARGET)/bin/python$(PYBASEVER_NODOT).dll $(MOD_BUILD)/python/test
	cd $(MOD_BUILD) && xvfb-run ctest -j$(JOBS_TEST) --output-on-failure --timeout 180
endif
	touch mod-test

mod-check-installer: mod-test
	@echo -n "Check OpenTURNS prefix: "
	@ls $(MOD_PREFIX) > /dev/null
	@echo "ok"
	@echo -n "Check nsis compiler: "
	@makensis -VERSION > /dev/null 2>&1
	@echo "ok"
	@echo "OpenTURNS installer dependencies seems all right."
	touch mod-check-installer


mod-installer: mod-check-installer
	makensis -DMODULE_PREFIX=$(MOD_PREFIX) -DMODULE_VERSION=$(MODULE_VERSION) -DOPENTURNS_VERSION=$(OT_VERSION) -DPYBASEVER=$(PYBASEVER) -DPYBASEVER_NODOT=$(PYBASEVER_NODOT) -DARCH=$(ARCH) installer.nsi
	touch mod-installer


clean:
	-rm -f mod-*
	-cd $(MOD_BUILD) && make clean


mrproper:
	-rm -f mod-*
	-rm -rf $(MOD_PREFIX) $(MOD_BUILD)
	-rm -f *.exe


