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
#include <ObjectTreeImplProxy.moc>

namespace sgi {

using namespace qt_helpers;

SGIPluginHostInterface * ObjectTreeItem::s_hostInterface = NULL;

ObjectTreeImplProxy::ObjectTreeImplProxy(QTreeWidget * widget, IObjectTreeImpl * impl, SGIPluginHostInterface * hostInterface)
    : QObject(widget)
    , _widget(widget)
    , _impl(impl)
    , _hostInterface(hostInterface)
    , _selectedTreeItem(NULL)
{
    _widget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(_widget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(onItemExpanded(QTreeWidgetItem*)));
    QObject::connect(_widget, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(onItemCollapsed(QTreeWidgetItem*)));
    QObject::connect(_widget, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(onItemActivated(QTreeWidgetItem*,int)));
    QObject::connect(_widget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onItemContextMenu(QPoint)));
    QObject::connect(_widget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
    QObject::connect(_widget, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
}

ObjectTreeImplProxy::~ObjectTreeImplProxy()
{
}

IObjectTreeItem * ObjectTreeImplProxy::selectedTreeItem()
{
    return _selectedTreeItem.get();
}

bool ObjectTreeImplProxy::buildTree(ObjectTreeItem * treeItem, SGIItemBase * item, bool addInternal)
{
    bool ret = _hostInterface->objectTreeBuildTree(treeItem, item);

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

    QTreeWidgetItem * treeItemQt = treeItem->treeItem();
    QtSGIItem itemData = treeItemQt->data(0, Qt::UserRole).value<QtSGIItem>();
    itemData.markAsPopulated();
    treeItemQt->setData(0, Qt::UserRole, QVariant::fromValue(itemData));

    // but return the result of the buildTree call to the user
    return ret;
}

void ObjectTreeImplProxy::onItemExpanded(QTreeWidgetItem * item)
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
        _impl->itemExpanded(&treeItem);
    }
    else
    {
        ObjectTreeItem treeItem(item);
        _impl->itemExpanded(&treeItem);
    }
}

void ObjectTreeImplProxy::onItemCollapsed(QTreeWidgetItem * item)
{
    ObjectTreeItem treeItem(item);
    _impl->itemCollapsed(&treeItem);
}

void ObjectTreeImplProxy::onItemClicked(QTreeWidgetItem * item, int column)
{
    ObjectTreeItem treeItem(item);
    _impl->itemClicked(&treeItem);
}

void ObjectTreeImplProxy::onItemActivated(QTreeWidgetItem * item, int column)
{
    ObjectTreeItem treeItem(item);
    _impl->itemActivated(&treeItem);
}

void ObjectTreeImplProxy::onItemContextMenu(QPoint pt)
{
    QTreeWidgetItem * item = _widget->itemAt (pt);
    QtSGIItem itemData;
    if (item)
    {
        itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();

        QMenu * contextMenu = NULL;
        ObjectTreeItem treeItem(item);

        IContextMenuPtr objectMenu;
        _impl->itemContextMenu(&treeItem, objectMenu);

        if (objectMenu.valid())
            contextMenu = objectMenu->getMenu();

        if (contextMenu)
        {
            pt.ry() += _widget->header()->height();
            QPoint globalPos = _widget->mapToGlobal(pt);
            contextMenu->popup(globalPos);
        }
    }
}

void ObjectTreeImplProxy::onItemSelectionChanged()
{
    QTreeWidgetItem * item = _widget->currentItem();
    IObjectTreeItemPtr oldItem = _selectedTreeItem;
    if(item)
    {
        QtSGIItem itemData = item->data(0, Qt::UserRole).value<QtSGIItem>();
        _selectedTreeItem = new ObjectTreeItem(item);
    }
    else
        _selectedTreeItem = NULL;
    _impl->itemSelected(oldItem.get(), _selectedTreeItem.get());
}

void ObjectTreeImplProxy::reloadSelectedItem()
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
            Q_FOREACH(QTreeWidgetItem * child, children)
            {
                delete child;
            }
        }
        ObjectTreeItem treeItem(item);
        buildTree(&treeItem, itemData.item());
    }
}

ObjectTreeItem::ObjectTreeItem (QTreeWidget * widget, IObjectTreeImpl * impl, SGIPluginHostInterface * hostInterface)
    : _hostInterface(hostInterface)
    , _item(widget->invisibleRootItem())
{
    new ObjectTreeImplProxy(widget, impl, hostInterface);
}

ObjectTreeItem::ObjectTreeItem (QTreeWidgetItem * item)
    : _hostInterface(s_hostInterface)
    , _item(item)
{
}

ObjectTreeItem::ObjectTreeItem(const ObjectTreeItem & item)
    : _hostInterface(s_hostInterface)
    , _item(item._item)
{
}

ObjectTreeItem::~ObjectTreeItem()
{
}


IObjectTreeItem * ObjectTreeItem::root()
{
    QTreeWidget * widget = _item->treeWidget();
    if(widget)
        return new ObjectTreeItem(widget->invisibleRootItem());
    else
        return NULL;
}

IObjectTreeItem * ObjectTreeItem::parent()
{
    QTreeWidgetItem * parent = _item->parent();
    if(parent)
        return new ObjectTreeItem(parent);
    else
        return NULL;
}

