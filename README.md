# devchat-gui
This is a third-party client for EGOSOFT's DevNet chat. It's based on Gtk+ and offers (or is planned to offer) several improvements over the normal browser client, e.g. tabbed PMs and several graphical improvements (Avatars in the user list, themeable colors etc.). The original was written by Samuel Creshal, who eventually abandoned the project causing the client to cease working after an EGOSOFT server update.

## Installation
If you're using Linux or Unix(untested), installation is straight forward. Dependencies are:
pkg-config, gtk2, libsoup, gtkspell, libnotify and libxml2
`./configure`
`make`
and if you want to install it system-wide
`make install`

If you are a Windows user, see Cresh's old INSTALL file to get an idea how fucked you are trying to complile this under Windows.

