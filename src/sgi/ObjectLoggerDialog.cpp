#include "stdafx.h"
#include "ObjectLoggerDialog.h"

#include <ui_ObjectLoggerDialog.h>

#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>
#include <QTimer>

#include "SGIPlugin.h"
#include <sgi/plugins/HostCallback>
#include <sgi/plugins/ContextMenu>
#include <sgi/plugins/SceneGraphDialog>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIDataItemBase.h>

#include <sgi/plugins/ObjectTreeImpl>
#include <sgi/SGIItemInternal>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/helpers/qt>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

using namespace qt_helpers;



class ObjectLoggerDialog::HostCallback : public HostCallbackFilterT<IObjectLoggerDialog>
{
public:
   HostCallback(IHostCallback * original, ObjectLoggerDialog * dialog)
        : HostCallbackFilterT<IObjectLoggerDialog>(original, dialog->_interface) {}
};

class ObjectLoggerDialog::ObjectLoggerDialogImpl : public IObjectLoggerDialog
{
public:
    ObjectLoggerDialogImpl(ObjectLoggerDialog * dialog)
        : _dialog(dialog) {}
    virtual                 ~ObjectLoggerDialogImpl() override { delete _dialog; }
    virtual QDialog *       getDialog() override { return _dialog; }
    virtual IHostCallback * getHostCallback() override { return _dialog->getHostCallback(); }
    virtual bool            addItem(SGIItemBase * item, bool alsoChilds=true) override { return _dialog->addItem(item, alsoChilds); }
    virtual bool            addItems(const SGIItemBasePtrPath & path) override { return _dialog->addItems(path); }
    virtual bool            removeItem(SGIItemBase * item) override { return _dialog->removeItem(item); }
    virtual bool            removeItems(const SGIItemBasePtrPath & path) override { return _dialog->removeItems(path); }

    virtual bool            addLogItem(SGIDataItemBase * item) override { return _dialog->addLogItem(item); }
    virtual bool            removeLogItem(SGIDataItemBase * item, bool first) override { return _dialog->removeLogItem(item, first); }

    virtual void            show() override { emit _dialog->triggerShow(); }
    virtual void            hide() override { emit _dialog->triggerHide(); }
    virtual bool            isVisible() override { return _dialog->isVisible(); }

private:
    ObjectLoggerDialog * _dialog;
};

class ObjectLoggerDialog::QueuedOperation
{
public:
    enum Type {
        TypeAddLogItem = 0,
        TypeRemoveLogItem,
        TypeRemoveFirstLogItem,
    };
    QueuedOperation(Type type, SGIDataItemBase * dataItem)
        : _type(type), _dataItem(dataItem) { }

    Type type() const { return _type; }
    SGIDataItemBase * dataItem() const { return _dataItem.get(); }
private:
    Type _type;
    SGIDataItemBasePtr _dataItem;
};

class ObjectLoggerDialog::OperationQueue : public std::queue<ObjectLoggerDialog::QueuedOperation>
{
public:
    OperationQueue()
        : std::queue<ObjectLoggerDialog::QueuedOperation>()
    {
    }

};

