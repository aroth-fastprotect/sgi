
void SceneGraphDialog::toolsGPSToXYZConverter()
{
    static QString text;
    bool ok = false;
    QString input = QInputDialog::getText(this, tr("GPS to XYZ converter"), tr("GPS Coordinate (lat,lon,elev)"), QLineEdit::Normal, text, &ok);
    if(ok)
    {
        GpsCoordinate gps = GpsCoordinate::fromString(input, &ok);
        if(ok)
        {
            osg::Vec3d v = fromGpsCoordinateToXYZ(gps);
            QString result = QString::number(v.x(), 'g', DBL_DIG) + ',' + QString::number(v.y(), 'g', DBL_DIG) + ',' + QString::number(v.z(), 'g', DBL_DIG);
            QInputDialog::getText(this, tr("GPS to XYZ converter"), tr("Result"), QLineEdit::Normal, result, &ok);
        }
        text = input;
    }
}

void SceneGraphDialog::toolsXYZToGPSConverter()
{
    static QString text;
    bool ok = false;
    QString input = QInputDialog::getText(this, tr("XYZ to GPS converter"), tr("XYZ Coordinate (x,y,z)"), QLineEdit::Normal, text, &ok);
    if(ok)
    {
        QStringList elems = input.split(',');
        if(elems.size() == 3)
        {
            osg::Vec3d v(elems[0].toDouble(), elems[1].toDouble(), elems[2].toDouble());
            GpsCoordinate gps = fromXYZToGpsCoordinate(v);
            QString result = gps.toString();
            QInputDialog::getText(this, tr("GPS to XYZ converter"), tr("Result"), QLineEdit::Normal, result, &ok);
        }
        text = input;
    }
}

void SceneGraphDialog::toolsSavePositionMatrix()
{
    static QString text;
    bool ok = false;
    QString input = QInputDialog::getText(this, tr("Enter GPS position for matrix"), tr("GPS Coordinate (lat,lon,elev)"), QLineEdit::Normal, text, &ok);
    if(ok)
    {
        GpsCoordinate gps = GpsCoordinate::fromString(input, &ok);
        if(ok)
        {
            osg::EllipsoidModel ellipsoid;
            double latitude = osg::DegreesToRadians(gps.latitude());
            double longitude = osg::DegreesToRadians(gps.longitude());
            double height = gps.elevation();
            osg::Matrixd localToWorld;
            ellipsoid.computeLocalToWorldTransformFromLatLongHeight(latitude, longitude, height, localToWorld);
            
            osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
            std::string name = gps.toString().toStdString();
            mt->setMatrix(localToWorld);
            mt->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
            mt->setName(name);
            
            osg::ref_ptr<osg::Node> dummyNode = new osg::Node;
            dummyNode->setName("dummy");

            osg::ref_ptr<osg::ProxyNode> proxy = new osg::ProxyNode;
            proxy->setName("proxy_to_load_other_nodes_from_file");
            proxy->addChild(dummyNode, "dummy.ive");
            
            mt->addChild(proxy);

            saveOsgObject(mt, true);
        }
        text = input;
    }
}

void SceneGraphDialog::toolsSaveShapeBox()
{
    static QString text;
    bool ok = false;
    QString input = QInputDialog::getText(this, tr("Enter size of the box"), tr("Size (x,y,z)"), QLineEdit::Normal, text, &ok);
    if(ok)
    {
        QStringList elems = input.split(',');
        if(elems.size() == 3)
        {
            osg::Vec3d v(elems[0].toDouble(), elems[1].toDouble(), elems[2].toDouble());
            
            osg::ref_ptr<osg::Geode> geode = new osg::Geode;
            geode->setName(input.toStdString());
            
            osg::ref_ptr<osg::Box> shape = new osg::Box();
            shape->set(osg::Vec3d(0,0,0), osg::Vec3d(v.x()/2.0, v.y()/2.0, v.z()/2.0));

            osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(shape);
            geode->addDrawable(drawable);

            saveOsgObject(geode, true);
        }
        text = input;
    }
}


