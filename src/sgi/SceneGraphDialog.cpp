#include "stdafx.h"

#include "SceneGraphDialog.h"
#include "SceneGraphDialog.moc"
#include <ui_SceneGraphDialog.h>

#include <iostream>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QSpinBox>
#include <QDesktopWidget>
#include <QTimer>

#include "sgi/plugins/SGIPluginInterface.h"
#include "sgi/plugins/SGIProxyItem.h"

#include "SGIPlugin.h"
#include "sgi/SGIItemInternal"

#include <sgi/plugins/ObjectTreeImpl>
#include <sgi/helpers/qt>
#include "ContextMenu.h"

#include <sgi/plugins/SGIHostItemInternal.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

using namespace qt_helpers;


class SceneGraphDialog::HostCallback : public HostCallbackFilterT<ISceneGraphDialog>
{
public:
   HostCallback(IHostCallback * original, SceneGraphDialog * dialog)
        : HostCallbackFilterT<ISceneGraphDialog>(original, dialog->_interface) {}
};

class SceneGraphDialog::SceneGraphDialogImpl : public ISceneGraphDialog
{
public:
    SceneGraphDialogImpl(SceneGraphDialog * dialog)
        : _dialog(dialog) {}
    virtual                 ~SceneGraphDialogImpl() { delete _dialog; }
    virtual QDialog *       getDialog() { return _dialog; }
    virtual IHostCallback * getHostCallback() { return _dialog->_hostCallback; }
    virtual IContextMenu *  toolsMenu() { return _dialog->toolsMenu(); }
    virtual void            setObject(SGIItemBase * item, IHostCallback * callback=NULL) { _dialog->setObject(item, callback); }
    virtual void            setObject(const SGIHostItemBase * item, IHostCallback * callback=NULL) { _dialog->setObject(item, callback); }
    virtual void            show() { emit _dialog->triggerShow(); }
    virtual void            hide() { emit _dialog->triggerHide(); }
    virtual bool            isVisible() { return _dialog->isVisible(); }
    virtual int             showModal() { return _dialog->exec(); }
    virtual IObjectTreeItem * selectedItem() { return _dialog->selectedItem(); }
    virtual IObjectTreeItem * rootItem() { return _dialog->rootItem(); }
    virtual void            setInfoText(const std::string & text) { return _dialog->setInfoText(text); }
	virtual SGIItemBase *   item() const { return _dialog->item(); }
	virtual const SGIItemBasePtrPath & itemPath() const { return _dialog->itemPath(); }

    SceneGraphDialog * _dialog;
};

class SceneGraphDialog::ToolsMenuImpl : public ISceneGraphDialogToolsMenu
{
public:
    ToolsMenuImpl(SceneGraphDialog * dialog)
        : _dialog(dialog) {}
    virtual                     ~ToolsMenuImpl() {}
    virtual ISceneGraphDialog * getDialog() { return _dialog->dialogInterface(); }
    virtual IContextMenu *      toolsMenu() { return _dialog->toolsMenu(); }

    SceneGraphDialog * _dialog;
};

