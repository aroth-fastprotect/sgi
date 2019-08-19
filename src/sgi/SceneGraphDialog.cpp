#include "stdafx.h"

#include "SceneGraphDialog.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

#include <iostream>
#include <QToolBar>
#include <QToolButton>
#include <QComboBox>
#include <QSpinBox>
#include <QDesktopWidget>
#include <QScrollBar>
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
    ~SceneGraphDialogImpl() override
    {
        delete _dialog;
    }
    QDialog *       getDialog() override { return _dialog; }
    IHostCallback * getHostCallback() override { return _dialog->_hostCallback.get(); }
    IContextMenu *  toolsMenu() override { return _dialog->toolsMenu(); }
    void            setObject(SGIItemBase * item, IHostCallback * callback=nullptr) override { _dialog->setObject(item, callback); }
    void            setObject(const SGIHostItemBase * item, IHostCallback * callback=nullptr) override { _dialog->setObject(item, callback); }
    void            show() override { emit _dialog->triggerShow(); }
    void            hide() override { emit _dialog->triggerHide(); }
    bool            isVisible() override { return _dialog->isVisible(); }
    IObjectTreeItem * selectedItem() override { return _dialog->selectedItem(); }
    IObjectTreeItem * rootItem() override { return _dialog->rootItem(); }
    void            setInfoText(const std::string & text) override { return _dialog->setInfoText(text); }
    SGIItemBase *   item() const override { return _dialog->item(); }
    const SGIItemBasePtrPath & itemPath() const override { return _dialog->itemPath(); }

    SceneGraphDialog * _dialog;
};

class SceneGraphDialog::ToolsMenuImpl : public ISceneGraphDialogToolsMenu
{
public:
    ToolsMenuImpl(SceneGraphDialog * dialog)
        : _dialog(dialog) {}
    ~ToolsMenuImpl() override {}
    ISceneGraphDialog * getDialog() override { return _dialog->dialogInterface(); }
    IContextMenu *      toolsMenu() override { return _dialog->toolsMenu(); }

    SceneGraphDialog * _dialog;
};

class SceneGraphDialog::Ui_TabPage
{
public:
    QWidget *tabPageWidget;
    QSplitter *splitter;
    QTreeWidget *treeWidget;
    QTextBrowser *textEdit;

    SGIItemBasePtr item;
    SGIItemBasePtrPath itemPath;
    IObjectTreeItemPtr rootTreeItem;
    IObjectTreeItemPtr selectedTreeItem;

    Ui_TabPage()
        : tabPageWidget(nullptr)
        , splitter(nullptr)
        , treeWidget(nullptr)
        , textEdit(nullptr)
    {
    }
    ~Ui_TabPage()
    {
        delete treeWidget;
        delete textEdit;
        delete splitter;
        delete tabPageWidget;
    }
    void setupUi(SceneGraphDialog *dlg, QWidget * tabPage)
    {
        QVBoxLayout * verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(3, 3, 3, 3);

        splitter = new QSplitter(tabPage);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        treeWidget = new QTreeWidget(splitter);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("Name"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        splitter->addWidget(treeWidget);
        textEdit = new QTextBrowser(splitter);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setReadOnly(true);
        textEdit->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);
        textEdit->setOpenExternalLinks(true);
        splitter->addWidget(textEdit);

        QObject::connect(treeWidget, &QTreeWidget::itemExpanded, dlg, &SceneGraphDialog::onItemExpanded);
        QObject::connect(treeWidget, &QTreeWidget::itemCollapsed, dlg, &SceneGraphDialog::onItemCollapsed);
        QObject::connect(treeWidget, &QTreeWidget::itemActivated, dlg, &SceneGraphDialog::onItemActivated);
        QObject::connect(treeWidget, &QTreeWidget::customContextMenuRequested, dlg, &SceneGraphDialog::onItemContextMenu);
        QObject::connect(treeWidget, &QTreeWidget::itemClicked, dlg, &SceneGraphDialog::onItemClicked);
        QObject::connect(treeWidget, &QTreeWidget::itemSelectionChanged, dlg, &SceneGraphDialog::onItemSelectionChanged);

        verticalLayout->addWidget(splitter);
        tabPage->setLayout(verticalLayout);
        tabPageWidget = tabPage;
        QMetaObject::connectSlotsByName(dlg);
    }
    void clear()
    {
        treeWidget->clear();
        textEdit->clear();
        item = nullptr;
        itemPath.clear();
        rootTreeItem = nullptr;
        selectedTreeItem = nullptr;
    }
};

