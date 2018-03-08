# SGI - a scene graph inspector
Copyright 2012-2018 FAST Protect GmbH, Andreas Roth

SGI is primarily designed as tool to investigate and manipulate an OSG scene graph. It can be used to investigate other 
object hierarchies as well. 

## Design
- core part is located in osgSGI which includes
  - plugin management
  - implementation of SceneGraphDialog
  - implementation of ImagePreviewDialog
  - implementation of ContextMenu
  - implementation of ObjectLoggerDialog
- SGI reader/writer; loads the SGI using a filename (.sgi_loader)
- plugins; it ships with the following plugins
  - OSG
  - OsgEarth
  - Qt5
  - log4cplus
- applications;
  - helloWorldQt: sample application to use SGI in a Qt5 application
  - sgi_osgviewer: adapted version of osgviewer to use SGI
  - sgi_osgviewerQt: adapted version of osgviewer using Qt5 and loading SGI

## License
SGI is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