ObjectLoggerDialog::ObjectLoggerDialog(SGIItemBase * item, IHostCallback * callback, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(nullptr)
    , _hostInterface(SGIPlugins::instance()->hostInterface())
    , _interface(new ObjectLoggerDialogImpl(this))
    , _logger(nullptr)
    , _item(item)
    , _hostCallback(callback)
    , _toolBar(nullptr)
    , _actionReload(nullptr)
    , _contextMenu(nullptr)
    , _spinBoxRefreshTime(nullptr)
    , _refreshTimer(nullptr)
    , _queuedOperations(new OperationQueue)
{
    init();
}

ObjectLoggerDialog::ObjectLoggerDialog(IObjectLogger * logger, IHostCallback * callback, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(nullptr)
    , _hostInterface(SGIPlugins::instance()->hostInterface())
    , _interface(new ObjectLoggerDialogImpl(this))
    , _logger(logger)
    , _item(nullptr)
    , _hostCallback(callback)
    , _toolBar(nullptr)
    , _actionReload(nullptr)
    , _contextMenu(nullptr)
    , _spinBoxRefreshTime(nullptr)
    , _refreshTimer(nullptr)
    , _queuedOperations(new OperationQueue)
{
    init();
}

ObjectLoggerDialog::~ObjectLoggerDialog()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
    delete _queuedOperations;
    _interface = nullptr;
}

void ObjectLoggerDialog::init()
{
    ui = new Ui_ObjectLoggerDialog;
    ui->setupUi( this );

    Qt::WindowFlags flags =this->windowFlags()
        | Qt::WindowMinimizeButtonHint
        | Qt::WindowMaximizeButtonHint
        | Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    _toolBar = new QToolBar;
    QVBoxLayout * mainLayout = (QVBoxLayout *)this->layout();
    mainLayout->insertWidget(0, _toolBar);

    _actionReload = new QAction(tr("Reload"), this);
	_actionReload->setIcon(QIcon::fromTheme("view-refresh"));
    connect(_actionReload, &QAction::triggered, this, &ObjectLoggerDialog::reload);

    _spinBoxRefreshTime = new QSpinBox(_toolBar);
    _spinBoxRefreshTime->setToolTip(tr("Automatically reloads the information every X milliseconds."));
    _spinBoxRefreshTime->setMinimum(0);
    _spinBoxRefreshTime->setMaximum(60000);
    _spinBoxRefreshTime->setSingleStep(100);
    _spinBoxRefreshTime->setPrefix("Refresh ");
    _spinBoxRefreshTime->setSuffix("ms");
    connect(_spinBoxRefreshTime, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &ObjectLoggerDialog::refreshTimeChanged);

    _toolBar->addAction(_actionReload);
    _toolBar->addWidget(_spinBoxRefreshTime);

    connect(this, &ObjectLoggerDialog::triggerOnObjectChanged, this, &ObjectLoggerDialog::onObjectChanged, Qt::QueuedConnection);
    connect(this, &ObjectLoggerDialog::triggerShow, this, &ObjectLoggerDialog::show, Qt::QueuedConnection);
    connect(this, &ObjectLoggerDialog::triggerHide, this, &ObjectLoggerDialog::hide, Qt::QueuedConnection);

    connect(this, &ObjectLoggerDialog::triggerUpdateLog, this, &ObjectLoggerDialog::updateLog, Qt::QueuedConnection);

    reload();
}

void ObjectLoggerDialog::onObjectChanged()
{
    reload();
}

void ObjectLoggerDialog::reload()
{
	if (_item.valid())
	{
		std::string displayName;
        SGIPlugins::instance()->getObjectDisplayName(displayName, _item.get());
		setWindowTitle(tr("Log of %1").arg(fromUtf8(displayName)));
	}
	else
	{
		setWindowTitle(tr("No Log available"));
	}

    reloadTree();
    reloadLog();
}

void ObjectLoggerDialog::reloadTree()
{
    setCursor(Qt::WaitCursor);

    ui->treeWidget->blockSignals(true);
    ui->treeWidget->clear();
    QList<int> panes_sizes;
    int total_width ;
    total_width = this->width() - ui->verticalLayout->margin();
    const int tree_width = 3 * total_width / 5;
    const int textbox_width = 2 * total_width / 5;
    panes_sizes.append(tree_width);
    panes_sizes.append(textbox_width);
    ui->splitter->setSizes(panes_sizes);

    total_width = tree_width - 32;
    ui->treeWidget->setColumnWidth(0, 3 * total_width / 4);
    ui->treeWidget->setColumnWidth(1, total_width / 4);

    ObjectTreeItem objectTreeRootItem(ui->treeWidget->invisibleRootItem(), SGIPlugins::instance()->hostInterface());

    ObjectTreeItem * firstTreeItem = nullptr;

    if(!_logger.valid() && _item.valid())
    {
        SGIPlugins::instance()->getOrCreateObjectLogger(_logger, _item.get());
    }

    if(_logger.valid())
    {
        SGIItemBasePtrVector items;
        if(_logger->getItems(items))
        {
            for(SGIItemBasePtrVector::const_iterator it = items.begin(); it != items.end(); it++)
            {
                const SGIItemBasePtr & item = *it;
                ObjectTreeItem * treeItem = (ObjectTreeItem *)objectTreeRootItem.addChild(std::string(), item.get());
                buildTree(treeItem, item.get());
                if(!firstTreeItem)
                    firstTreeItem = treeItem;
            }
        }
    }

    if(firstTreeItem)
    {
        firstTreeItem->setSelected(true);
        onItemActivated(firstTreeItem->treeItem(), 0);
    }

    ui->treeWidget->blockSignals(false);
    setCursor(Qt::ArrowCursor);
}

void ObjectLoggerDialog::reloadLog()
{
    setCursor(Qt::WaitCursor);

    ui->tableWidget->blockSignals(true);
    ui->tableWidget->clear();

    if(_logger.valid())
    {
        SGIDataItemBasePtrList logEntries;
        _logger->getLog(logEntries);

        unsigned numFields = 0;
        SGIDataItemBasePtr fieldInfo;
        if(_logger->getFields(fieldInfo) && fieldInfo.valid())
            numFields = fieldInfo->numFields();
        else if(!logEntries.empty())
            numFields = logEntries.front()->numFields();

        QStringList headerItems;
        for(unsigned column = 0; column < numFields; column++)
        {
            SGIDataFieldBase * field = fieldInfo.valid()?fieldInfo->getField(column):nullptr;
            if(field)
                headerItems << fromUtf8(field->toString(_hostInterface));
            else
                headerItems << QString("Field#%1").arg(column);
        }
        ui->tableWidget->setColumnCount(headerItems.size());
        ui->tableWidget->setHorizontalHeaderLabels(headerItems);
        ui->tableWidget->resizeColumnToContents(numFields);

        // adjust the number of row to include all log entries
        ui->tableWidget->setRowCount(logEntries.size());

        unsigned row = 0;
        for(SGIDataItemBasePtrList::const_iterator it = logEntries.begin(); it != logEntries.end(); it++, row++)
        {
            const SGIDataItemBasePtr & dataItem = *it;
            numFields = dataItem->numFields();
            for(unsigned column = 0; column < numFields; column++)
            {
                SGIDataFieldBase * field = dataItem->getField(column);
                QTableWidgetItem * tableItem = new QTableWidgetItem;
                QtTableSGIItem data(dataItem.get(), column);
                tableItem->setText(fromUtf8(field->toString(_hostInterface)));
                tableItem->setData(Qt::UserRole, QVariant::fromValue(data));

                ui->tableWidget->setItem(row, column, tableItem);
            }
        }
    }

    ui->tableWidget->blockSignals(false);
    setCursor(Qt::ArrowCursor);
}

void ObjectLoggerDialog::onItemExpanded(QTreeWidgetItem * item)
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

void ObjectLoggerDialog::onItemCollapsed(QTreeWidgetItem * item)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    setNodeInfo(itemData.item());
}