class SceneGraphDialog::Ui_SceneGraphDialog
{
public:
    QTabWidget * tabWidget;
    QVBoxLayout *verticalLayout;

    typedef std::vector<Ui_TabPage*> Ui_TabPageList;
    Ui_TabPageList tabs;

    QToolBar * toolBar;
    QComboBox * comboBoxPath;
    QSpinBox * spinBoxRefreshTime;

    QAction * actionAddTab;
    QAction * actionCloseTab;
    QAction * actionReload;
    QAction * actionReloadSelected;
    QAction * actionItemPrevious;
    QAction * actionItemNext;

    void setupUi(SceneGraphDialog *dlg)
    {
        if (dlg->objectName().isEmpty())
            dlg->setObjectName(QStringLiteral("SceneGraphDialog"));
        dlg->resize(800, 600);

        verticalLayout = new QVBoxLayout(dlg);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);

        tabWidget = new QTabWidget(dlg);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        connect(tabWidget, &QTabWidget::currentChanged, dlg, &SceneGraphDialog::tabChanged);

        toolBar = new QToolBar;
        verticalLayout->addWidget(toolBar);
        verticalLayout->addWidget(tabWidget);

        comboBoxPath = new QComboBox(toolBar);
        connect(comboBoxPath, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), dlg, &SceneGraphDialog::selectItemFromPath);

        actionAddTab = new QAction(tr("Add tab"), dlg);
        actionAddTab->setToolTip(tr("Add a new tab"));
        actionAddTab->setIcon(QIcon::fromTheme("tab-new"));
        connect(actionAddTab, &QAction::triggered, dlg, &SceneGraphDialog::addNewTab);

        actionCloseTab = new QAction(tr("Close tab"), dlg);
        actionCloseTab->setToolTip(tr("Close current tab"));
        actionCloseTab->setIcon(QIcon::fromTheme("tab-close"));
        connect(actionCloseTab, &QAction::triggered, dlg, &SceneGraphDialog::closeTab);

        actionReloadSelected = new QAction(tr("Reload"), dlg);
        actionReloadSelected->setToolTip(tr("Reload only the selected item and its childs"));
        actionReloadSelected->setIcon(QIcon::fromTheme("edit-undo"));
        connect(actionReloadSelected, &QAction::triggered, dlg, &SceneGraphDialog::reloadSelectedItem);

        actionReload = new QAction(tr("Reload All"), dlg);
        actionReload->setToolTip(tr("Reload all items in the tree and all their children"));
        actionReload->setIcon(QIcon::fromTheme("system-reboot"));
        connect(actionReload, &QAction::triggered, dlg, &SceneGraphDialog::reload);

        actionItemPrevious = new QAction(tr("Previous"), dlg);
        actionItemPrevious->setToolTip(tr("Select the previous item from the combo box"));
        actionItemPrevious->setIcon(QIcon::fromTheme("arrow-left"));
        connect(actionItemPrevious, &QAction::triggered, dlg, &SceneGraphDialog::itemPrevious);
        actionItemNext = new QAction(tr("Next"), dlg);
        actionItemNext->setToolTip(tr("Select the next item from the combo box"));
        actionItemNext->setIcon(QIcon::fromTheme("arrow-right"));
        connect(actionItemNext, &QAction::triggered, dlg, &SceneGraphDialog::itemNext);

        spinBoxRefreshTime = new QSpinBox(toolBar);
        spinBoxRefreshTime->setToolTip(tr("Automatically reloads the information every X milliseconds."));
        spinBoxRefreshTime->setMinimum(0);
        spinBoxRefreshTime->setMaximum(60000);
        spinBoxRefreshTime->setSingleStep(100);
        spinBoxRefreshTime->setPrefix("Refresh ");
        spinBoxRefreshTime->setSuffix("ms");
        connect(spinBoxRefreshTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), dlg, &SceneGraphDialog::refreshTimeChanged);

        toolBar->addAction(actionAddTab);
        toolBar->addAction(actionCloseTab);
        toolBar->addAction(actionReloadSelected);
        toolBar->addAction(actionReload);
        toolBar->addAction(actionItemPrevious);
        toolBar->addWidget(comboBoxPath);
        toolBar->addAction(actionItemNext);
        toolBar->addWidget(spinBoxRefreshTime);
    }

    Ui_TabPage * addTabPage(SceneGraphDialog * dlg)
    {
        QWidget * tabPage = new QWidget(dlg);
        tabPage->setObjectName(QStringLiteral("tabPage"));

        Ui_TabPage * page = new Ui_TabPage;
        page->setupUi(dlg, tabPage);
        tabs.push_back(page);

        tabWidget->addTab(tabPage, tr("New Tab"));
        tabWidget->setCurrentWidget(tabPage);
        return page;
    } // setupUi
};

