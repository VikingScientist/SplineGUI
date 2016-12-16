# SplineGUI

Graphical user interface to visualize and work with [GoTools](https://github.com/SINTEF-Geometry/GoTools) (.g2) files

##  Introduction

The Spline GUI project is an easy-to-access graphical user interface (GUI) which allows for interactions with GoTools object of the type SplineCurve, SplineSurface and SplineVolume. It is designed to be general purpose in the sense that in its current form, it is only giving the user visualisation, and the means to select curves, surfaces or volumes. Any additional functionality must be implemented by the user.

## Overview

The GUI is accessed through the SplineGUI class. This is in turn relying on old C-code from the GLUT library. Due to the nature of the Object-oriented SplineGUI class and the procedural GLUT libraries a workaround has to be made to allow these to talk smoothly. This is the Workaround_namespace. It should not under any circumstances be necessary to use any of the functions or variables declared in this namespace. All interactions with the GUI should go through the SplineGUI class. See main.cpp for the simplest way of initializing the GUI.

The controls of the GUI are as follows
  * space : maximize the current viewport
  * c : display the control mesh
  * q : quit program
  * Right mousebutton : rotate
  * Right mousebutton + ctrl : zoom
  * Right mousebutton + shift : pan

## Library requirements

The project relies on the use of openGL, and specifically the openGL Utility Toolkit (GLUT) for the visualisation. These libraries must be installed on the local computer. It is also dependent on the following GoTools libraries: Trivariate and Core.