void ObjectLoggerDialog::onItemClicked(QTreeWidgetItem * item, int column)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    setNodeInfo(itemData.item());
}

void ObjectLoggerDialog::onItemActivated(QTreeWidgetItem * item, int column)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    setNodeInfo(itemData.item());
}

bool ObjectLoggerDialog::buildTree(ObjectTreeItem * treeItem, SGIItemBase * item)
{
    bool ret = SGIPlugins::instance()->objectTreeBuildTree(treeItem, item);
    if(ret)
    {
        InternalItemData internalItemData(item);;
        SGIHostItemInternal hostItemInternal(new ReferencedInternalItemData(internalItemData));
        treeItem->addChild("Internal", &hostItemInternal);

        QTreeWidgetItem * treeItemQt = treeItem->treeItem();
        QtSGIItem itemData = treeItemQt->data(0, Qt::UserRole).value<QtSGIItem>();
        itemData.markAsPopulated();
        treeItemQt->setData(0, Qt::UserRole, QVariant::fromValue(itemData));
    }
    return ret;
}

void ObjectLoggerDialog::onItemContextMenu(QPoint pt)
{
    QTreeWidgetItem * item = ui->treeWidget->itemAt (pt);
    QtSGIItem itemData;
    if(item)
        itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();

    QMenu * contextQMenu = nullptr;
    IContextMenuPtr objectMenu = _hostCallback->contextMenu(this, itemData.item());
    if(!objectMenu)
    {
        if(_contextMenu.valid())
        {
            _contextMenu->setObject(itemData.item());
            objectMenu = _contextMenu;
        }
        else
        {
            objectMenu = SGIPlugins::instance()->createContextMenu(this, itemData.item(), _hostCallback.get());
        }
    }

    if(objectMenu.valid())
        contextQMenu = objectMenu->getMenu();

    _contextMenu = objectMenu;

    if(contextQMenu)
    {
        pt.ry() += ui->treeWidget->header()->height();
        QPoint globalPos = ui->treeWidget->mapToGlobal(pt);
        contextQMenu->popup(globalPos);
    }
}