void SceneGraphDialog::toolsFindVisibleNodes()
{
    std::stringstream os;

    os << "<b>Visible locations</b><br/>" << std::endl;
    const SceneGraphManager * smgr = findSceneGraphManager();
    if(smgr)
    {
        os << "<ol>" << std::endl;
        SceneGraphManager::DataObjectIdToProxyNodeMap locations = smgr->computeVisibleLocations();
        for(SceneGraphManager::DataObjectIdToProxyNodeMap::const_iterator it = locations.begin(); it != locations.end(); it++)
        {
            const osg::ref_ptr<const SceneGraphProxyNode> & proxynode = it->second;
            QString name;
            sgi_getObjectDisplayName(proxynode.get(), name, true);
            os << "<li>" << name << "</li>" << std::endl;
        }
        os << "</ol>" << std::endl;
    }
    else
    {
        os << "<i>Unable to find SceneGraphManager.</i><br/>" << std::endl;
    }

    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(QString::fromStdString(os.str()));
    ui->textEdit->blockSignals(false);
}

void SceneGraphDialog::toolsFindOsgUpdableNodes()
{
    std::stringstream os;
    
    QList<QTreeWidgetItem *> items = ui->treeWidget->selectedItems();
    if(items.empty())
    {
        os << "<b>Results</b><br/>" << std::endl;
        os << "<i>No items selected</i><br/>";
    }
    else
    {
        foreach( QTreeWidgetItem *item, items)
        {
            QVariant data;
            if(item)
                data = item->data(0, Qt::UserRole);
            SGIItem itemData = qVariantValue<SGIItem>(data);
            
            QString itemName = getFullObjectName(itemData);
            os << "<b>Results from " << itemName << "</b><br/>" << std::endl;
            
            const osg::Node * node = dynamic_cast<const osg::Node*>(itemData.ref());
            if(node)
            {
                FindOsgUpdatableNodesVisitor visitor;
                const_cast<osg::Node*>(node)->accept(visitor);
                
                const osg::NodePathList & results = visitor.results();
                if(results.empty())
                {
                    os << "<i>no nodes found</i><br/>" << std::endl;
                }
                else
                {
                    os << "<table border=\'1\' width=\'100%\'>" << std::endl;
                    unsigned num = 0;
                    for(osg::NodePathList::const_iterator it = results.begin(); it != results.end(); num++, it++)
                    {
                        const osg::NodePath & path = *it;
                        os << "<tr><td>" << num << "</td><td>";
                        writePrettyHTML(os, path);
                        os << "</td></tr>" << std::endl;
                    }
                    os << "</table>" << std::endl;
                }
            }
            else
            {
                os << "<i>selected item is not a osg::Node</i><br/>" << std::endl;
            }
        }
    }

    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(QString::fromStdString(os.str()));
    ui->textEdit->blockSignals(false);
}

void SceneGraphDialog::toolsFindOsgUpdableNodesStuck()
{
    std::stringstream os;
    
    QList<QTreeWidgetItem *> items = ui->treeWidget->selectedItems();
    if(items.empty())
    {
        os << "<b>Results</b><br/>" << std::endl;
        os << "<i>No items selected</i><br/>";
    }
    else
    {
        foreach( QTreeWidgetItem *item, items)
        {
            QVariant data;
            if(item)
                data = item->data(0, Qt::UserRole);
            SGIItem itemData = qVariantValue<SGIItem>(data);
            
            QString itemName = getFullObjectName(itemData);
            os << "<b>Results from " << itemName << "</b><br/>" << std::endl;
            
            const osg::Node * node = dynamic_cast<const osg::Node*>(itemData.ref());
            if(node)
            {
                FindOsgUpdatableNodesStuckVisitor visitor;
                const_cast<osg::Node*>(node)->accept(visitor);
                
                const osg::NodePathList & results = visitor.results();
                if(results.empty())
                {
                    os << "<i>no nodes found</i><br/>" << std::endl;
                }
                else
                {
                    os << "<table border=\'1\' width=\'100%\'>" << std::endl;
                    unsigned num = 0;
                    for(osg::NodePathList::const_iterator it = results.begin(); it != results.end(); num++, it++)
                    {
                        const osg::NodePath & path = *it;
                        os << "<tr><td>" << num << "</td><td>";
                        writePrettyHTML(os, path);
                        os << "</td></tr>" << std::endl;
                    }
                    os << "</table>" << std::endl;
                }
            }
            else
            {
                os << "<i>selected item is not a osg::Node</i><br/>" << std::endl;
            }
        }
    }

    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(QString::fromStdString(os.str()));
    ui->textEdit->blockSignals(false);
}