SceneGraphDialog::SceneGraphDialog(SGIItemBase * item, IHostCallback * callback, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
	, ui(NULL)
    , _interface(new SceneGraphDialogImpl(this))
    , _item(item)
	, _hostCallback(new HostCallback(callback, this))
    , _toolBar(NULL)
    , _actionReload(NULL)
    , _actionReloadSelected(NULL)
    , _actionItemPrevious(NULL)
    , _actionItemNext(NULL)
    , _contextMenu()
    , _spinBoxRefreshTime(NULL)
    , _refreshTimer(NULL)
    , _toolsMenu()
    , _toolsMenuInterface(new ToolsMenuImpl(this))
    , _itemToolsMenu()
    , _firstShow(true)
{
    init();
}

SceneGraphDialog::~SceneGraphDialog()
{
	if (ui)
	{
		delete ui;
		ui = NULL;
	}
    if (_toolsMenuInterface)
    {
        // tell interface that this instance is already gone, so no need to
        // delete again
        static_cast<ToolsMenuImpl*>(_toolsMenuInterface.get())->_dialog = NULL;
        _toolsMenuInterface = NULL;
    }
    if (_interface)
    {
        // tell interface that this instance is already gone, so no need to
        // delete again
        static_cast<SceneGraphDialogImpl*>(_interface)->_dialog = NULL;
        _interface = NULL;
    }
}

void SceneGraphDialog::init()
{
    ui = new Ui_SceneGraphDialog;
    ui->setupUi( this );

    Qt::WindowFlags flags =this->windowFlags()
        | Qt::WindowMinimizeButtonHint
        | Qt::WindowMaximizeButtonHint
        | Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    _toolBar = new QToolBar;
    QVBoxLayout * mainLayout = (QVBoxLayout *)this->layout();
    mainLayout->insertWidget(0, _toolBar);

    _comboBoxPath = new QComboBox(_toolBar);
    connect(_comboBoxPath, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &SceneGraphDialog::selectItemFromPath);

    _actionReloadSelected = new QAction(tr("Reload"), this);
    _actionReloadSelected->setToolTip(tr("Reload only the selected item and its childs"));
	_actionReloadSelected->setIcon(QIcon::fromTheme("edit-undo"));
    connect(_actionReloadSelected, &QAction::triggered, this, &SceneGraphDialog::reloadSelectedItem);

    _actionReload = new QAction(tr("Reload All"), this);
    _actionReload->setToolTip(tr("Reload all items in the tree and all their children"));
	_actionReload->setIcon(QIcon::fromTheme("system-reboot"));
	connect(_actionReload, &QAction::triggered, this, &SceneGraphDialog::reload);

    _actionItemPrevious = new QAction(tr("Previous"), this);
    _actionItemPrevious->setToolTip(tr("Select the previous item from the combo box"));
	_actionItemPrevious->setIcon(QIcon::fromTheme("arrow-left"));
    connect(_actionItemPrevious, &QAction::triggered, this, &SceneGraphDialog::itemPrevious);
    _actionItemNext = new QAction(tr("Next"), this);
    _actionItemNext->setToolTip(tr("Select the next item from the combo box"));
	_actionItemNext->setIcon(QIcon::fromTheme("arrow-right"));
	connect(_actionItemNext, &QAction::triggered, this, &SceneGraphDialog::itemNext);

    _spinBoxRefreshTime = new QSpinBox(_toolBar);
    _spinBoxRefreshTime->setToolTip(tr("Automatically reloads the information every X seconds."));
    _spinBoxRefreshTime->setMinimum(0);
    _spinBoxRefreshTime->setMaximum(600);
    _spinBoxRefreshTime->setPrefix("Refresh ");
    _spinBoxRefreshTime->setSuffix("s");
    connect(_spinBoxRefreshTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &SceneGraphDialog::refreshTimeChanged);

    _toolBar->addAction(_actionReloadSelected);
    _toolBar->addAction(_actionReload);
    _toolBar->addAction(_actionItemPrevious);
    _toolBar->addWidget(_comboBoxPath);
    _toolBar->addAction(_actionItemNext);
    _toolBar->addWidget(_spinBoxRefreshTime);

    _toolsMenu = new ContextMenu(false, this);

    QToolButton * toolsMenuButton = new QToolButton(this);
    toolsMenuButton->setMenu(_toolsMenu);
    toolsMenuButton->setText(tr("Tools"));
	toolsMenuButton->setIcon(QIcon::fromTheme("tool-measure"));
    toolsMenuButton->setPopupMode(QToolButton::InstantPopup);

    _toolBar->addWidget(toolsMenuButton);

    connect(this, &SceneGraphDialog::triggerOnObjectChanged, this, &SceneGraphDialog::onObjectChanged, Qt::QueuedConnection);
    connect(this, &SceneGraphDialog::triggerShow, this, &SceneGraphDialog::showBesideParent, Qt::QueuedConnection);
    connect(this, &SceneGraphDialog::triggerHide, this, &SceneGraphDialog::hide, Qt::QueuedConnection);
}

void SceneGraphDialog::closeEvent(QCloseEvent * event)
{
    ui->treeWidget->clear();
    ui->textEdit->clear();
    _comboBoxPath->clear();
    _toolsMenu->setObject((SGIItemBase*)NULL);
    _contextMenu = NULL;
    _rootTreeItem = NULL;
    _selectedTreeItem = NULL;
    _itemSelf = NULL;
    _itemToolsMenu = NULL;

    /*
    delete _comboBoxPath;
    _comboBoxPath = NULL;
    delete ui->treeWidget;
    ui->treeWidget = NULL;
    delete ui->textEdit;
    ui->textEdit = NULL;

    _toolsMenu->setObject((SGIItemBase*)NULL);
    delete _toolsMenu;
    _toolsMenu = NULL;
    _itemSelf = NULL;
    _interface = NULL;
    _item = NULL;
    _itemPath.clear();
    _hostCallback = NULL;
    _contextMenu = NULL;
    _toolsMenuInterface = NULL;
    _itemToolsMenu = NULL;
    _rootTreeItem = NULL;
    _selectedTreeItem = NULL;
    */

    QDialog::closeEvent(event);
}

void SceneGraphDialog::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);

    if(!_comboBoxPath->count())
        updatePathComboBox();

    if(!_rootTreeItem.valid())
        reload();
}


