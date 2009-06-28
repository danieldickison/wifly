# Change these according to platform.
PREPROC_MACROS = -DAPL=1 -DIBM=0 -DLIN=0
PLATFORM = mac

INCLUDES = -ISDK/CHeaders/Widgets -ISDK/CHeaders/XPLM -IThirdParty
CFLAGS = -std=c99 -Wall -fPIC $(PREPROC_MACROS) $(INCLUDES)
CXXFLAGS = -Wall -fPIC $(PREPROC_MACROS) $(INCLUDES)
LINKER_FLAGS = -dynamiclib -shared -rdynamic -nodefaultlibs -pie -fPIC -flat_namespace -undefined warning
OBJECTS = iX-Yoke-plugin.o Preferences.o Server.o Window.o ThirdParty/XPListBox.o ThirdParty/XPPopups.o
PRODUCT = Wi-Fly-$(PLATFORM).xpl


main: objects
	gcc $(LINKER_FLAGS) -o $(PRODUCT) $(OBJECTS)

objects: $(OBJECTS)

clean-objects:
	rm -vf $(OBJECTS)

clean: clean-objects
	rm -vf $(PRODUCT)

