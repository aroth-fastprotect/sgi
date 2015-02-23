#include "stdafx.h"
#include "ContextMenu.h"

#include "sgi/ContextMenu"
#include "sgi/plugins/SGIPluginInterface.h"

#include "SGIPlugin.h"

#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/helpers/qt>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

using namespace qt_helpers;

class ContextMenu::ContextMenuItem : public IContextMenuItem
{
public:
    ContextMenuItem(ContextMenu * contextMenu, QMenu * menu, QActionGroup * actionGroup=NULL)
        : _contextMenu(contextMenu), _menu(menu), _actionGroup(actionGroup)
    {
    }
    ContextMenuItem(const ContextMenuItem & item)
        : _contextMenu(item._contextMenu), _menu(item._menu), _actionGroup(item._actionGroup)
    {
    }

    virtual ~ContextMenuItem()
    {
        _childs.clear();
    }

    void addSeparator()
    {
        _menu->addSeparator();
    }
    bool addSimpleAction(unsigned actionId, const std::string & name, const SGIHostItemBase * hostitem, osg::Referenced * userData=NULL)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(SGIPlugins::instance()->generateItem(item, hostitem))
            return addSimpleAction(actionId, name, item.get(), userData);
        else
            return false;
    }

    bool addSimpleAction(unsigned actionId, const std::string & name, SGIItemBase * item, osg::Referenced * userData=NULL)
    {
        QString itemText;
        if(name.empty())
        {
            std::string displayName;
            SGIPlugins::instance()->getObjectDisplayName(displayName, item);
            itemText = fromLocal8Bit(displayName);
        }
        else
            itemText = fromLocal8Bit(name);
        QtMenuSGIItem itemData(item);
        itemData.setActionId(actionId);
        itemData.setUserData(userData);

        QAction * action = new QAction(itemText, _menu);
        action->setData(QVariant::fromValue(itemData));
        _menu->addAction(action);
        Q_ASSERT(connect(action, SIGNAL(triggered()), _contextMenu, SLOT(slotSimpleItemAction())));
        return action;
    }
    bool addBoolAction(unsigned actionId, const std::string & name, SGIItemBase * item, bool state, osg::Referenced * userData=NULL)
    {
        QString itemText;
        if(name.empty())
        {
            std::string displayName;
            SGIPlugins::instance()->getObjectDisplayName(displayName, item);
            itemText = fromLocal8Bit(displayName);
        }
        else
            itemText = fromLocal8Bit(name);
        QtMenuSGIItem itemData(item);
        itemData.setActionId(actionId);
        itemData.setUserData(userData);

        QAction * action = new QAction(itemText, _menu);
        action->setData(QVariant::fromValue(itemData));
        action->setCheckable(true);
        action->setChecked(state);
        _menu->addAction(action);
        Q_ASSERT(connect(action, SIGNAL(triggered(bool)), _contextMenu, SLOT(slotBoolItemAction(bool))));
        return action;
    }

    bool addModeAction(const std::string & name, int mode, osg::Referenced * userData=NULL)
    {
        QString itemText = fromLocal8Bit(name);
        QtMenuSGIItem menuItemData = _menu->menuAction()->data().value<QtMenuSGIItem>();
        QtMenuSGIItem itemData(menuItemData);
        int currentMode = menuItemData.mode();
        itemData.setActionId(menuItemData.actionId());
        itemData.setMode(mode);
        itemData.setUserData(userData);

        QAction * action = new QAction(itemText, _menu);
        action->setActionGroup(_actionGroup);
        action->setData(QVariant::fromValue(itemData));
        action->setCheckable(true);
        action->setChecked(mode == currentMode);
        _menu->addAction(action);
        return action;
    }

    IContextMenuItem * addMenu(const std::string & name, SGIItemBase * item, osg::Referenced * userData=NULL)
    {
        return addMenuImpl(name, item, userData);
    }

    IContextMenuItem * addMenu(const std::string & name, const SGIHostItemBase * hostitem, osg::Referenced * userData=NULL)
    {
        osg::ref_ptr<SGIItemBase> item;
        if(SGIPlugins::instance()->generateItem(item, hostitem))
            return addMenuImpl(name, item.get(), userData);
        else
            return NULL;
    }

    IContextMenuItem * addModeMenu(unsigned actionId, const std::string & name, SGIItemBase * item, int currentMode, osg::Referenced * userData=NULL)
    {
        return addModeMenuImpl(actionId, name, item, currentMode, userData);
    }

    IContextMenuItem * getOrCreateMenu(const std::string & name, osg::Referenced * userData=NULL)
    {
        QMenu * menu = getOrCreateNamedMenu(_menu, fromLocal8Bit(name), userData);
        return addChild(new ContextMenuItem(_contextMenu, menu));
    }

    QMenu * menu() { return _menu; }
    const QMenu * menu() const { return _menu; }