void SceneGraphDialog::showBesideParent()
{
    QWidget::show();
    if(_firstShow)
    {
        _firstShow = false;

        QDesktopWidget * dw = QApplication::desktop();
        QWidget * parent = parentWidget();
        if(parent)
        {
            int numScreens = dw->screenCount();
            int parentScreen = dw->screenNumber(parent);
            int currentScreen = dw->screenNumber(this);

            if(parentScreen == currentScreen)
            {
                int targetScreen = (currentScreen + 1) % numScreens;
                if(targetScreen != currentScreen)
                {
                    QRect geom = frameGeometry();
                    QRect currentScreenRect = dw->screenGeometry(currentScreen);
                    QRect targetScreenRect = dw->screenGeometry(targetScreen);
                    QPoint currentTopLeft = parent->mapToGlobal(geom.topLeft());
                    QPoint currentBottomRight = parent->mapToGlobal(geom.bottomRight());
                    QPoint screenOffset = currentTopLeft - currentScreenRect.topLeft();
                    QPoint targetTopLeft = targetScreenRect.topLeft() + screenOffset;
                    QPoint targetBottomRight(targetTopLeft.x() + geom.width(), targetTopLeft.y() + geom.height());
                    if (targetScreenRect.contains(targetTopLeft))
                    {
                        targetTopLeft = parent->mapFromGlobal(targetTopLeft);
                        move(targetTopLeft);
                    }
                }
            }
        }
    }
}

void SceneGraphDialog::onObjectChanged()
{
    updatePathComboBox();
    selectItemInPathBox();
    reload();
}

IContextMenu * SceneGraphDialog::toolsMenu()
{
    return _toolsMenu->menuInterface();
}

void SceneGraphDialog::setObject(const SGIHostItemBase * hostitem, IHostCallback * callback)
{
    SGIItemBasePtr item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        setObject(item.get(), callback);
}

void SceneGraphDialog::setObject(SGIItemBase * item, IHostCallback * callback)
{
    _item = item;
	if(callback)
		_hostCallback = callback;
    emit triggerOnObjectChanged();
}

void SceneGraphDialog::updatePathComboBox()
{
    _comboBoxPath->blockSignals(true);
    _comboBoxPath->clear();
    // first get the current path
    if(_item.valid())
        SGIPlugins::instance()->getObjectPath(_itemPath, _item);
    else
        _itemPath.clear();

    int currentItemIndex = -1;

    if(_itemPath.empty())
    {
        if(_item.valid())
        {
            std::string objectName;
            SGIPlugins::instance()->getObjectName(objectName, _item, true);
            QtSGIItem data(_item.get());
            _comboBoxPath->addItem(fromLocal8Bit(objectName), QVariant::fromValue(data));
            currentItemIndex = 0;
        }
    }
    else
    {
        for(SGIItemBasePtrPath::const_iterator it = _itemPath.begin(); it != _itemPath.end(); it++)
        {
            const SGIItemBasePtr & item = *it;
            std::string objectName;
            SGIPlugins::instance()->getObjectName(objectName, item, true);
			QtSGIItem data(item.get());
            _comboBoxPath->addItem(fromLocal8Bit(objectName), QVariant::fromValue(data));
            if(*item.get() == *_item.get())
                currentItemIndex = _comboBoxPath->count() - 1;
        }
    }
    int numItems = _comboBoxPath->count();
    _comboBoxPath->setEnabled(_itemPath.empty() == false);
    // trigger the drop-down box to adjust to the new content
    _comboBoxPath->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    _comboBoxPath->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _comboBoxPath->setCurrentIndex(currentItemIndex);
    _comboBoxPath->blockSignals(false);
    _actionItemPrevious->setEnabled(currentItemIndex > 0);
    _actionItemNext->setEnabled(currentItemIndex + 1 < numItems);
}

