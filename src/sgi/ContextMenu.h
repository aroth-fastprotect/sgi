#pragma once

#include <QMenu>
#include "sgi/plugins/ContextMenu"
#include "sgi/ContextMenuQt"

namespace sgi {

namespace qt_helpers {
    class QtSGIItem;
}

class SGIItemBase;
class SGIHostItemBase;
class SGIDataItemBase;
typedef osg::ref_ptr<SGIItemBase> SGIItemBasePtr;
typedef osg::ref_ptr<SGIDataItemBase> SGIDataItemBasePtr;
typedef std::list<SGIDataItemBasePtr> SGIDataItemBasePtrList;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrVector;

class ContextMenu : public QMenu
{
    Q_OBJECT
public:
                                    ContextMenu(bool onlyRootItem=true, QWidget *parent = 0);
                                    ContextMenu(SGIItemBase * item, IHostCallback * callback, bool onlyRootItem=true, QWidget *parent=0);
    virtual					        ~ContextMenu();

public:
    IContextMenu *                  menuInterface() { return _interface; }
    void                            setObject(SGIItemBase * item, IHostCallback * callback=NULL);
    void                            setObject(const SGIHostItemBase * item, IHostCallback * callback=NULL);
    IHostCallback *                 getHostCallback();
    void                            popup(QWidget * parent, int x, int y);

protected:
    void                            populateMenu(QMenu * menu, const SGIItemBase * item);

protected slots:
    void                            slotPopulateItemMenu();
    void                            slotClearItemMenu();
    void                            slotSimpleItemAction();
    void                            slotBoolItemAction(bool newState);
    void                            slotActionGroup(QAction * action);

protected:
    void                            populate();

    QAction *                       addSimpleAction(QMenu * menu, const QString & text, const qt_helpers::QtSGIItem & data, const char * trigger_method);
    QAction *                       addBoolAction(QMenu * menu, const QString & text, const qt_helpers::QtSGIItem & data, bool value, const char * trigger_method);

    void							showSceneGraphDialog(SGIItemBase * item);

    class ContextMenuItem;
    class ContextMenuAction;
    class ContextMenuImpl;

protected:
    // use a simple raw-ptr to the interface to avoid a circular ref-ptr
    IContextMenu *       _interface;
    SGIItemBasePtr       _item;
    IHostCallbackPtr     _hostCallback;
    bool                 _onlyRootItem;
};

class ContextMenuQt : public QObject
{
public:
    ContextMenuQt(QObject * item, IHostCallback * callback, bool onlyRootItem=true, QWidget *parent=0);
    virtual ~ContextMenuQt();

public:
    IContextMenuQt *                menuInterface() { return _interface; }
    void                            setObject(QObject * item, IHostCallback * callback=NULL);
    IHostCallback *                 getHostCallback();

    QWidget *                       parentWidget();
    QMenu *                         getMenu();

protected:
    class ContextMenuQtImpl;

protected:
    // use a simple raw-ptr to the interface to avoid a circular ref-ptr
    IContextMenuQt *        _interface;
    SGIItemBasePtr          _item;
    IHostCallbackPtr        _hostCallback;
    QObject *               _qobject;
    bool                    _onlyRootItem;
    IContextMenuPtr         _realMenu;
};

} // namespace sgi
