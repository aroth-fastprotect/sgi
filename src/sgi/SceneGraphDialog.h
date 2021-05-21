#pragma once

#include <QDialog>
#include <sgi/plugins/SceneGraphDialog>

QT_BEGIN_NAMESPACE
class QTimer;
class QTreeWidgetItem;
class QToolBar;
class QSpinBox;
class QComboBox;
QT_END_NAMESPACE

namespace sgi {

class IContextMenuItem;
class IContextMenu;
typedef details::ref_ptr<IContextMenu> IContextMenuPtr;
class IObjectTreeItem;
class ObjectTreeItem;
class ContextMenu;
typedef details::ref_ptr<IObjectTreeItem> IObjectTreeItemPtr;
class SGIItemBase;
class SGIHostItemBase;
class SGIDataItemBase;
typedef details::ref_ptr<SGIItemBase> SGIItemBasePtr;
typedef details::ref_ptr<SGIDataItemBase> SGIDataItemBasePtr;
typedef std::list<SGIDataItemBasePtr> SGIDataItemBasePtrList;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrPath;
typedef std::vector<SGIItemBasePtr> SGIItemBasePtrVector;

class SceneGraphDialog : public QDialog
{
	Q_OBJECT

public:
                            SceneGraphDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
                            SceneGraphDialog(SGIItemBase * item, IHostCallback * callback=nullptr, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
                            ~SceneGraphDialog() override;

public:
    ISceneGraphDialog *     dialogInterface() { return _interface; }
    IContextMenu *          toolsMenu();
    void                    setObject(const SGIHostItemBase * hostitem, IHostCallback * callback);
    void                    setObject(SGIItemBase * item, IHostCallback * callback);
    IObjectTreeItem *       selectedItem();
    IObjectTreeItem *       rootItem();
	SGIItemBase *           item() const;
	const SGIItemBasePtrPath & itemPath() const;
    void                    setInfoText(const std::string & text);

public slots:
    void					itemPrevious();
    void					itemNext();
    void					onObjectChanged(int tabIndex, SGIItemBase * item);

	void					onItemExpanded(QTreeWidgetItem * item);
	void					onItemCollapsed(QTreeWidgetItem * item);
    void					onItemClicked(QTreeWidgetItem * item, int column);
	void					onItemActivated(QTreeWidgetItem * item, int column);
	void					onItemContextMenu(QPoint pt);
    void					onItemSelectionChanged();

protected slots:
    void                    selectItemFromPath(int index);
    void                    refreshTimeChanged(int n);
    void                    refreshTimerExpired();
    void                    reload();
    void                    reloadSelectedItem();

    void                    tabChanged(int index);
    void                    addNewTab();
    void                    tabCloseRequest(int index);

    void                    showBesideParent();

signals:
    void                    triggerOnObjectChanged(int tabIndex, SGIItemBase * item);
    void                    triggerShow();
    void                    triggerHide();

protected:
    class HostCallback;
    class SceneGraphDialogImpl;
    class ToolsMenuImpl;
private:
    class Ui_SceneGraphDialog;
    class Ui_TabPage;

protected:
    bool                    buildRootTree(ObjectTreeItem * treeItem);
	bool                    buildTree(ObjectTreeItem * treeItem, SGIItemBase * item);
    bool                    populateToolsMenu(IContextMenuItem * menuItem);

	void					setNodeInfo(const SGIItemBase * item);

	void					triggerRepaint();
    SGIItemBase *           getView();
    bool                    newInstance(SGIItemBase * item);
    bool                    newInstance(const SGIHostItemBase * item);

    bool                    showObjectLoggerDialog(SGIItemBase * item);
    bool                    showObjectLoggerDialog(const SGIHostItemBase * item);

    void                    selectItemInPathBox();

    virtual void            closeEvent(QCloseEvent * event) override;
    virtual void            showEvent(QShowEvent * event) override;

private:
    void                    updatePathComboBox();

private:
	Ui_SceneGraphDialog *	ui;
    Ui_TabPage *            uiPage;

protected:  // for now
	SGIItemBasePtr                      _itemSelf;
    // use a simple raw-ptr to the interface to avoid a circular ref-ptr
    ISceneGraphDialog *                 _interface;
	IHostCallbackPtr                    _hostCallback;
    IContextMenuPtr                     _contextMenu;
    QTimer *                            _refreshTimer;
    ContextMenu *                       _toolsMenu;
    ISceneGraphDialogToolsMenuPtr       _toolsMenuInterface;
    SGIItemBasePtr                      _itemToolsMenu;
    bool                                _firstShow;
    SGIItemBasePtr                      _itemPending;
};

} // namespace sgi