void SceneGraphDialog::reload()
{
    setCursor(Qt::WaitCursor);

    if(!_rootTreeItem.valid())
    {
        QTreeWidgetItem * root = ui->treeWidget->invisibleRootItem();
        QtSGIItem nodeDataRoot(SGIItemTypeTreeRoot, NULL, true);
        // set dummy data into the second column (type)
        root->setData(0, Qt::UserRole, QVariant::fromValue(nodeDataRoot));
        _rootTreeItem = new ObjectTreeItem(root, SGIPlugins::instance()->hostInterface());
    }

	if (_item.valid())
	{
		std::string displayName;
		SGIPlugins::instance()->getObjectDisplayName(displayName, _item);
		setWindowTitle(tr("Information about %1").arg(fromLocal8Bit(displayName)));
	}
	else
	{
		setWindowTitle(tr("No Information available"));
	}

    if (!_itemSelf.valid())
    {
        SGIHostItemInternal hostItemSelf(_interface);
        SGIPlugins::instance()->generateItem(_itemSelf, &hostItemSelf);
    }
    if (!_itemToolsMenu.valid())
    {
        SGIHostItemInternal hostItemToolsMenu(_toolsMenuInterface);
        SGIPlugins::instance()->generateItem(_itemToolsMenu, &hostItemToolsMenu);
        _toolsMenu->setObject(_itemToolsMenu);
    }

    ui->treeWidget->blockSignals(true);
	ui->treeWidget->clear();
	QList<int> panes_sizes;
	int total_width ;
	QLayout * currentLayout = ui->verticalLayout;
	total_width = this->width() - ui->verticalLayout->margin();
	const int tree_width = 3 * total_width / 5;
	const int textbox_width = 2 * total_width / 5;
	panes_sizes.append(tree_width);
	panes_sizes.append(textbox_width);
	ui->splitter->setSizes(panes_sizes);

	total_width = tree_width - 32;
	ui->treeWidget->setColumnWidth(0, 3 * total_width / 4);
	ui->treeWidget->setColumnWidth(1, total_width / 4);

    ObjectTreeItem * viewNodeItem = NULL;

    buildRootTree(static_cast<ObjectTreeItem *>(_rootTreeItem.get()));

	ObjectTreeItem * treeItem = NULL;
    if(_item.valid())
    {
        treeItem = static_cast<ObjectTreeItem *>(_rootTreeItem->addChild(std::string(), _item.get()));
    }

	if(treeItem)
	{
		treeItem->setSelected(true);
		onItemActivated(treeItem->treeItem(), 0);
        _selectedTreeItem = treeItem;
	}
    else if(viewNodeItem)
    {
        viewNodeItem->setSelected(true);
        onItemActivated(viewNodeItem->treeItem(), 0);
        _selectedTreeItem = viewNodeItem;
    }
    else
        _selectedTreeItem = _rootTreeItem;
    ui->treeWidget->blockSignals(false);

    QTreeWidgetItem * currentItem = ui->treeWidget->currentItem();
    if(currentItem)
        ui->treeWidget->expandItem(currentItem);
    setCursor(Qt::ArrowCursor);
}

void SceneGraphDialog::itemNext()
{
    if(!_itemPath.empty())
    {
        for(SGIItemBasePtrPath::const_iterator it = _itemPath.begin(); it != _itemPath.end(); it++)
        {
            if(*_item.get() == *(*it))
            {
                it++;
                if(it != _itemPath.end())
                    _item = *it;
                selectItemInPathBox();
                reload();
                break;
            }
        }
    }
}

void SceneGraphDialog::itemPrevious()
{
    if(!_itemPath.empty())
    {
        SGIItemBasePtr prev;
        for(SGIItemBasePtrPath::const_iterator it = _itemPath.begin(); it != _itemPath.end(); it++)
        {
            if(*_item.get() == *(*it))
            {
                if(prev)
                {
                    _item = prev;
                    selectItemInPathBox();
                    reload();
                }
                break;
            }
            else
                prev =*it;
        }
    }
}

