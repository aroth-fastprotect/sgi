#include "stdafx.h"
#include "ObjectLoggerDialog.h"

#include "ui_ObjectLoggerDialog.h"

#include <QToolBar>
#include <QComboBox>
#include <QSpinBox>
#include <QTimer>

#include "SGIPlugin.h"
#include <sgi/ContextMenu>
#include <sgi/SceneGraphDialog>
#include <sgi/plugins/SGIPluginInterface.h>
#include <sgi/plugins/SGIDataItemBase.h>

#include <sgi/plugins/ObjectTreeImpl>
#include <sgi/SGIItemInternal>
#include <sgi/helpers/qt>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace sgi {

using namespace qt_helpers;

class ObjectLoggerDialog::ContextMenuCallback : public IContextMenuInfo
{
public:
    ContextMenuCallback(ObjectLoggerDialog * dialog)
        : _dialog(dialog) {}
public:
    virtual SGIHostItemBase * getView()
    {
        return _dialog->getView();
    }
    virtual void            triggerRepaint()
    {
        _dialog->triggerRepaint();
    }
    virtual bool            showSceneGraphDialog(SGIItemBase * item)
    {
        return _dialog->showSceneGraphDialog(item);
    }
    virtual bool            showObjectLoggerDialog(SGIItemBase * item)
    {
        return _dialog->newInstance(item);
    }
private:
    ObjectLoggerDialog * _dialog;
};

class ObjectLoggerDialog::SceneGraphDialogInfo : public ISceneGraphDialogInfo
{
public:
    SceneGraphDialogInfo(ObjectLoggerDialog * dialog)
        : _dialog(dialog) {}
public:
    virtual IContextMenu *  contextMenu(QWidget * parent, const SGIItemBase * item, IContextMenuInfo * info)
    {
        return NULL;
    }
    virtual SGIHostItemBase * getView()
    {
        return _dialog->getView();
    }
    virtual void            triggerRepaint()
    {
        _dialog->triggerRepaint();
    }
private:
    ObjectLoggerDialog * _dialog;
};

class ObjectLoggerDialog::ObjectLoggerDialogImpl : public IObjectLoggerDialog
{
public:
    ObjectLoggerDialogImpl(ObjectLoggerDialog * dialog)
        : _dialog(dialog) {}

    virtual QDialog *       getDialog() { return _dialog; }
    virtual bool            addItem(SGIItemBase * item, bool alsoChilds=true) { return _dialog->addItem(item, alsoChilds); }
    virtual bool            addItems(const SGIItemBasePtrPath & path) { return _dialog->addItems(path); }
    virtual bool            removeItem(SGIItemBase * item) { return _dialog->removeItem(item); }
    virtual bool            removeItems(const SGIItemBasePtrPath & path) { return _dialog->removeItems(path); }

    virtual bool            addLogItem(SGIDataItemBase * item) { return _dialog->addLogItem(item); }
    virtual bool            removeLogItem(SGIDataItemBase * item, bool first) { return _dialog->removeLogItem(item, first); }