protected:
    IContextMenuItem * addChild(ContextMenuItem * child)
    {
        _childs.push_back(child);
        return child;
    }
    IContextMenuItem * addMenuImpl(const std::string & name, SGIItemBase * item, osg::Referenced * userData)
    {
        QMenu * newMenu = new QMenu(_menu);
        QtMenuSGIItem itemData(item);
        itemData.setUserData(userData);
        QString itemText;
        if(name.empty())
        {
            std::string displayName;
            SGIPlugins::instance()->getObjectDisplayName(displayName, item);
            itemText = fromLocal8Bit(displayName);
        }
        else
            itemText = fromLocal8Bit(name);

        newMenu->setTitle(itemText);
        newMenu->menuAction()->setData(QVariant::fromValue(itemData));
        Q_ASSERT(connect(newMenu, SIGNAL(aboutToShow()), _contextMenu, SLOT(slotPopulateItemMenu())));
        // ... and finally add the new sub-menu to the menu
        _menu->addMenu(newMenu);
        return addChild(new ContextMenuItem(_contextMenu, newMenu));
    }

    IContextMenuItem * addModeMenuImpl(unsigned actionId, const std::string & name, SGIItemBase * item, int currentMode, osg::Referenced * userData)
    {
        QMenu * newMenu = new QMenu(_menu);
        QtMenuSGIItem itemData(item);
        itemData.setActionId(actionId);
        itemData.setMode(currentMode);
        itemData.setUserData(userData);
        QString itemText;
        if(name.empty())
        {
            std::string displayName;
            SGIPlugins::instance()->getObjectDisplayName(displayName, item);
            itemText = fromLocal8Bit(displayName);
        }
        else
            itemText = fromLocal8Bit(name);

        QActionGroup * actionGroup = new QActionGroup(newMenu);
        actionGroup->setExclusive(true);
        Q_ASSERT(connect(actionGroup, SIGNAL(triggered(QAction *)), _contextMenu, SLOT(slotActionGroup(QAction *))));

        newMenu->setTitle(itemText);
        newMenu->menuAction()->setData(QVariant::fromValue(itemData));
        // ... and finally add the new sub-menu to the menu
        _menu->addMenu(newMenu);
        return addChild(new ContextMenuItem(_contextMenu, newMenu, actionGroup));
    }

    static QMenu * getOrCreateNamedMenu(QMenu * menu, const QString & name, osg::Referenced * userData=NULL)
    {
        QMenu * ret = NULL;
        QList<QAction*> actions = menu->actions();
        foreach(QAction * action, actions)
        {
            QMenu * submenu = action->menu();
            if(submenu && action->text() == name)
            {
                ret = submenu;
                break;
            }
        }
        if(!ret)
        {
            ret = menu->addMenu(name);
            if(ret)
            {
                QtMenuSGIItem itemData;
                itemData.setUserData(userData);
                ret->menuAction()->setData(QVariant::fromValue(itemData));
            }
        }
        return ret;
    }

private:
    ContextMenu * _contextMenu;
    QMenu * _menu;
    QActionGroup * _actionGroup;
    typedef std::vector<IContextMenuItemPtr> IContextMenuItemPtrVector;
    IContextMenuItemPtrVector _childs;
};

