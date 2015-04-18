#pragma once

namespace sgi {

class ObjectTreeImplProxyPrivate
{
public:
    ObjectTreeImplProxyPrivate(QTreeWidget * widget, IObjectTreeImpl * impl, SGIPluginHostInterface * hostInterface);
    ~ObjectTreeImplProxyPrivate();

	QTreeWidget *			 _widget;
	IObjectTreeImplPtr		 _impl;
	SGIPluginHostInterface * _hostInterface;
	IObjectTreeItemPtr		 _selectedTreeItem;
};

} // namespace sgi {
