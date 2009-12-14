# This is an attempt to build the plugin on Mac, Windows and Linux.
# On the Mac, a (better?) alternative is to build via Xcode.
# On Windows, this requires mingw.
# This hasn't been tried on Linux.

# Change this according to platform: mac, win or lin.
SYSTEM := $(shell uname -s)

ifeq ($(SYSTEM), Darwin)
    PLATFORM = mac
    PREPROC_MACROS = -DAPL=1 -DIBM=0 -DLIN=0
    PLATFORM_LINKER_FLAGS = -flat_namespace -undefined warning
    PLATFORM_CODEGEN_FLAGS = -fPIC -arch i386 -arch ppc -mmacosx-version-min=10.4
else ifeq ($(SYSTEM), MINGW32_NT-5.1)
    PLATFORM = win
    PREPROC_MACROS = -DAPL=0 -DIBM=1 -DLIN=0
    LINKED_LIBS = -lws2_32 -lstdc++ -lopengl32 -l:XPLM.lib -l:XPWidgets.lib
    PLATFORM_LINKER_FLAGS = -LSDK/Libraries/Win
    PLATFORM_CODEGEN_FLAGS = 
else ifeq ($(SYSTEM), Linux)
    PLATFORM = lin
    PREPROC_MACROS = -DAPL=0 -DIBM=0 -DLIN=1
    PLATFORM_CODEGEN_FLAGS = -fPIC
endif

CC = gcc
INCLUDES = -ISDK/CHeaders/Widgets -ISDK/CHeaders/XPLM -IThirdParty -I.
WARNING_FLAGS = -Wall -Wno-sign-compare -Wno-unknown-pragmas
CFLAGS = -std=c99 $(PLATFORM_CODEGEN_FLAGS) $(WARNING_FLAGS) $(PREPROC_MACROS) $(INCLUDES)
CXXFLAGS = $(PLATFORM_CODEGEN_FLAGS) $(WARNING_FLAGS) $(PREPROC_MACROS) $(INCLUDES)
LDFLAGS = -dynamiclib -shared -static-libgcc $(PLATFORM_CODEGEN_FLAGS) $(PLATFORM_LINKER_FLAGS)
OBJECTS = src/iX-Yoke-plugin.o src/Preferences.o src/Server.o src/Window.o src/bsd-strlcat.o ThirdParty/XPListBox.o ThirdParty/XPPopups.o
PRODUCT = Wi-Fly-$(PLATFORM).xpl


main: objects
	$(CC) $(LDFLAGS) -o $(PRODUCT) $(OBJECTS) $(LINKED_LIBS)

objects: $(OBJECTS)

clean-objects:
	rm -f $(OBJECTS)

clean: clean-objects
	rm -f $(PRODUCT)

