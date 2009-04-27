X-Plane Plugin Project Template For Xcode 3.0
README.TXT
Jeff Reinecke (jeff@paploo.net)


============================== TABLE OF CONTENTS ===============================

This README contains the following sections:
+ Installation
+ Working With Different SDK Versions
+ Notes
+ Manual Project Creation
+ ToDo List
+ Acknowledgements
+ Change Notes


================================= INSTALLATION =================================

To install this template, do the following:

1. Verify that you have already installed Xcode v3.  Older versions of Xcode are
   NOT compatible with this template.

2. Navigate to "/Developer/Library/Xcode/Project Templates"

3. Create a folder called "X-Plane"

4. Move or Copy the "X-Plane Plugin" folder into your newly created "X-Plane"
   directory.
   
The next time you open Xcode, an X-Plane group should exist with this template
in it.  Note that if you already had Xcode open, you must re-open it to see this
template.


====================== WORKING WITH DIFFERENT SDK VERSIONS =====================

This template comes with the latest version of the SDK (as of May 2008; v2.00b2)
pre-installed in a folder called "SDK".

By default this is set-up to be backwards compatible to v1 of the SDK (so that
X-Plane v8 users can benefit from your hard work).  If you NEED to use v2 only
functionality, then uncomment the following line in your project:
  #define XPLM200


If you would like to use a different release of the SDK headers altogether, you
may either replace the contents of the SDK folder with another version of the
SDK, or change the target setting XPSDK_ROOT to point to a new SDK folder.

The key thing is to make sure that the XPSDK_ROOT is pointed at a folder that
contains a CHeader directory.  This directory is recursively searched for any
includes in your code.

Lastly, if you would like to make all new projects use a different version of
the SDK, you may change the project directly, using one of these methods.


===================================== NOTES ====================================

+ This project includes the Carbon libraries.  If you want your plugin
  to be compatible, avoid using Carbon calls whenever necessary.  However, Ben
  has pointed out that there are a few legitimate uses for Carbon API calls in
  your plugins.
  
+ A lot of examples turn of pre-compiled headers for the project.  As I link
  against Carbon, I have left these on so that recompiles may be quicker,
  however you can turn this off at anytime in the target settings.
  
+ If the undefined API warnings thrown by the linker get annoying, you can
  change the -undefined warning Other Linker Flag to -undefined suppress


============================ MANUAL PROJECT CREATION ===========================

There are several reasons why one would want to manually create a project.
Usually this is because you either possess a different version of Xcode or that
you want to use a different starting project to base your plugin off of, but may
also include a general distrust of what I've done.

To create a project with the same target settings as mine, you can do as
follows:

+ Create a new project using Dynamic Library > Carbon Dynamic Library as the
  starting point.
  
+ When changing the target's settings, be sure to change them for both release
  and debug!

+ Change the Base SDK Path to /Developer/SDKs/MacOSX10.4u.sdk so that Tiger user
  and Leopard users can run your plugin.
  
+ Change the Executable Extension to: xpl

+ Set the Other Linker Flags to: -flat_namespace -undefined warning
  Note that you can change warning to suppress if the warnings are bothering
  you.
  
+ Set the Header Search Paths to the following two entries:
    $(XPSDK_ROOT)/CHeaders
    $(HEADER_SEARCH_PATHS)
  Be SURE to make the CHEaders path search recursively, or you'll have to
  manually add all the sub paths as separate entries.
  
+ Set the Preprocessor Macros to: APL=1 IBM=0 LIN=0
  
+ Disable Fix & Continue.
  
+ Define the user constant XPSDK_ROOT and point it to the SDK.  If the SDK is
  in the project direcoty, this is just the name of the SDK directory.


================================== TODO LIST ===================================

+ Default to SDK v2 instead of v1.  We shouldn't do this until X-Plane v10 goes
  final at the earliest.


================================ ACKNOWLEDGMENTS ===============================

Thanks to Sandy Barbour for the large number of compilable examples he has
provided.  This template is based off of his hard work.

Thanks to Ben Supnik for all his help and hard work on the SDK.

Thanks to Bob Feaver for hosting this template so that others may enjoy it, and
for updating the original wiki instructions that lead me down this path.

Thanks to Ben Russell for posting the original wiki page instructions.


================================= CHANGE NOTES =================================

[May 6 2008] Jeff Reinecke
Updated/edited the README.TXT

[May 3 2008] Jeff Reinecke
Initial release.
