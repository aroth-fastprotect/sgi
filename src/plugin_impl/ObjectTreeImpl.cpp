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

namespace sgi {

using namespace qt_helpers;

ObjectTreeItem::ObjectTreeItem (QTreeWidget * widget, IObjectTreeImpl * impl, SGIPluginHostInterface * hostInterface)
    : IObjectTreeItem()
    , _hostInterface(hostInterface)
    , _item(widget->invisibleRootItem())
{
    new ObjectTreeImplProxy(widget, impl, hostInterface);
}

ObjectTreeItem::ObjectTreeItem (QTreeWidgetItem * item, SGIPluginHostInterface * hostInterface)
    : IObjectTreeItem()
    , _hostInterface(hostInterface)
    , _item(item)
{
}

ObjectTreeItem::ObjectTreeItem(const ObjectTreeItem & item)
    : IObjectTreeItem(item)
    , _hostInterface(item._hostInterface)
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
        return new ObjectTreeItem(widget->invisibleRootItem(), _hostInterface);
    else
        return NULL;
}

IObjectTreeItem * ObjectTreeItem::parent()
{
    QTreeWidgetItem * parent = _item->parent();
    if(parent)
        return new ObjectTreeItem(parent, _hostInterface);
    else
        return NULL;
}

void ObjectTreeItem::clear()
{
    QList<QTreeWidgetItem*> childs = _item->takeChildren();
    for(QTreeWidgetItem* child : childs)
        delete child;
}

IObjectTreeItem * ObjectTreeItem::addChild(const std::string & name, SGIItemBase * item)
{
    return addChildImpl(name, item);
}

IObjectTreeItem * ObjectTreeItem::addChild(const std::string & name, const SGIHostItemBase * hostitem)
{
    osg::ref_ptr<SGIItemBase> item;
    if(_hostInterface->generateItem(item, hostitem))
        return addChildImpl(name, item.get());
    else
        return NULL;
}
IObjectTreeItem * ObjectTreeItem::findChild(const std::string & name)
{
    QString qname = fromUtf8(name);
    QTreeWidgetItem * retitem = NULL;
    for(int n = _item->childCount() - 1; !retitem && n >= 0; n--)
    {
        QTreeWidgetItem * child = _item->child(n);
        if(child->text(0) == qname)
            retitem = child;
    }
    if(retitem)
        return new ObjectTreeItem(retitem, _hostInterface);
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
        _hostInterface->getObjectDisplayName(displayName, item);
        itemText = fromUtf8(displayName);
    }
    else
        itemText = fromUtf8(name);
    std::string typeName;
    if(item)
    {
        _hostInterface->getObjectTypename(typeName, item);
        itemTypeText = fromUtf8(typeName);
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
    return new ObjectTreeItem(newItem, _hostInterface);
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
        children[i] = new ObjectTreeItem(child, _hostInterface);
    }
}

IObjectTreeItem * ObjectTreeItem::selectedItem()
{
    if(!_item)
        return NULL;
    QTreeWidgetItem * selitem = _item->treeWidget()->currentItem();
    return new ObjectTreeItem(selitem, _hostInterface);
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
            for(QTreeWidgetItem * child : children)
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

void ObjectTreeItem::setName(const std::string & name)
{
    _item->setText(0, fromUtf8(name));
}

void ObjectTreeItem::updateName()
{
    std::string displayName;
    _hostInterface->getObjectDisplayName(displayName, item());
    _item->setText(0, fromUtf8(displayName));
}

} // namespace sgi
