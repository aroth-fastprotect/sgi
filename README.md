# SGI - a scene graph inspector
Copyright 2012-2018 FAST Protect GmbH, Andreas Roth

SGI is primarily designed as tool to investigate and manipulate an OSG scene graph or Qt object structure. It can be used to investigate other
object hierarchies as well.

## Using within an OSG application
- Within the application code load the SGI by
```
    osg::ref_ptr<osg::Node> sgi_loader = osgDB::readRefNodeFile(".sgi_loader", opts);
    if(sgi_loader.valid())
        existing_scene_graph->addChild(sgi_loader);
```
- The sgi_loader node can be inserted in any place within the scene graph, but it's recommended
  to add it somewhere close to the top of the main view camera. This ensures that the SGI loader
  receives all events and can catch the event to open the SGI dialogs or context menu.
- For more details and options how to load SGI please check the osgviewer and osgviewerQt examples.


## Using within a Qt application
- If the SGI library is located in a directory not included in the default library search path you can
  add this directory manually using the following code:
```
    QString path = QCoreApplication::applicationDirPath();
    path = QDir::cleanPath(path + SGI_QT_PLUGIN_DIR);
    QCoreApplication::addLibraryPath(path);
```
- Within the application code load the SGI by
```
    QImage load_sgi;
    QBuffer dummyMem;
    load_sgi.load(&dummyMem, "sgi_loader");
```
- For more details and options how to load SGI please check the helloWorldQt example.

## Usage
If the SGI library is loaded an event filter is in place to allow to access the object hierarchy of an
application starting by the context menu. It can be opened by **Shift+Ctrl+Right-Click** somewhere in the application.
A context menu for the object under the mouse cursor should appear. It contains action for the currenct object and
sub-menus for all child objects. Using the context menu you can go though the objects and then show more detail by
selecting *"Object info..."*. This opens the SceneGraphDialog which shows the select object in a tree on the left and
a textual representation of the object on the right. The tree also contain several *"global"* objects, like the
global application object or some useful singleton objects (like *osgDB::Registry*).

## Design
- core part is located in osgSGI which includes
  - plugin management
  - implementation of SceneGraphDialog
  - implementation of ImagePreviewDialog
  - implementation of ContextMenu
  - implementation of ObjectLoggerDialog
- SGI reader/writer; loads the SGI using a filename (*.sgi_loader*)
- plugins; it ships with the following plugins
  - OSG
  - OsgEarth
  - Qt5
  - log4cplus
- applications;
  - helloWorldQt: sample application to use SGI in a Qt5 application
  - sgi_osgviewer: adapted version of osgviewer to use SGI
  - sgi_osgviewerQt: adapted version of osgviewer using Qt5 and loading SGI

## Compilers
The following compilers have been tested so far:
- Visual Studio 2015 and 2017
- GCC 5.x, 6.x, 7.x
- Clang 4.x, 5.x, 6.x

## Notes
SGI is indented as debugging tool. Using SGI to manipulate any data during the runtime of an
application might cause data corruption, instability or even crashes.
Within some plugins use programming-tricks to access certain data field and properties without
taking the proper channels. For example fields marked as protected are accessed in a manner not
intended by the designer of the code.


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