    virtual void            show() { emit _dialog->triggerShow(); }
    virtual void            hide() { emit _dialog->triggerHide(); }
    virtual bool            isVisible() { return _dialog->isVisible(); }
    virtual int             showModal() { return _dialog->exec(); }

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

ObjectLoggerDialog::ObjectLoggerDialog(SGIItemBase * item, IObjectLoggerDialogInfo * info, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(NULL)
    , _hostInterface(SGIPlugins::instance()->hostInterface())
    , _interface(new ObjectLoggerDialogImpl(this))
    , _logger(NULL)
    , _item(item)
    , _info(info)
    , _toolBar(NULL)
    , _actionReload(NULL)
    , _contextMenu(NULL)
    , _contextMenuCallback(NULL)
    , _spinBoxRefreshTime(NULL)
    , _refreshTimer(NULL)
    , _queuedOperations(new OperationQueue)
{
    init();
}

ObjectLoggerDialog::ObjectLoggerDialog(IObjectLogger * logger, IObjectLoggerDialogInfo * info, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
    , ui(NULL)
    , _hostInterface(SGIPlugins::instance()->hostInterface())
    , _interface(new ObjectLoggerDialogImpl(this))
    , _logger(logger)
    , _item(NULL)
    , _info(info)
    , _toolBar(NULL)
    , _actionReload(NULL)
    , _contextMenu(NULL)
    , _contextMenuCallback(NULL)
    , _spinBoxRefreshTime(NULL)
    , _refreshTimer(NULL)
    , _queuedOperations(new OperationQueue)
{
    init();
}

ObjectLoggerDialog::~ObjectLoggerDialog()
{
    if(_contextMenuCallback)
        delete _contextMenuCallback;
    if(_sceneGraphDialogInfo)
        delete _sceneGraphDialogInfo;
    if (ui)
    {
        delete ui;
        ui = NULL;
    }
    delete _queuedOperations;
    _interface = NULL;
}

void ObjectLoggerDialog::init()
{
    ui = new Ui_ObjectLoggerDialog;
    ui->setupUi( this );
    
    ObjectTreeItem::s_hostInterface = SGIPlugins::instance()->hostInterface();

    _toolBar = new QToolBar;
    QVBoxLayout * mainLayout = (QVBoxLayout *)this->layout();
    mainLayout->insertWidget(0, _toolBar);

    _actionReload = new QAction(tr("Reload"), this);
    Q_ASSERT(connect(_actionReload, SIGNAL(triggered()), this, SLOT(reload())));

    _spinBoxRefreshTime = new QSpinBox(_toolBar);
    _spinBoxRefreshTime->setMinimum(0);
    _spinBoxRefreshTime->setMaximum(600);
    _spinBoxRefreshTime->setPrefix("Refresh ");
    _spinBoxRefreshTime->setSuffix("s");
    Q_ASSERT(connect(_spinBoxRefreshTime, SIGNAL(valueChanged(int)), this, SLOT(refreshTimeChanged(int))));

    _toolBar->addAction(_actionReload);
    _toolBar->addWidget(_spinBoxRefreshTime);

    Q_ASSERT(QObject::connect(this, SIGNAL(triggerOnObjectChanged()), this, SLOT(onObjectChanged()), Qt::QueuedConnection));
    Q_ASSERT(QObject::connect(this, SIGNAL(triggerShow()), this, SLOT(show()), Qt::QueuedConnection));
    Q_ASSERT(QObject::connect(this, SIGNAL(triggerHide()), this, SLOT(hide()), Qt::QueuedConnection));

    Q_ASSERT(QObject::connect(this, SIGNAL(triggerUpdateLog()), this, SLOT(updateLog()), Qt::QueuedConnection));

    reload();
}

void ObjectLoggerDialog::onObjectChanged()
{
    reload();
}

void ObjectLoggerDialog::reload()
{
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

    ObjectTreeItem objectTreeRootItem(ui->treeWidget->invisibleRootItem());

    ObjectTreeItem * firstTreeItem = NULL;

    if(!_logger.valid() && _item.valid())
    {
        SGIPlugins::instance()->getOrCreateObjectLogger(_logger, _item);
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
            SGIDataFieldBase * field = fieldInfo.valid()?fieldInfo->getField(column):NULL;
            if(field)
                headerItems << fromLocal8Bit(field->toString(_hostInterface));
            else
                headerItems << QString("Field#%1").arg(column);
        }
        ui->tableWidget->setColumnCount(headerItems.size());
        ui->tableWidget->setVerticalHeaderLabels(headerItems);
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
                QtTableSGIItem data(dataItem, column);
                tableItem->setText(fromLocal8Bit(field->toString(_hostInterface)));
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
        Q_FOREACH(QTreeWidgetItem * child, children)
        {
            delete child;
        }
        ObjectTreeItem treeItem(item);
        buildTree(&treeItem, itemData.item());
    }
}

void ObjectLoggerDialog::onItemCollapsed(QTreeWidgetItem * item)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    //setNodeInfo(itemData.item());
}

void ObjectLoggerDialog::onItemClicked(QTreeWidgetItem * item, int column)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    //setNodeInfo(itemData.item());
}