class ContextMenu::ContextMenuAction : public IContextMenuAction
{
public:
    ContextMenuAction(ContextMenu * contextMenu, QAction * action, bool state=false)
        : _contextMenu(contextMenu), _action(action), _state(state)
    {
    }
    ContextMenuAction(const ContextMenuAction & rhs)
        : _contextMenu(rhs._contextMenu), _action(rhs._action), _state(rhs._state)
    {
    }

    virtual SGIItemBase * item()
    {
        QtMenuSGIItem itemData = _action->data().value<QtMenuSGIItem>();
        return itemData.item();
    }
    virtual unsigned actionId()
    {
        QtMenuSGIItem itemData = _action->data().value<QtMenuSGIItem>();
        return itemData.actionId();
    }
    virtual unsigned mode()
    {
        QtMenuSGIItem itemData = _action->data().value<QtMenuSGIItem>();
        return itemData.mode();
    }

    virtual bool state()
    {
        return _state;
    }

    virtual IContextMenu * menu()
    {
        return _contextMenu->menuInterface();
    }
    virtual osg::Referenced * userData()
    {
        QtMenuSGIItem itemData = _action->data().value<QtMenuSGIItem>();
        return itemData.userData();
    }
    virtual const osg::Referenced * userData() const
    {
        QtMenuSGIItem itemData = _action->data().value<QtMenuSGIItem>();
        return itemData.userData();
    }
    virtual osg::Referenced * modeUserData()
    {
        QMenu * menu = dynamic_cast<QMenu*>(_action->parent());
        QAction * menuAction = menu?menu->menuAction():NULL;

        QtMenuSGIItem menuItemData;
        if(menuAction)
            menuItemData = menuAction->data().value<QtMenuSGIItem>();
        return menuItemData.userData();
    }
    virtual const osg::Referenced * modeUserData() const
    {
        QMenu * menu = dynamic_cast<QMenu*>(_action->parent());
        QAction * menuAction = menu?menu->menuAction():NULL;

        QtMenuSGIItem menuItemData;
        if(menuAction)
            menuItemData = menuAction->data().value<QtMenuSGIItem>();
        return menuItemData.userData();
    }

private:
    ContextMenu * _contextMenu;
    QAction * _action;
    bool _state;
};

class ContextMenu::ContextMenuImpl : public IContextMenu
{
public:
    ContextMenuImpl(ContextMenu * menu)
        : _menu(menu) {}

    virtual QWidget *               parentWidget() { return _menu->parentWidget(); }
    virtual QMenu *                 getMenu() { return _menu; }
    virtual void                    setObject(SGIItemBase * item, IContextMenuInfo * info=NULL) { _menu->setObject(item, info); }
    virtual void                    setObject(const SGIHostItemBase * item, IContextMenuInfo * info=NULL) { _menu->setObject(item, info); }
    virtual IContextMenuInfo *      getInfo() { return _menu->getInfo(); }

private:
    ContextMenu * _menu;
};

ContextMenu::ContextMenu(bool onlyRootItem, QWidget * parent)
    : QMenu(parent)
    , _interface(new ContextMenuImpl(this))
    , _item()
    , _info(NULL)
    , _onlyRootItem(onlyRootItem)
{
    Q_ASSERT(connect(this, SIGNAL(aboutToShow()), this, SLOT(slotPopulateItemMenu())));
}

ContextMenu::ContextMenu(SGIItemBase * item, IContextMenuInfo* info, bool onlyRootItem, QWidget *parent)
    : QMenu(parent)
    , _interface(new ContextMenuImpl(this))
    , _item(item)
    , _info(info)
    , _onlyRootItem(onlyRootItem)
{
    populate();
    Q_ASSERT(connect(this, SIGNAL(aboutToShow()), this, SLOT(slotPopulateItemMenu())));
}

ContextMenu::~ContextMenu()
{
}

void ContextMenu::showSceneGraphDialog(SGIItemBase * item)
{
    if(_info)
        _info->showSceneGraphDialog(item);
    else
        SGIPlugins::instance()->showSceneGraphDialog(parentWidget(), item, NULL);
}

void ContextMenu::populate()
{
    if(!_item.valid())
        return;
    std::string displayName;
    SGIPlugins::instance()->getObjectDisplayName(displayName, _item.get());
    setTitle(fromLocal8Bit(displayName));

    QtMenuSGIItem data(_item);
    menuAction()->setData(QVariant::fromValue(data));
}