void ObjectLoggerDialog::refreshTimeChanged ( int n )
{
    if(!_refreshTimer)
    {
        _refreshTimer = new QTimer(this);
        connect(_refreshTimer, &QTimer::timeout, this, &ObjectLoggerDialog::refreshTimerExpired);
    }
    if(n >= 100)
        _refreshTimer->start(n);
    else
        _refreshTimer->stop();
}

void ObjectLoggerDialog::refreshTimerExpired()
{
    QTreeWidgetItem * item = ui->treeWidget->currentItem();
    if(item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        setNodeInfo(itemData.item());
    }
}

SGIItemBase * ObjectLoggerDialog::getView()
{
    if(_hostCallback.valid())
        return _hostCallback->getView();
    else
        return nullptr;
}

void ObjectLoggerDialog::triggerRepaint()
{
    if(_hostCallback.valid())
        _hostCallback->triggerRepaint();
}

bool ObjectLoggerDialog::showSceneGraphDialog(SGIItemBase * item)
{
    return _hostCallback->showSceneGraphDialog(this, item);
}

bool ObjectLoggerDialog::showSceneGraphDialog(const SGIHostItemBase * hostitem)
{
    bool ret;
    SGIItemBasePtr item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        ret = showSceneGraphDialog(item.get());
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::newInstance(SGIItemBase * item)
{
    return false;
}

bool ObjectLoggerDialog::newInstance(const SGIHostItemBase * hostitem)
{
    bool ret;
    SGIItemBasePtr item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        ret = newInstance(item.get());
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::addItem(SGIItemBase * item, bool alsoChilds)
{
    bool ret;
    if(_logger.valid())
        ret = _logger->addItem(item, alsoChilds);
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::addItems(const SGIItemBasePtrPath & path)
{
    bool ret;
    if(_logger.valid())
        ret = _logger->addItems(path);
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::removeItem(SGIItemBase * item)
{
    bool ret;
    if(_logger.valid())
        ret = _logger->removeItem(item);
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::removeItems(const SGIItemBasePtrPath & path)
{
    bool ret;
    if(_logger.valid())
        ret = _logger->removeItems(path);
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::addLogItem(SGIDataItemBase * item)
{
    _queuedOperations->push(QueuedOperation(QueuedOperation::TypeAddLogItem, item));
    emit triggerUpdateLog();
    return true;
}

bool ObjectLoggerDialog::removeLogItem(SGIDataItemBase * item, bool first)
{
    if(first)
        _queuedOperations->push(QueuedOperation(QueuedOperation::TypeRemoveFirstLogItem, item));
    else
        _queuedOperations->push(QueuedOperation(QueuedOperation::TypeRemoveLogItem, item));
    emit triggerUpdateLog();
    return true;
}

void ObjectLoggerDialog::setNodeInfo(const SGIItemBase * item)
{
	std::ostringstream os;
	if (item)
		SGIPlugins::instance()->writePrettyHTML(os, item);
	else
	{
        os << "<b>item is <i>nullptr</i></b>";
	}
	ui->textEdit->blockSignals(true);
	ui->textEdit->setHtml(fromUtf8(os.str()));
	ui->textEdit->blockSignals(false);
}

void ObjectLoggerDialog::updateLog()
{
    while(!_queuedOperations->empty())
    {
        const QueuedOperation & op = _queuedOperations->front();
        switch(op.type())
        {
        case QueuedOperation::TypeAddLogItem:
            {
                SGIDataItemBase * dataItem = op.dataItem();
				if (dataItem)
				{
					unsigned row = ui->tableWidget->rowCount();
					ui->tableWidget->setRowCount(row + 1);
					size_t numFields = dataItem->numFields();
					for (unsigned column = 0; column < numFields; column++)
					{
						SGIDataFieldBase * field = dataItem->getField(column);
						QTableWidgetItem * tableItem = new QTableWidgetItem;
						QtTableSGIItem data(dataItem, column);
						tableItem->setText(fromUtf8(field->toString(_hostInterface)));
						tableItem->setData(Qt::UserRole, QVariant::fromValue(data));

						ui->tableWidget->setItem(row, column, tableItem);
					}
				}
            }
            break;
        case QueuedOperation::TypeRemoveFirstLogItem:
            ui->tableWidget->removeRow(0);
            break;
        case QueuedOperation::TypeRemoveLogItem:
            break;
        }
        _queuedOperations->pop();
    }
}

void ObjectLoggerDialog::closeEvent(QCloseEvent * event)
{
    if (_refreshTimer)
    {
        delete _refreshTimer;
        _refreshTimer = nullptr;
    }

    QDialog::closeEvent(event);
}

} // namespace sgi