void SceneGraphDialog::toolsUpdatableNodeWatcher()
{
    std::stringstream os;
    
    if(m_updatableNodeWatcher.valid())
        m_updatableNodeWatcher->clear();
    else
    {
        m_updatableNodeWatcher = new osgUpdatableNodeWatcher;
        QTreeWidgetItem * rootItem = ui->treeWidget->invisibleRootItem();
        buildTree(0, rootItem, m_updatableNodeWatcher.get(), tr("Update Node watcher"));
    }

    QList<QTreeWidgetItem *> items = ui->treeWidget->selectedItems();
    if(items.empty())
    {
        os << "<b>Results</b><br/>" << std::endl;
        os << "<i>No items selected</i><br/>";
    }
    else
    {
        foreach( QTreeWidgetItem *item, items)
        {
            QVariant data;
            if(item)
                data = item->data(0, Qt::UserRole);
            SGIItem itemData = qVariantValue<SGIItem>(data);
            
            QString itemName = getFullObjectName(itemData);
            os << "<b>Results from " << itemName << "</b><br/>" << std::endl;
            
            const osg::Node * node = dynamic_cast<const osg::Node*>(itemData.ref());
            if(node)
            {
                FindOsgUpdatableNodesVisitor visitor;
                const_cast<osg::Node*>(node)->accept(visitor);
                
                const osg::NodePathList & results = visitor.results();
                if(results.empty())
                {
                    os << "<i>no nodes found</i><br/>" << std::endl;
                }
                else
                {
                    os << "<table border=\'1\' width=\'100%\'>" << std::endl;
                    unsigned num = 0;
                    for(osg::NodePathList::const_iterator it = results.begin(); it != results.end(); num++, it++)
                    {
                        const osg::NodePath & path = *it;
                        osgUpdatableNode * upnode = dynamic_cast<osgUpdatableNode *>(path.back());
                        if(upnode)
                        {
                            m_updatableNodeWatcher->watch(upnode);
                            os << "<tr><td>" << num << "</td><td>";
                            writePrettyHTML(os, path);
                            os << "</td></tr>" << std::endl;
                        }
                    }
                    os << "</table>" << std::endl;
                }
            }
            else
            {
                os << "<i>selected item is not a osg::Node</i><br/>" << std::endl;
            }
        }
    }
    
    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(QString::fromStdString(os.str()));
    ui->textEdit->blockSignals(false);
}

void SceneGraphDialog::toolsTraversalHook()
{
    std::stringstream os;
    
    if(m_traversalWatcher.valid())
        m_traversalWatcher->clear();
    else
    {
        m_traversalWatcher = new osgTraversalWatcher;
        QTreeWidgetItem * rootItem = ui->treeWidget->invisibleRootItem();
        buildTree(0, rootItem, m_traversalWatcher.get(), tr("Traversal watcher"));
    }

    QList<QTreeWidgetItem *> items = ui->treeWidget->selectedItems();
    if(items.empty())
    {
        os << "<b>Results</b><br/>" << std::endl;
        os << "<i>No items selected</i><br/>";
    }
    else
    {
        foreach( QTreeWidgetItem *item, items)
        {
            QVariant data;
            if(item)
                data = item->data(0, Qt::UserRole);
            SGIItem itemData = qVariantValue<SGIItem>(data);
            
            QString itemName = getFullObjectName(itemData);
            os << "<b>Results from " << itemName << "</b><br/>" << std::endl;
            
            const osg::Node * node = dynamic_cast<const osg::Node*>(itemData.ref());
            if(node)
            {
                m_traversalWatcher->watch(const_cast<osg::Node*>(node));
            }
            else
            {
                os << "<i>selected item is not a osg::Node</i><br/>" << std::endl;
            }
        }
    }
    
    ui->textEdit->blockSignals(true);
    ui->textEdit->setHtml(QString::fromStdString(os.str()));
    ui->textEdit->blockSignals(false);
}

