\page screenshot Screenshot

Previous Tutorial: \ref examples

## Overview

Screenshots of the current 3D scene can be taken using the screenshot plugin.
The default directory for saved screenshots is in `~/.gz/gui/pictures` and
the filename will be the timestamp of when the screenshot was taken.
It is possible to change the default directory to a different location, which will
be shown in the example below.

**Note**: This example is using Gazebo Sim but is not required.
The same steps can be done using `gz gui` with a 3D scene.

First, go to the plugins menu on the top right (vertical ellipsis) then select
'Screenshot':

@image html images/screenshot_menu.png

The Screenshot plugin will appear in the right panel:

@image html images/screenshot_plugin.png

To take a screenshot of the 3D scene, click on the camera icon:

@image html images/screenshot_button.png

The default location of saved screenshots is `~/.gz/gui/pictures`, to change
this click on the folder icon:

@image html images/screenshot_change_directory.png

A file dialog menu will appear and a new save location can be selected.
Once selected, all new screenshots will be saved in the chosen directory.
