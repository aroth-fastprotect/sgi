#pragma once

#include <QObject>

class QTreeWidget;
class QTreeWidgetItem;

namespace sgi {

class SGIItemBase;
class IObjectTreeImpl;
class IObjectTreeItem;
class SGIPluginHostInterface;

class ObjectTreeImplProxyPrivate;

class ObjectTreeImplProxy : public QObject
{
	Q_OBJECT
    Q_DECLARE_PRIVATE(ObjectTreeImplProxy)
public:
    ObjectTreeImplProxy(QTreeWidget * widget, IObjectTreeImpl * impl=nullptr, SGIPluginHostInterface * hostInterface=nullptr);
	virtual ~ObjectTreeImplProxy();

public slots:
	void                    onItemExpanded(QTreeWidgetItem * item);
    void                    onItemCollapsed(QTreeWidgetItem * item);
    void                    onItemClicked(QTreeWidgetItem * item, int column);
    void                    onItemActivated(QTreeWidgetItem * item, int column);
    void                    onItemContextMenu(const QPoint & pt);
	void					onItemSelectionChanged();

public:
	IObjectTreeItem *		selectedTreeItem();
	void					reloadSelectedItem();

protected:
	bool					buildTree(IObjectTreeItem * treeItem, SGIItemBase * item, bool addInternal=true);

private:
    ObjectTreeImplProxyPrivate * d_ptr;
};

} // namespace sgi {