SceneGraphDialog::SceneGraphDialog(SGIItemBase * item, IHostCallback * callback, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(nullptr)
    , _interface(new SceneGraphDialogImpl(this))
	, _hostCallback(new HostCallback(callback, this))
    , _contextMenu()
    , _refreshTimer(nullptr)
    , _toolsMenu()
    , _toolsMenuInterface(new ToolsMenuImpl(this))
    , _itemToolsMenu()
    , _firstShow(true)
{
    ui = new Ui_SceneGraphDialog;
    ui->setupUi(this);

    Qt::WindowFlags flags = this->windowFlags()
        | Qt::WindowMinimizeButtonHint
        | Qt::WindowMaximizeButtonHint
        | Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    _toolsMenu = new ContextMenu(false, this);
    _toolsMenu->setDonotClearItem(true);

    QToolButton * toolsMenuButton = new QToolButton(this);
    toolsMenuButton->setMenu(_toolsMenu);
    toolsMenuButton->setText(tr("Tools"));
    toolsMenuButton->setIcon(QIcon::fromTheme("tool-measure"));
    toolsMenuButton->setPopupMode(QToolButton::InstantPopup);

    ui->toolBar->addWidget(toolsMenuButton);

    connect(this, &SceneGraphDialog::triggerOnObjectChanged, this, &SceneGraphDialog::onObjectChanged, Qt::QueuedConnection);
    connect(this, &SceneGraphDialog::triggerShow, this, &SceneGraphDialog::showBesideParent, Qt::QueuedConnection);
    connect(this, &SceneGraphDialog::triggerHide, this, &SceneGraphDialog::hide, Qt::QueuedConnection);

    addNewTab();

    uiPage->item = item;
}

SceneGraphDialog::~SceneGraphDialog()
{
	if (ui)
	{
		delete ui;
        ui = nullptr;
	}
    if (_toolsMenuInterface.valid())
    {
        // tell interface that this instance is already gone, so no need to
        // delete again
        static_cast<ToolsMenuImpl*>(_toolsMenuInterface.get())->_dialog = nullptr;
        _toolsMenuInterface = nullptr;
    }
    if (_interface)
    {
        // tell interface that this instance is already gone, so no need to
        // delete again
        static_cast<SceneGraphDialogImpl*>(_interface)->_dialog = nullptr;
        _interface = nullptr;
    }
}

void SceneGraphDialog::closeEvent(QCloseEvent * event)
{
    if (_refreshTimer)
    {
        delete _refreshTimer;
        _refreshTimer = nullptr;
    }

    _itemPending = nullptr;
    for (Ui_TabPage * page : ui->tabs)
        page->clear();
    ui->comboBoxPath->clear();
    _toolsMenu->setObject(static_cast<SGIItemBase*>(nullptr));
    _contextMenu = nullptr;
    _itemToolsMenu = nullptr;
    //_itemSelf = nullptr;

    /*
    delete ui->comboBoxPath;
    ui->comboBoxPath = nullptr;
    delete ui->treeWidget;
    ui->treeWidget = nullptr;
    delete ui->textEdit;
    ui->textEdit = nullptr;

    _toolsMenu->setObject((SGIItemBase*)nullptr);
    delete _toolsMenu;
    _toolsMenu = nullptr;
    _itemSelf = nullptr;
    _interface = nullptr;
    _item = nullptr;
    _itemPath.clear();
    _hostCallback = nullptr;
    _contextMenu = nullptr;
    _toolsMenuInterface = nullptr;
    _itemToolsMenu = nullptr;
    uiPage->rootTreeItem = nullptr;
    uiPage->selectedTreeItem = nullptr;
    */

    QDialog::closeEvent(event);
}

void SceneGraphDialog::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);

    if(!ui->comboBoxPath->count())
        updatePathComboBox();

    if(!uiPage->rootTreeItem.valid())
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
                    //QPoint currentBottomRight = parent->mapToGlobal(geom.bottomRight());
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