void ObjectTreeItem::clear()
{
    QList<QTreeWidgetItem*> childs = _item->takeChildren();
    foreach(QTreeWidgetItem* child, childs)
        delete child;
}

IObjectTreeItem * ObjectTreeItem::addChild(const std::string & name, SGIItemBase * item)
{
    return addChildImpl(name, item);
}

IObjectTreeItem * ObjectTreeItem::addChild(const std::string & name, const SGIHostItemBase * hostitem)
{
    osg::ref_ptr<SGIItemBase> item;
    if(s_hostInterface->generateItem(item, hostitem))
        return addChildImpl(name, item.get());
    else
        return NULL;
}
IObjectTreeItem * ObjectTreeItem::findChild(const std::string & name)
{
    QString qname = fromLocal8Bit(name);
    QTreeWidgetItem * retitem = NULL;
    for(int n = _item->childCount() - 1; !retitem && n >= 0; n--)
    {
        QTreeWidgetItem * child = _item->child(n);
        if(child->text(0) == qname)
            retitem = child;
    }
    if(retitem)
        return new ObjectTreeItem(retitem);
    else
        return NULL;
}

IObjectTreeItem * ObjectTreeItem::addChildIfNotExists(const std::string & name, SGIItemBase * item)
{
    IObjectTreeItem * ret = findChild(name);
    if(!ret)
        ret = addChild(name, item);
    return ret;
}

IObjectTreeItem * ObjectTreeItem::addChildIfNotExists(const std::string & name, const SGIHostItemBase * hostitem)
{
    IObjectTreeItem * ret = findChild(name);
    if(!ret)
        ret = addChild(name, hostitem);
    return ret;
}

void ObjectTreeItem::setSelected(bool select)
{
    _item->setSelected(select);
}
bool ObjectTreeItem::isSelected() const
{
    return _item->isSelected();
}

SGIItemBase * ObjectTreeItem::item()
{
    QtSGIItem itemData = _item->data(0, Qt::UserRole).value<QtSGIItem>();
    return itemData.item();
}
void ObjectTreeItem::expand()
{
    _item->setExpanded(true);
}

void ObjectTreeItem::collapse()
{
    _item->setExpanded(false);
}

QTreeWidgetItem * ObjectTreeItem::treeItem() { return _item; }
const QTreeWidgetItem * ObjectTreeItem::treeItem() const { return _item; }

IObjectTreeItem * ObjectTreeItem::addChildImpl(const std::string & name, SGIItemBase * item)
{
    QTreeWidgetItem * newItem = new QTreeWidgetItem;
    QtSGIItem itemData(item);
    QString itemText;
    QString itemTypeText;
    if(name.empty() && item)
    {
        std::string displayName;
        s_hostInterface->getObjectDisplayName(displayName, item);
        itemText = fromLocal8Bit(displayName);
    }
    else
        itemText = fromLocal8Bit(name);
    std::string typeName;
    if(item)
    {
        s_hostInterface->getObjectTypename(typeName, item);
        itemTypeText = fromLocal8Bit(typeName);
    }

    newItem->setText(0, itemText);
    newItem->setText(1, itemTypeText);
    newItem->setData(0, Qt::UserRole, QVariant::fromValue(itemData));
    addDummyChild(newItem);
    if(_item->childCount() == 1)
    {
        // if there's a dummy item in place at parent remove it first
        QTreeWidgetItem * firstChild = _item->child(0);
        if(firstChild)
        {
            QtSGIItem itemData = firstChild->data(0, Qt::UserRole).value<QtSGIItem>();
            if(itemData.type() == SGIItemTypeDummy)
                _item->removeChild(firstChild);
        }
    }
    // ... and finally add the new item to the tree
    _item->addChild(newItem);
    return new ObjectTreeItem(newItem);
}

void ObjectTreeItem::addDummyChild(QTreeWidgetItem * itemParent)
{
    QTreeWidgetItem * dummyChild = new QTreeWidgetItem;
    QtSGIItem dummyData(SGIItemTypeDummy);
    dummyChild->setText(0, "dummy");
    dummyChild->setData(0, Qt::UserRole, QVariant::fromValue(dummyData));
    itemParent->addChild(dummyChild);
}

void ObjectTreeItem::children(IObjectTreeItemPtrList & children)
{
    int numChilds = _item->childCount();
    children.resize(numChilds);
    for(int i = 0; i < numChilds; i++)
    {
        QTreeWidgetItem * child = _item->child(i);
        children[i] = new ObjectTreeItem(child);
    }
}

IObjectTreeItem * ObjectTreeItem::selectedItem()
{
    if(!_item)
        return NULL;
    QTreeWidgetItem * selitem = _item->treeWidget()->currentItem();
    return new ObjectTreeItem(selitem);
}

void ObjectTreeItem::reload()
{
    if(_item)
    {
        QtSGIItem itemData = _item->data(0, Qt::UserRole).value<QtSGIItem>();
        if(itemData.hasItem())
        {
            // we are going to re-populate the item with new data,
            // so first remove the old dummy child item.
            QList<QTreeWidgetItem *> children = _item->takeChildren();
            Q_FOREACH(QTreeWidgetItem * child, children)
            {
                delete child;
            }
        }
        itemData.markAsUnpopulated();
        _item->setData(0, Qt::UserRole, QVariant::fromValue(itemData));
        _item->setExpanded(false);
        addDummyChild(_item);
    }
}

} // namespace sgi
