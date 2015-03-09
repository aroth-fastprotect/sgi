#pragma once

namespace sgi {

class ObjectTreeImplProxy : public QObject
{
	Q_OBJECT
public:
	ObjectTreeImplProxy(QTreeWidget * widget, IObjectTreeImpl * impl=NULL, SGIPluginHostInterface * hostInterface=NULL);
	virtual ~ObjectTreeImplProxy();

public slots:
	void                    onItemExpanded(QTreeWidgetItem * item);
    void                    onItemCollapsed(QTreeWidgetItem * item);
    void                    onItemClicked(QTreeWidgetItem * item, int column);
    void                    onItemActivated(QTreeWidgetItem * item, int column);
    void                    onItemContextMenu(QPoint pt);
	void					onItemSelectionChanged();

public:
	IObjectTreeItem *		selectedTreeItem();

protected:
	bool					buildTree(ObjectTreeItem * treeItem, SGIItemBase * item, bool addInternal=true);

protected:
	QTreeWidget *			 _widget;
	IObjectTreeImplPtr		 _impl;
	SGIPluginHostInterface * _hostInterface;
	IObjectTreeItemPtr		 _selectedTreeItem;
};

} // namespace sgi {
