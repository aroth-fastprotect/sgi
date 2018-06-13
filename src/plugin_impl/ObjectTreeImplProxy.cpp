// kate: syntax C++11;
// SGI - Copyright (C) 2012-2015 FAST Protect, Andreas Roth

#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>

#include <sgi/plugins/ObjectTreeImpl>
#include <sgi/plugins/ContextMenu>
#include <sgi/helpers/qt>
#include <sgi/plugins/SGIHostItemInternal.h>
#include <sgi/SGIItemInternal>

#include "ObjectTreeImplProxy.h"
#include "ObjectTreeImplProxy_p.h"

namespace sgi {

using namespace qt_helpers;

ObjectTreeImplProxyPrivate::ObjectTreeImplProxyPrivate(QTreeWidget * widget, IObjectTreeImpl * impl, SGIPluginHostInterface * hostInterface)
    : _widget(widget)
    , _impl(impl)
    , _hostInterface(hostInterface)
    , _selectedTreeItem(nullptr)
{
}

ObjectTreeImplProxyPrivate::~ObjectTreeImplProxyPrivate()
{
}

ObjectTreeImplProxy::ObjectTreeImplProxy(QTreeWidget * widget, IObjectTreeImpl * impl, SGIPluginHostInterface * hostInterface)
    : QObject(widget)
    , d_ptr(new ObjectTreeImplProxyPrivate(widget, impl, hostInterface))
{
    Q_D(ObjectTreeImplProxy);
    d->_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(d->_widget, &QTreeWidget::itemExpanded, this, &ObjectTreeImplProxy::onItemExpanded);
    QObject::connect(d->_widget, &QTreeWidget::itemCollapsed, this, &ObjectTreeImplProxy::onItemCollapsed);
    QObject::connect(d->_widget, &QTreeWidget::itemActivated, this, &ObjectTreeImplProxy::onItemActivated);
    QObject::connect(d->_widget, &QTreeWidget::customContextMenuRequested, this, &ObjectTreeImplProxy::onItemContextMenu);
    QObject::connect(d->_widget, &QTreeWidget::itemClicked, this, &ObjectTreeImplProxy::onItemClicked);
    QObject::connect(d->_widget, &QTreeWidget::itemSelectionChanged, this, &ObjectTreeImplProxy::onItemSelectionChanged);
}

ObjectTreeImplProxy::~ObjectTreeImplProxy()
{
    delete d_ptr;
}

IObjectTreeItem * ObjectTreeImplProxy::selectedTreeItem()
{
    Q_D(ObjectTreeImplProxy);
    return d->_selectedTreeItem.get();
}

bool ObjectTreeImplProxy::buildTree(IObjectTreeItem * treeItem, SGIItemBase * item, bool addInternal)
{
    Q_D(ObjectTreeImplProxy);
    bool ret = d->_hostInterface->objectTreeBuildTree(treeItem, item);

    if(addInternal)
    {
        // in any case mark the item as populated and add the internal item as well.
        // We ignore the result from any plugin here, because some plugin be not be
        // functioning as expected or no plugin might be available for this item. The
        // internal item is useful especially in these cases to investigate why the
        // call to build tree failed.
        InternalItemData internalItemData(item);;
        SGIHostItemInternal hostItemInternal(new ReferencedInternalItemData(internalItemData));
        treeItem->addChild("Internal", &hostItemInternal);
    }

    QTreeWidgetItem * treeItemQt = static_cast<ObjectTreeItem*>(treeItem)->treeItem();
    QtSGIItem itemData = treeItemQt->data(0, Qt::UserRole).value<QtSGIItem>();
    itemData.markAsPopulated();
    treeItemQt->setData(0, Qt::UserRole, QVariant::fromValue(itemData));

    // but return the result of the buildTree call to the user
    return ret;
}

void ObjectTreeImplProxy::onItemExpanded(QTreeWidgetItem * item)
{
    Q_D(ObjectTreeImplProxy);
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
        ObjectTreeItem treeItem(item, d->_hostInterface);
        buildTree(&treeItem, itemData.item());
        d->_impl->itemExpanded(&treeItem);
    }
    else
    {
        ObjectTreeItem treeItem(item, d->_hostInterface);
        d->_impl->itemExpanded(&treeItem);
    }
}

void ObjectTreeImplProxy::onItemCollapsed(QTreeWidgetItem * item)
{
    Q_D(ObjectTreeImplProxy);
    ObjectTreeItem treeItem(item, d->_hostInterface);
    d->_impl->itemCollapsed(&treeItem);
}

void ObjectTreeImplProxy::onItemClicked(QTreeWidgetItem * item, int column)
{
    Q_D(ObjectTreeImplProxy);
    ObjectTreeItem treeItem(item, d->_hostInterface);
    d->_impl->itemClicked(&treeItem);
}

void ObjectTreeImplProxy::onItemActivated(QTreeWidgetItem * item, int column)
{
    Q_D(ObjectTreeImplProxy);
    ObjectTreeItem treeItem(item, d->_hostInterface);
    d->_impl->itemActivated(&treeItem);
}

void ObjectTreeImplProxy::onItemContextMenu(const QPoint & pt)
{
    Q_D(ObjectTreeImplProxy);
    QTreeWidgetItem * item = d->_widget->itemAt (pt);
    QtSGIItem itemData;
    if (item)
    {
        itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();

        QMenu * contextMenu = nullptr;
        ObjectTreeItem treeItem(item, d->_hostInterface);

        IContextMenuPtr objectMenu;
        d->_impl->itemContextMenu(&treeItem, objectMenu);

        if (objectMenu.valid())
            contextMenu = objectMenu->getMenu();

        if (contextMenu)
        {
            QPoint tmp = pt;
            tmp.ry() += d->_widget->header()->height();
            QPoint globalPos = d->_widget->mapToGlobal(tmp);
            contextMenu->popup(globalPos);
        }
    }
}

void ObjectTreeImplProxy::onItemSelectionChanged()
{
    Q_D(ObjectTreeImplProxy);
    QTreeWidgetItem * item = d->_widget->currentItem();
    IObjectTreeItemPtr oldItem = d->_selectedTreeItem;
    if(item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        d->_selectedTreeItem = new ObjectTreeItem(item, d_ptr->_hostInterface);
    }
    else
        d->_selectedTreeItem = nullptr;
    d->_impl->itemSelected(oldItem.get(), d->_selectedTreeItem.get());
}

void ObjectTreeImplProxy::reloadSelectedItem()
{
    Q_D(ObjectTreeImplProxy);
    QTreeWidgetItem * item = d->_selectedTreeItem.valid()?((ObjectTreeItem*)d->_selectedTreeItem.get())->treeItem():nullptr;
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
        ObjectTreeItem treeItem(item, d->_hostInterface);
        buildTree(&treeItem, itemData.item());
    }
}


} // namespace sgi