void SceneGraphDialog::onObjectChanged(int tabIndex, SGIItemBase * item)
{
    if (uiPage && uiPage->item.get() == item)
        return;
    int existingTabIndex = tabIndex;
    for (int index = 0; item && existingTabIndex < 0 && index < (int)ui->tabs.size(); ++index)
    {
        Ui_TabPage * page = ui->tabs[index];
        if (page->item.get() == item)
            existingTabIndex = index;
    }
    if (existingTabIndex >= 0)
        ui->tabWidget->setCurrentIndex(existingTabIndex);
    else if(item)
    {
        addNewTab();
        uiPage->item = item;
    }
    else
    {
        // got nullptr as object/item, so clear the given tab page
        if(tabIndex >= 0 && tabIndex < static_cast<int>(ui->tabs.size()))
        {
            // clear out the specific tab page
            Ui_TabPage * page = ui->tabs[tabIndex];
            page->clear();
        }
        else
        {
            for (int index = 0; index < (int)ui->tabs.size(); ++index)
            {
                Ui_TabPage * page = ui->tabs[index];
                page->clear();
            }
        }
    }
    // release any grab on pending item
    _itemPending = nullptr;

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
    if (callback)
        _hostCallback = callback;

    _itemPending = item;
    emit triggerOnObjectChanged(-1, item);
}

void SceneGraphDialog::updatePathComboBox()
{
    ui->comboBoxPath->blockSignals(true);
    ui->comboBoxPath->clear();
    // first get the current path
    if(uiPage->item.valid())
        SGIPlugins::instance()->getObjectPath(uiPage->itemPath, uiPage->item.get());
    else
        uiPage->itemPath.clear();

    int currentItemIndex = -1;

    if(uiPage->itemPath.empty())
    {
        if(uiPage->item.valid())
        {
            std::string objectName;
            SGIPlugins::instance()->getObjectName(objectName, uiPage->item.get(), true);
            QtSGIItem data(uiPage->item.get());
            QString qobjectName = fromUtf8(objectName);
            ui->comboBoxPath->addItem(qobjectName, QVariant::fromValue(data));
            currentItemIndex = 0;
            int index = ui->tabWidget->indexOf(uiPage->tabPageWidget);
            ui->tabWidget->setTabText(index, qobjectName);
        }
    }
    else
    {
        for(SGIItemBasePtrPath::const_iterator it = uiPage->itemPath.begin(); it != uiPage->itemPath.end(); it++)
        {
            const SGIItemBasePtr & item = *it;
            std::string objectName;
            SGIPlugins::instance()->getObjectName(objectName, item.get(), true);
			QtSGIItem data(item.get());
            QString qobjectName = fromUtf8(objectName);
            ui->comboBoxPath->addItem(qobjectName, QVariant::fromValue(data));
            if (*item.get() == *uiPage->item.get())
            {
                currentItemIndex = ui->comboBoxPath->count() - 1;
                int index = ui->tabWidget->indexOf(uiPage->tabPageWidget);
                ui->tabWidget->setTabText(index, qobjectName);
            }
        }
    }
    int numItems = ui->comboBoxPath->count();
    ui->comboBoxPath->setEnabled(uiPage->itemPath.empty() == false);
    // trigger the drop-down box to adjust to the new content
    ui->comboBoxPath->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
    ui->comboBoxPath->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    ui->comboBoxPath->setCurrentIndex(currentItemIndex);
    ui->comboBoxPath->blockSignals(false);
    ui->actionItemPrevious->setEnabled(currentItemIndex > 0);
    ui->actionItemNext->setEnabled(currentItemIndex + 1 < numItems);
}