IContextMenuInfo * ContextMenu::getInfo()
{
    return _info;
}

void ContextMenu::slotPopulateItemMenu()
{
    QMenu * menu = qobject_cast<QMenu *>(sender());
    if(menu)
    {
        QAction * action = menu->menuAction();
        QtMenuSGIItem itemData = action->data().value<QtMenuSGIItem>();
        if(itemData.hasItem())
        {
            ContextMenuItem menuItem(this, menu);
            menu->blockSignals(true);
            menu->clear();
            bool onlyRootItem = (menu == this)?_onlyRootItem:true;
            SGIPlugins::instance()->contextMenuPopulate(&menuItem, itemData.item(), onlyRootItem);
            menu->blockSignals(false);
        }
    }
}

void ContextMenu::slotSimpleItemAction()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action)
    {
        QtMenuSGIItem itemData = action->data().value<QtMenuSGIItem>();
        IContextMenuActionPtr menuAction = new ContextMenuAction(this, action);
        SGIPlugins::instance()->contextMenuExecute(menuAction, itemData.item());
    }
}

void ContextMenu::slotBoolItemAction(bool newState)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(action)
    {
        QtMenuSGIItem itemData = action->data().value<QtMenuSGIItem>();
        IContextMenuActionPtr menuAction = new ContextMenuAction(this, action, newState);
        SGIPlugins::instance()->contextMenuExecute(menuAction, itemData.item());
    }
}

void ContextMenu::slotActionGroup(QAction * action)
{
    if(action)
    {
        QtMenuSGIItem itemData = action->data().value<QtMenuSGIItem>();
        IContextMenuActionPtr menuAction = new ContextMenuAction(this, action);
        SGIPlugins::instance()->contextMenuExecute(menuAction, itemData.item());
    }
}

void ContextMenu::setObject(SGIItemBase * item, IContextMenuInfo * info)
{
    _item = item;
    _info = info;
    clear();
    populate();
}

void ContextMenu::setObject(const SGIHostItemBase * hostitem, IContextMenuInfo * info)
{
    SGIItemBasePtr item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        setObject(item.get(), info);
}



class ContextMenuQt::ContextMenuQtImpl : public IContextMenuQt
{
public:
    ContextMenuQtImpl(ContextMenuQt * menu)
        : _menu(menu) {}

    virtual QWidget *               parentWidget() { return _menu->parentWidget(); }
    virtual QMenu *                 getMenu() { return _menu->getMenu(); }
    virtual void                    setObject(QObject * item, IContextMenuInfoQt * info=NULL) { _menu->setObject(item, info); }
    virtual IContextMenuInfoQt *    getInfo() { return _menu->getInfo(); }

private:
    ContextMenuQt * _menu;
};

ContextMenuQt::ContextMenuQt(QObject * qobject, IContextMenuInfoQt* info, bool onlyRootItem, QWidget *parent)
    : QObject(parent)
    , _interface(new ContextMenuQtImpl(this))
    , _item()
    , _qobject(qobject)
    , _info(info)
    , _onlyRootItem(onlyRootItem)
{
    SGIHostItemQt hostItem(qobject);
    SGIPlugins::instance()->generateItem(_item, &hostItem);
    ContextMenu * realMenu = new ContextMenu(_item.get(), NULL, onlyRootItem, parent);
    _realMenu = realMenu->menuInterface();
}

ContextMenuQt::~ContextMenuQt()
{
}

IContextMenuInfoQt * ContextMenuQt::getInfo()
{
    return _info;
}

QWidget * ContextMenuQt::parentWidget()
{
    return _realMenu->parentWidget();
}

QMenu * ContextMenuQt::getMenu()
{
    return _realMenu->getMenu();
}

void ContextMenuQt::setObject(QObject * qobject, IContextMenuInfoQt * info)
{
    SGIHostItemQt hostItem(qobject);
    SGIPlugins::instance()->generateItem(_item, &hostItem);
    _info = info;
    _realMenu->setObject(_item, NULL);
}


} // namespace sgi
