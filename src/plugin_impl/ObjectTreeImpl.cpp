#include <QtCore/QString>
#include <QtGui/QTreeWidget>

#include <sgi/plugins/ObjectTreeImpl>

namespace sgi {

SGIPluginHostInterface * ObjectTreeItem::s_hostInterface = NULL;

ObjectTreeItem::ObjectTreeItem (QTreeWidgetItem * item)
    : _item(item)
{
}

ObjectTreeItem::ObjectTreeItem(const ObjectTreeItem & item)
    : _item(item._item)
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


} // namespace sgi
