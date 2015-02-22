#pragma once

#include <QDialog>
#include <sgi/SceneGraphDialog>

QT_BEGIN_NAMESPACE
class Ui_SceneGraphDialog;
class QTimer;
class QTreeWidgetItem;
class QToolBar;
class QSpinBox;
class QComboBox;
QT_END_NAMESPACE

namespace sgi {

class IContextMenuItem;
class IObjectTreeItem;
class ObjectTreeItem;
class ContextMenu;
typedef osg::ref_ptr<IObjectTreeItem> IObjectTreeItemPtr;

class SceneGraphDialog : public QDialog
{
	Q_OBJECT

public:
							SceneGraphDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
							SceneGraphDialog(SGIItemBase * item, ISceneGraphDialogInfo * info=NULL, QWidget *parent = 0, Qt::WindowFlags f = 0);
	virtual					~SceneGraphDialog();

public:
    ISceneGraphDialog *     dialogInterface() { return _interface; }
    IContextMenu *          toolsMenu();
    void                    setObject(const SGIHostItemBase * hostitem, ISceneGraphDialogInfo * info);
    void                    setObject(SGIItemBase * item, ISceneGraphDialogInfo * info);
    IObjectTreeItem *       selectedItem();
    IObjectTreeItem *       rootItem();
    void                    setInfoText(const std::string & text);

public slots:
    void					itemPrevious();
    void					itemNext();
    void					onObjectChanged();

	void					onItemExpanded(QTreeWidgetItem * item);
	void					onItemCollapsed(QTreeWidgetItem * item);
    void					onItemClicked(QTreeWidgetItem * item, int column);
	void					onItemActivated(QTreeWidgetItem * item, int column);
	void					onItemContextMenu(QPoint pt);
    void                    onItemSelectionChanged();

protected slots:
    void                    selectItemFromPath(int index);
    void                    refreshTimeChanged(int n);
    void                    refreshTimerExpired();
    void                    reload();
    void                    reloadSelectedItem();

    void                    showBesideParent();

signals:
    void                    triggerOnObjectChanged();
    void                    triggerShow();
    void                    triggerHide();

protected:
    class ContextMenuCallback;
    class SceneGraphDialogImpl;
    class ToolsMenuImpl;

protected:
    bool                    buildRootTree(ObjectTreeItem * treeItem);
	bool                    buildTree(ObjectTreeItem * treeItem, SGIItemBase * item);
    bool                    populateToolsMenu(IContextMenuItem * menuItem);

	void					setNodeInfo(const SGIItemBase * item);

	void					triggerRepaint();
    SGIHostItemBase *       getView();
    bool                    newInstance(SGIItemBase * item);
    bool                    newInstance(const SGIHostItemBase * item);

    bool                    showObjectLoggerDialog(SGIItemBase * item);
    bool                    showObjectLoggerDialog(const SGIHostItemBase * item);

    void                    selectItemInPathBox();

private:
    void                    init();
    void                    updatePathComboBox();

private:
	Ui_SceneGraphDialog *	ui;

protected:  // for now
	SGIItemBasePtr                      _itemSelf;
    ISceneGraphDialogPtr                _interface;
    SGIItemBasePtr                      _item;
    SGIItemBasePtrPath                  _itemPath;
	osg::ref_ptr<ISceneGraphDialogInfo> _info;
    QToolBar *                          _toolBar;
    QAction *                           _actionReload;
    QAction *                           _actionReloadSelected;
    QAction *                           _actionItemPrevious;
    QAction *                           _actionItemNext;
    QComboBox *                         _comboBoxPath;
    osg::ref_ptr<IContextMenu>          _contextMenu;
    osg::ref_ptr<ContextMenuCallback>   _contextMenuCallback;
    QSpinBox *                          _spinBoxRefreshTime;
    QTimer *                            _refreshTimer;
    ContextMenu *                       _toolsMenu;
    ISceneGraphDialogToolsMenuPtr       _toolsMenuInterface;
    SGIItemBasePtr                      _itemToolsMenu;
    IObjectTreeItemPtr                  _rootTreeItem;
    IObjectTreeItemPtr                  _selectedTreeItem;
    bool                                _firstShow;
};

} // namespace sgi
