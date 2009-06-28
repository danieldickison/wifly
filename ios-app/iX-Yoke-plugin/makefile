# Change this according to platform: mac, win32 or linux.
PLATFORM = win

ifeq ($(PLATFORM), mac)
PREPROC_MACROS = -DAPL=1 -DIBM=0 -DLIN=0
PLATFORM_LINKER_FLAGS = -flat_namespace -undefined warning
endif
ifeq ($(PLATFORM), win)
PREPROC_MACROS = -DAPL=0 -DIBM=1 -DLIN=0
LINKED_LIBS = -lws2_32 -lstdc++ -lopengl32 -l:XPLM.lib -l:XPWidgets.lib
PLATFORM_LINKER_FLAGS = -LSDK/Libraries/Win
endif
ifeq ($(PLATFORM), linux)
PREPROC_MACROS = -DAPL=0 -DIBM=0 -DLIN=1
endif

CC = gcc
INCLUDES = -ISDK/CHeaders/Widgets -ISDK/CHeaders/XPLM -IThirdParty
CFLAGS = -std=c99 -Wall -fPIC $(PREPROC_MACROS) $(INCLUDES)
CXXFLAGS = -Wall -fPIC $(PREPROC_MACROS) $(INCLUDES)
LDFLAGS = -dynamiclib -shared -fPIC -static-libgcc $(PLATFORM_LINKER_FLAGS)
OBJECTS = iX-Yoke-plugin.o Preferences.o Server.o Window.o ThirdParty/XPListBox.o ThirdParty/XPPopups.o
PRODUCT = Wi-Fly-$(PLATFORM).xpl


main: objects
	$(CC) $(LDFLAGS) -o $(PRODUCT) $(OBJECTS) $(LINKED_LIBS)

objects: $(OBJECTS)

clean-objects:
	rm -f $(OBJECTS)

clean: clean-objects
	rm -f $(PRODUCT)