void SceneGraphDialog::reloadSelectedItem()
{
    QTreeWidgetItem * item = _selectedTreeItem.valid()?((ObjectTreeItem*)_selectedTreeItem.get())->treeItem():NULL;
    if(item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        if(itemData.hasItem())
        {
            // we are going to re-populate the item with new data,
            // so first remove the old dummy child item.
            QList<QTreeWidgetItem *> children = item->takeChildren();
            for(QTreeWidgetItem * child : children)
            {
                delete child;
            }
        }
        ObjectTreeItem treeItem(item, SGIPlugins::instance()->hostInterface());
        buildTree(&treeItem, itemData.item());
    }
}

void SceneGraphDialog::onItemExpanded(QTreeWidgetItem * item)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
	if(!itemData.isPopulated() && itemData.hasItem())
    {
        // we are going to re-populate the item with new data,
        // so first remove the old dummy child item.
        QList<QTreeWidgetItem *> children = item->takeChildren();
        for(QTreeWidgetItem * child : children)
        {
            delete child;
        }
        ObjectTreeItem treeItem(item, SGIPlugins::instance()->hostInterface());
        buildTree(&treeItem, itemData.item());
	}
}

void SceneGraphDialog::onItemCollapsed(QTreeWidgetItem * item)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    //setNodeInfo(itemData.item());
}

void SceneGraphDialog::onItemClicked(QTreeWidgetItem * item, int column)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    setNodeInfo(itemData.item());
}

void SceneGraphDialog::onItemActivated(QTreeWidgetItem * item, int column)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    setNodeInfo(itemData.item());
}

void SceneGraphDialog::onItemSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidget->currentItem();
    IObjectTreeItemPtr oldItem = _selectedTreeItem;
    if (item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        _selectedTreeItem = new ObjectTreeItem(item, SGIPlugins::instance()->hostInterface());
    }
    else
        _selectedTreeItem = NULL;
    //d->_impl->itemSelected(oldItem.get(), _selectedTreeItem.get());
}

bool SceneGraphDialog::buildTree(ObjectTreeItem * treeItem, SGIItemBase * item)
{
    bool ret = SGIPlugins::instance()->objectTreeBuildTree(treeItem, item);
    // in any case mark the item as populated and add the internal item as well.
    // We ignore the result from any plugin here, because some plugin be not be
    // functioning as expected or no plugin might be available for this item. The
    // internal item is useful especially in these cases to investigate why the
    // call to build tree failed.
    InternalItemData internalItemData(item);;
    SGIHostItemInternal hostItemInternal(new ReferencedInternalItemData(internalItemData));
    treeItem->addChild("Internal", &hostItemInternal);

    QTreeWidgetItem * treeItemQt = treeItem->treeItem();
    QtSGIItem itemData = treeItemQt->data(0, Qt::UserRole).value<QtSGIItem>();
    itemData.markAsPopulated();
    treeItemQt->setData(0, Qt::UserRole, QVariant::fromValue(itemData));

    // but return the result of the buildTree call to the user
    return ret;
}

bool SceneGraphDialog::buildRootTree(ObjectTreeItem * treeItem)
{
    bool ret = false;
    if(_itemSelf.valid())
        ret = SGIPlugins::instance()->objectTreeBuildRootTree(treeItem, _itemSelf);
    return ret;
}

void SceneGraphDialog::setNodeInfo(const SGIItemBase * item)
{
    std::ostringstream os;
    if(item)
        SGIPlugins::instance()->writePrettyHTML(os, item);
    else
    {
        os << "<b>item is <i>NULL</i></b>";
    }
    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(fromLocal8Bit(os.str()));
    ui->textEdit->blockSignals(false);
}

