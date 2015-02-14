#pragma once

#include <QMenu>
#include "sgi/ContextMenu"
#include "sgi/ContextMenuQt"

namespace sgi {

class QtSGIItem;

class ContextMenu : public QMenu
{
    Q_OBJECT
public:
                                    ContextMenu(bool onlyRootItem=true, QWidget *parent = 0);
                                    ContextMenu(SGIItemBase * item, IContextMenuInfo* info, bool onlyRootItem=true, QWidget *parent=0);
    virtual					        ~ContextMenu();

public:
    IContextMenu *                  menuInterface() { return _interface; }
    void                            setObject(SGIItemBase * item, IContextMenuInfo * info=NULL);
    void                            setObject(const SGIHostItemBase * item, IContextMenuInfo * info=NULL);
    IContextMenuInfo *              getInfo();

protected:
    void                            populateMenu(QMenu * menu, const SGIItemBase * item);

protected slots:
    void                            slotPopulateItemMenu();
    void                            slotSimpleItemAction();
    void                            slotBoolItemAction(bool newState);
    void                            slotActionGroup(QAction * action);

protected:
    void                            populate();

    QAction *                       addSimpleAction(QMenu * menu, const QString & text, const QtSGIItem & data, const char * trigger_method);
    QAction *                       addBoolAction(QMenu * menu, const QString & text, const QtSGIItem & data, bool value, const char * trigger_method);

    void							showSceneGraphDialog(SGIItemBase * item);

    class ContextMenuItem;
    class ContextMenuAction;
    class ContextMenuImpl;

protected:
    IContextMenuPtr      _interface;
    SGIItemBasePtr       _item;
    IContextMenuInfoPtr  _info;
    bool                 _onlyRootItem;
};

class ContextMenuQt : public QObject
{
public:
    ContextMenuQt(QObject * item, IContextMenuInfoQt* info, bool onlyRootItem=true, QWidget *parent=0);
    virtual ~ContextMenuQt();

public:
    IContextMenuQt *                menuInterface() { return _interface; }
    void                            setObject(QObject * item, IContextMenuInfoQt * info=NULL);
    IContextMenuInfoQt *            getInfo();

    QWidget *                       parentWidget();
    QMenu *                         getMenu();

protected:
    class ContextMenuQtImpl;

protected:
    IContextMenuQtPtr       _interface;
    SGIItemBasePtr          _item;
    QObject *               _qobject;
    IContextMenuInfoQtPtr   _info;
    bool                    _onlyRootItem;
    IContextMenuPtr         _realMenu;
};

} // namespace sgi