void SceneGraphDialog::reload()
{
    setCursor(Qt::WaitCursor);

    if(!uiPage->rootTreeItem.valid())
    {
        QTreeWidgetItem * root = uiPage->treeWidget->invisibleRootItem();
        QtSGIItem nodeDataRoot(SGIItemTypeTreeRoot, nullptr, true);
        // set dummy data into the second column (type)
        root->setData(0, Qt::UserRole, QVariant::fromValue(nodeDataRoot));
        uiPage->rootTreeItem = new ObjectTreeItem(root, SGIPlugins::instance()->hostInterface());
    }

	if (uiPage->item.valid())
	{
		std::string displayName;
        SGIPlugins::instance()->getObjectDisplayName(displayName, uiPage->item.get());
		setWindowTitle(tr("Information about %1").arg(fromUtf8(displayName)));
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
        SGIHostItemInternal hostItemToolsMenu(_toolsMenuInterface.get());
        SGIPlugins::instance()->generateItem(_itemToolsMenu, &hostItemToolsMenu);
        _toolsMenu->setObject(_itemToolsMenu.get());
    }

    uiPage->treeWidget->blockSignals(true);
    uiPage->treeWidget->clear();
    QList<int> panes_sizes;
    int total_width;
    QLayout * currentLayout = ui->verticalLayout;
    total_width = this->width() - ui->verticalLayout->margin();
    const int tree_width = 3 * total_width / 5;
    const int textbox_width = 2 * total_width / 5;
    panes_sizes.append(tree_width);
    panes_sizes.append(textbox_width);
    uiPage->splitter->setSizes(panes_sizes);

    total_width = tree_width - 32;
    uiPage->treeWidget->setColumnWidth(0, 3 * total_width / 4);
    uiPage->treeWidget->setColumnWidth(1, total_width / 4);

    ObjectTreeItem * viewNodeItem = nullptr;

    buildRootTree(static_cast<ObjectTreeItem *>(uiPage->rootTreeItem.get()));

    ObjectTreeItem * treeItem = nullptr;
    if(uiPage->item.valid())
    {
        treeItem = static_cast<ObjectTreeItem *>(uiPage->rootTreeItem->addChild(std::string(), uiPage->item.get()));
    }

	if(treeItem)
	{
		treeItem->setSelected(true);
		onItemActivated(treeItem->treeItem(), 0);
        uiPage->selectedTreeItem = treeItem;
	}
    else if(viewNodeItem)
    {
        viewNodeItem->setSelected(true);
        onItemActivated(viewNodeItem->treeItem(), 0);
        uiPage->selectedTreeItem = viewNodeItem;
    }
    else
        uiPage->selectedTreeItem = uiPage->rootTreeItem;
    uiPage->treeWidget->blockSignals(false);

    QTreeWidgetItem * currentItem = uiPage->treeWidget->currentItem();
    if (currentItem)
        uiPage->treeWidget->expandItem(currentItem);

    setCursor(Qt::ArrowCursor);
}

