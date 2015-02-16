# SGI - a scene graph inspector

SGI is primarily designed as tool to investigate and manipulate an OSG scene graph. It can be used to investigate other 
object hierarchies as well. 

## Design
- core part is located in libSGI which includes
  - plugin management
  - implementation of SceneGraphDialog
  - implementation of ContextMenu
  - implementation of ObjectLoggerDialog
- SGI reader/writer; loads the SGI using a filename (.sgi_loader)
- plugins; it ships with the following plugins
  - OSG
  - OsgEarth
  - qt (should work with Qt4 and Qt5)
  - log4cplus
