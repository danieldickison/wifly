Wi-Fly
=======

Website: http://danieldickison.com/wifly/
Contact: Daniel Dickison <danieldickison+wifly@gmail.com>

This is an X-Plane plugin and an iPhone app that lets you use the iPhone as a remote control yoke/joystick for X-Plane.

The X-Plane plugin acts as a server, to which the iPhone app sends commands.  This is currently implemented simply as a UDP socket with no verification or connection acknowledgement.


License
-------

Please note that the iPhone app (in the ios-app directory) and the X-Plane plugin are under two different open-source licenses.

The **iPhone app** is licensed under the **MIT** license.  See the file `ios-app/License.txt` for the full license.

The **X-Plane plugin** is licensed under the **GPL**.  See the file `xplane-plugin/GPL.txt` for the full license.

The X-Plane plugin also includes 3rd party code under separate licenses:

* bsd-strlcat.c copyright 1998 Todd C. Miller.  See that file for license info.
* Code from WidgetLibraryExample copyright 2005 Sandy Barbour and Ben Supnik.  See the file 'Third Party/license.txt' for license info.  This library is currently available online at http://www.xsquawkbox.net/xpsdk/mediawiki/XPWidgetsLibEx


TODO
----

### App ###

* Center and bounds ideas:
    - Two-finger drag for additional axes, e.g. 3-D cockpit panning.
    - Double-tap to re-center, or for tilt, set tilt center.
    - Double-tap to set current value as the tilt center?
    - Null-zone
* Change color/sound to alert when tilt exceeds tracking bounds.
* Auditory feedback of tap-drag for better blind interaction.
* Eye candy: background images, pulsating trackpad dot...etc.
* Landscape mode with multiple trackpads or buttons -- maybe something like the AR Drone Free Flight app.

### Plugin ###

* Make server update on every flight loop instead of a fixed rate (20Hz)?
* Auto-coordinate roll+yaw should move nosewheel steering more.
* Send instrument readings back to iPhone for display?
* Bonjour/zeroconf