void SceneGraphDialog::itemNext()
{
    if(!uiPage->itemPath.empty())
    {
        for(SGIItemBasePtrPath::const_iterator it = uiPage->itemPath.begin(); it != uiPage->itemPath.end(); it++)
        {
            if(*uiPage->item.get() == *(*it))
            {
                it++;
                if(it != uiPage->itemPath.end())
                    uiPage->item = *it;
                selectItemInPathBox();
                reload();
                break;
            }
        }
    }
}

void SceneGraphDialog::itemPrevious()
{
    if(!uiPage->itemPath.empty())
    {
        SGIItemBasePtr prev;
        for(SGIItemBasePtrPath::const_iterator it = uiPage->itemPath.begin(); it != uiPage->itemPath.end(); it++)
        {
            if(*uiPage->item.get() == *(*it))
            {
                if(prev.valid())
                {
                    uiPage->item = prev;
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
    QTreeWidgetItem * item = uiPage->selectedTreeItem.valid()?((ObjectTreeItem*)uiPage->selectedTreeItem.get())->treeItem():nullptr;
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
    QTreeWidget * treeWidget = qobject_cast<QTreeWidget *>(sender());
    QTreeWidgetItem * item = treeWidget->currentItem();
    IObjectTreeItemPtr oldItem = uiPage->selectedTreeItem;
    if (item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        uiPage->selectedTreeItem = new ObjectTreeItem(item, SGIPlugins::instance()->hostInterface());
    }
    else
        uiPage->selectedTreeItem = nullptr;
    //d->_impl->itemSelected(oldItem.get(), uiPage->selectedTreeItem.get());
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
        ret = SGIPlugins::instance()->objectTreeBuildRootTree(treeItem, _itemSelf.get());
    return ret;
}

void SceneGraphDialog::setNodeInfo(const SGIItemBase * item)
{
    std::ostringstream os;
    if(item)
        SGIPlugins::instance()->writePrettyHTML(os, item);
    else
        os << "<b>item is <i>nullptr</i></b>";
    QScrollBar * vbar = uiPage->textEdit->verticalScrollBar();
    int vscrollPos = -1;
    if (vbar)
        vscrollPos = vbar->sliderPosition();
    uiPage->textEdit->blockSignals(true);
    uiPage->textEdit->setHtml(fromUtf8(os.str()));
    uiPage->textEdit->blockSignals(false);
    if (vbar && vscrollPos >= 0)
        vbar->setSliderPosition(vscrollPos);
}

void SceneGraphDialog::onItemContextMenu(QPoint pt)
{
    QTreeWidget * treeWidget = qobject_cast<QTreeWidget *>(sender());
	QTreeWidgetItem * item = treeWidget->itemAt (pt);
	QtSGIItem itemData;
    if (item)
    {
        itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();

        QMenu * contextQMenu = nullptr;
        IContextMenuPtr objectMenu = _hostCallback->contextMenu(this, itemData.item());
        if (!objectMenu)
        {
            if (_contextMenu.valid())
            {
                _contextMenu->setObject(itemData.item());
                objectMenu = _contextMenu;
            }
            else
            {
                objectMenu = SGIPlugins::instance()->createContextMenu(this, itemData.item(), _hostCallback.get());
            }
        }

        if (objectMenu.valid())
            contextQMenu = objectMenu->getMenu();

        _contextMenu = objectMenu;

        if (contextQMenu)
        {
            pt.ry() += treeWidget->header()->height();
            QPoint globalPos = treeWidget->mapToGlobal(pt);
            contextQMenu->popup(globalPos);
        }
    }
}

SGIItemBase * SceneGraphDialog::getView()
{
    if(_hostCallback.valid())
        return _hostCallback->getView();
    else
        return nullptr;
}

void SceneGraphDialog::triggerRepaint()
{
    if(_hostCallback.valid())
        _hostCallback->triggerRepaint();
}

bool SceneGraphDialog::newInstance(SGIItemBase * item)
{
    bool ret;
    // only open a new instance when the object is different
    if(uiPage->item.get() != item && *uiPage->item.get() != *item)
    {
        ISceneGraphDialog * dlg = SGIPlugins::instance()->showSceneGraphDialog(parentWidget(), item, _hostCallback.get());
        if(dlg)
            dlg->show();
        ret = (dlg != nullptr);
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
    SGIItemBasePtr item;
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
    SGIItemBasePtr item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        ret = showObjectLoggerDialog(item.get());
    else
        ret = false;
    return ret;
}

void SceneGraphDialog::selectItemInPathBox()
{
    int selectIndex = -1;
    ui->comboBoxPath->blockSignals(true);
    int numItems = ui->comboBoxPath->count();
    for(int i = 0; selectIndex < 0 && i < numItems; i++)
    {
		QtSGIItem data = ui->comboBoxPath->itemData(i).value<QtSGIItem>();
        if(*data.item() == *uiPage->item.get())
        {
            selectIndex = i;
            break;
        }
    }
    ui->actionItemPrevious->setEnabled(selectIndex > 0);
    ui->actionItemNext->setEnabled(selectIndex + 1 < numItems);
    ui->comboBoxPath->setCurrentIndex(selectIndex);
    ui->comboBoxPath->blockSignals(false);
}

void SceneGraphDialog::selectItemFromPath(int index)
{
	QtSGIItem data = ui->comboBoxPath->itemData(index).value<QtSGIItem>();
	uiPage->item = data.item();
    ui->actionItemPrevious->setEnabled(index > 0);
    ui->actionItemNext->setEnabled(index + 1 < ui->comboBoxPath->count());
    reload();
}

void SceneGraphDialog::refreshTimeChanged ( int n )
{
    if(!_refreshTimer)
    {
        _refreshTimer = new QTimer(this);
        connect(_refreshTimer, &QTimer::timeout, this, &SceneGraphDialog::refreshTimerExpired);
    }
    const int minimumTimerInterval = 100; // ms
    const int initialTimerInterval = 2500; // ms
    if (_refreshTimer->interval() == 0 && n >= minimumTimerInterval && n < initialTimerInterval)
    {
        ui->spinBoxRefreshTime->setValue(initialTimerInterval);
        n = initialTimerInterval;
    }
    if (n >= minimumTimerInterval)
        _refreshTimer->start(n);
    else
        _refreshTimer->stop();
}

void SceneGraphDialog::refreshTimerExpired()
{
    QTreeWidgetItem * item = uiPage->treeWidget->currentItem();
    if(item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        setNodeInfo(itemData.item());
    }
}

IObjectTreeItem * SceneGraphDialog::selectedItem()
{
	return uiPage ? uiPage->selectedTreeItem.get() : nullptr;
}

IObjectTreeItem * SceneGraphDialog::rootItem()
{
	return uiPage ? uiPage->rootTreeItem.get() : nullptr;
}

SGIItemBase * SceneGraphDialog::item() const
{
	return uiPage ? uiPage->item.get() : nullptr;
}

const SGIItemBasePtrPath & SceneGraphDialog::itemPath() const
{
	static SGIItemBasePtrPath s_empty;
	return uiPage ? uiPage->itemPath : s_empty;
}

void SceneGraphDialog::setInfoText(const std::string & text)
{
    uiPage->textEdit->blockSignals(true);
    uiPage->textEdit->setHtml(fromUtf8(text));
    uiPage->textEdit->blockSignals(false);
}

void SceneGraphDialog::tabChanged(int index)
{
    uiPage = ui->tabs[index];
}

void SceneGraphDialog::addNewTab()
{
    uiPage = ui->addTabPage(this);
    ui->actionCloseTab->setEnabled(ui->tabs.size() > 1);
}

void SceneGraphDialog::closeTab()
{
    int index = ui->tabWidget->currentIndex();
    Ui_TabPage * page = ui->tabs[index];
    if (uiPage == page)
        uiPage = nullptr;
    delete page;
    ui->tabs.erase(ui->tabs.begin() + index);
    ui->tabWidget->removeTab(index);
    ui->actionCloseTab->setEnabled(ui->tabs.size() > 1);
}

} // namespace sgi