void ObjectLoggerDialog::onItemActivated(QTreeWidgetItem * item, int column)
{
    QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
    //setNodeInfo(itemData.item());
}

bool ObjectLoggerDialog::buildTree(ObjectTreeItem * treeItem, SGIItemBase * item)
{
    bool ret = SGIPlugins::instance()->objectTreeBuildTree(treeItem, item);
    if(ret)
    {
        InternalItemData internalItemData(item);;
        SGIHostItemOsg hostItemInternal(new ReferencedInternalItemData(internalItemData));
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

    QMenu * contextMenu = NULL;
    if(!_contextMenuCallback)
        _contextMenuCallback = new ContextMenuCallback(this);

    IContextMenu * objectMenu = NULL;
    if(_info)
        objectMenu = _info->contextMenu(this, itemData.item(), _contextMenuCallback);
    if(!objectMenu)
    {
        if(_contextMenu)
        {
            _contextMenu->setObject(itemData.item(), _contextMenuCallback);
            objectMenu = _contextMenu;
        }
        else
        {
            objectMenu = SGIPlugins::instance()->createContextMenu(this, itemData.item(), _contextMenuCallback);
        }
    }

    if(objectMenu)
        contextMenu = objectMenu->getMenu();

    _contextMenu = objectMenu;

    if(contextMenu)
    {
        pt.ry() += ui->treeWidget->header()->height();
        QPoint globalPos = ui->treeWidget->mapToGlobal(pt);
        contextMenu->popup(globalPos);
    }
}

void ObjectLoggerDialog::refreshTimeChanged ( int n )
{
    if(!_refreshTimer)
    {
        _refreshTimer = new QTimer(this);
        Q_ASSERT(connect(_refreshTimer, SIGNAL(timeout()), this, SLOT(refreshTimerExpired())));
    }
    if(n > 0)
        _refreshTimer->start(n * 1000);
    else
        _refreshTimer->stop();
}

void ObjectLoggerDialog::refreshTimerExpired()
{
    QTreeWidgetItem * item = ui->treeWidget->currentItem();
    if(item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        //setNodeInfo(itemData.item());
    }
}

SGIHostItemBase * ObjectLoggerDialog::getView()
{
    if(_info)
        return _info->getView();
    else
        return NULL;
}

void ObjectLoggerDialog::triggerRepaint()
{
    if(_info)
        _info->triggerRepaint();
}

bool ObjectLoggerDialog::showSceneGraphDialog(SGIItemBase * item)
{
    return _info->showSceneGraphDialog(item);
}

bool ObjectLoggerDialog::showSceneGraphDialog(const SGIHostItemBase * hostitem)
{
    bool ret;
    osg::ref_ptr<SGIItemBase> item;
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
    osg::ref_ptr<SGIItemBase> item;
    if(SGIPlugins::instance()->generateItem(item, hostitem))
        ret = newInstance(item.get());
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::addItem(SGIItemBase * item, bool alsoChilds)
{
    bool ret;
    if(_logger)
        ret = _logger->addItem(item, alsoChilds);
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::addItems(const SGIItemBasePtrPath & path)
{
    bool ret;
    if(_logger)
        ret = _logger->addItems(path);
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::removeItem(SGIItemBase * item)
{
    bool ret;
    if(_logger)
        ret = _logger->removeItem(item);
    else
        ret = false;
    return ret;
}

bool ObjectLoggerDialog::removeItems(const SGIItemBasePtrPath & path)
{
    bool ret;
    if(_logger)
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
                unsigned row = ui->tableWidget->rowCount();
                ui->tableWidget->setRowCount(row + 1);
                size_t numFields = dataItem->numFields();
                for(unsigned column = 0; column < numFields; column++)
                {
                    SGIDataFieldBase * field = dataItem->getField(column);
                    QTableWidgetItem * tableItem = new QTableWidgetItem;
                    QtTableSGIItem data(dataItem, column);
                    tableItem->setText(fromLocal8Bit(field->toString(_hostInterface)));
                    tableItem->setData(Qt::UserRole, QVariant::fromValue(data));

                    ui->tableWidget->setItem(row, column, tableItem);
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

} // namespace sgi
