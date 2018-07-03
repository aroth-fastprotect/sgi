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
typedef details::ref_ptr<SGIItemBase> SGIItemBasePtr;
typedef details::ref_ptr<SGIDataItemBase> SGIDataItemBasePtr;
typedef std::list<SGIDataItemBasePtr> SGIDataItemBasePtrList;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrVector;

class ContextMenuBase : public QMenu
{
    Q_OBJECT
public:
    ContextMenuBase(QWidget *parent = nullptr);
    ~ContextMenuBase() override;
signals:
    void hidden();

protected:
    void hideEvent(QHideEvent *event) override;
};

class ContextMenu : public ContextMenuBase
{
    Q_OBJECT
public:
                                    ContextMenu(bool onlyRootItem=true, QWidget *parent = nullptr);
                                    ContextMenu(SGIItemBase * item, IHostCallback * callback, bool onlyRootItem=true, QWidget *parent=nullptr);
    virtual					        ~ContextMenu();

public:
    IContextMenu *                  menuInterface() { return _interface; }
    void                            setObject(SGIItemBase * item, IHostCallback * callback=nullptr);
    void                            setObject(const SGIHostItemBase * item, IHostCallback * callback=nullptr);
    IHostCallback *                 getHostCallback();

    bool                            donotClearItem() const {
        return _donotClearItem;
    }
    void                            setDonotClearItem(bool enable) {
        _donotClearItem = enable;
    }

signals:
	void                            triggerPopup(QWidget * parent, int x, int y);
	void							triggerUpdateMenu();

protected:
    void                            populateMenu(QMenu * menu, const SGIItemBase * item);

protected slots:
	void                            popup(QWidget * parent, int x, int y);
	void							updateMenu();
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
    bool                 _donotClearItem;
};

class ContextMenuQt : public QObject
{
    Q_OBJECT

public:
    ContextMenuQt(QObject * item, IHostCallback * callback, bool onlyRootItem=true, QWidget *parent=nullptr);
    virtual ~ContextMenuQt();

public:
    IContextMenuQt *                menuInterface() { return _interface; }
    void                            setObject(QObject * item, IHostCallback * callback=nullptr);
    IHostCallback *                 getHostCallback();

    QWidget *                       parentWidget();
    QMenu *                         getMenu();
    void                            popup(QWidget * parent, int x, int y);

protected:
    class ContextMenuQtImpl;

protected:
    // use a simple raw-ptr to the interface to avoid a circular ref-ptr
    IContextMenuQt *        _interface;
    IHostCallbackPtr        _hostCallback;
    QObject *               _qobject;
    bool                    _onlyRootItem;
    IContextMenuPtr         _realMenu;
};

} // namespace sgi