void SceneGraphDialog::onItemContextMenu(QPoint pt)
{
	QTreeWidgetItem * item = ui->treeWidget->itemAt (pt);
	QtSGIItem itemData;
    if (item)
    {
        itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();

        QMenu * contextQMenu = NULL;
        IContextMenuPtr objectMenu = _hostCallback->contextMenu(this, itemData.item());
        if (!objectMenu)
        {
            if (_contextMenu)
            {
                _contextMenu->setObject(itemData.item());
                objectMenu = _contextMenu;
            }
            else
            {
                objectMenu = SGIPlugins::instance()->createContextMenu(this, itemData.item(), _hostCallback);
            }
        }

        if (objectMenu)
            contextQMenu = objectMenu->getMenu();

        _contextMenu = objectMenu;

        if (contextQMenu)
        {
            pt.ry() += ui->treeWidget->header()->height();
            QPoint globalPos = ui->treeWidget->mapToGlobal(pt);
            contextQMenu->popup(globalPos);
        }
    }
}

SGIItemBase * SceneGraphDialog::getView()
{
    if(_hostCallback)
        return _hostCallback->getView();
    else
        return NULL;
}

void SceneGraphDialog::triggerRepaint()
{
    if(_hostCallback)
        _hostCallback->triggerRepaint();
}

bool SceneGraphDialog::newInstance(SGIItemBase * item)
{
    bool ret;
    // only open a new instance when the object is different
    if(_item != item && *_item.get() != *item)
    {
        ISceneGraphDialog * dlg = SGIPlugins::instance()->showSceneGraphDialog(parentWidget(), item, _hostCallback);
        if(dlg)
            dlg->show();
        ret = (dlg != NULL);
    }
    else
    {
        // keep the current dialog open, but return success
        ret = true;
    }
    return ret;
}

bool SceneGraphDialog::newInstance(const SGIHostItemBase * hostitem)
{
    bool ret;
    osg::ref_ptr<SGIItemBase> item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        ret = newInstance(item.get());
    else
        ret = false;
    return ret;
}


bool SceneGraphDialog::showObjectLoggerDialog(SGIItemBase * item)
{
    return _hostCallback->showObjectLoggerDialog(this, item);
}

bool SceneGraphDialog::showObjectLoggerDialog(const SGIHostItemBase * hostitem)
{
    bool ret;
    osg::ref_ptr<SGIItemBase> item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        ret = showObjectLoggerDialog(item.get());
    else
        ret = false;
    return ret;
}

void SceneGraphDialog::selectItemInPathBox()
{
    int selectIndex = -1;
    _comboBoxPath->blockSignals(true);
    int numItems = _comboBoxPath->count();
    for(int i = 0; selectIndex < 0 && i < numItems; i++)
    {
		QtSGIItem data = _comboBoxPath->itemData(i).value<QtSGIItem>();
        if(*data.item() == *_item.get())
        {
            selectIndex = i;
            break;
        }
    }
    _actionItemPrevious->setEnabled(selectIndex > 0);
    _actionItemNext->setEnabled(selectIndex + 1 < numItems);
    _comboBoxPath->setCurrentIndex(selectIndex);
    _comboBoxPath->blockSignals(false);
}

void SceneGraphDialog::selectItemFromPath(int index)
{
	QtSGIItem data = _comboBoxPath->itemData(index).value<QtSGIItem>();
	_item = data.item();
    _actionItemPrevious->setEnabled(index > 0);
    _actionItemNext->setEnabled(index + 1 < _comboBoxPath->count());
    reload();
}

void SceneGraphDialog::refreshTimeChanged ( int n )
{
    if(!_refreshTimer)
    {
        _refreshTimer = new QTimer(this);
        connect(_refreshTimer, &QTimer::timeout, this, &SceneGraphDialog::refreshTimerExpired);
    }
    if(n > 0)
        _refreshTimer->start(n * 1000);
    else
        _refreshTimer->stop();
}

void SceneGraphDialog::refreshTimerExpired()
{
    QTreeWidgetItem * item = ui->treeWidget->currentItem();
    if(item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        setNodeInfo(itemData.item());
    }
}

IObjectTreeItem * SceneGraphDialog::selectedItem()
{
    return _selectedTreeItem.get();
}

IObjectTreeItem * SceneGraphDialog::rootItem()
{
    return _rootTreeItem.get();
}

SGIItemBase * SceneGraphDialog::item() const
{
	return _item.get();
}

const SGIItemBasePtrPath & SceneGraphDialog::itemPath() const
{
	return _itemPath;
}

void SceneGraphDialog::setInfoText(const std::string & text)
{
    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(fromLocal8Bit(text));
    ui->textEdit->blockSignals(false);
}

} // namespace sgi
