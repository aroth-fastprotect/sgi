
void SceneGraphDialog::objectDumpToText()
{
    QAction *action = qobject_cast<QAction *>(sender());
    SGIItem itemData;
    if(action)
    {
        itemData = qVariantValue<SGIItem>(action->data());
        const osg::Object * object = dynamic_cast<const osg::Object *>(itemData.object());
        const osg::Node * node = dynamic_cast<const osg::Node *>(itemData.object());

        QString displayName = getObjectDisplayName(itemData.object());
        QString suggestedFilename = getObjectSuggestedFilename(object);
        QStringList filters;
        filters << "Text Files (*.txt *.log)" << "All Files (*.*)";

        QFileDialog dlg(this);
        dlg.setFileMode(QFileDialog::AnyFile);
        dlg.setAcceptMode(QFileDialog::AcceptSave);
        dlg.setNameFilters(filters);
        dlg.setDefaultSuffix("txt");
        dlg.setWindowTitle( tr("Save %1").arg(displayName));
        //dlg.setDirectory(fileinfo.absoluteDir());
        dlg.selectFile(suggestedFilename);
        if(dlg.exec())
        {
            bool success;
            QString fileName = dlg.selectedFiles().front();
            std::ofstream outfile(fileName.toStdString().c_str());
            DumpNodeVisitor dnv(outfile);
            if(node)
            {
                const_cast<osg::Node*>(node)->accept(dnv);
                success = true;
            }
            else
                success = false;
            if(!success)
            {
                QMessageBox::warning(this, windowTitle(),
                    tr("Failed to save %1 to %2").arg(displayName).arg(fileName));
            }
        }
    }
}

void SceneGraphDialog::objectDumpToXML()
{
    QAction *action = qobject_cast<QAction *>(sender());
    SGIItem itemData;
    if(action)
    {
        itemData = qVariantValue<SGIItem>(action->data());
        const osg::Object * object = dynamic_cast<const osg::Object *>(itemData.object());
        const osg::Node * node = dynamic_cast<const osg::Node *>(itemData.object());

        QString displayName = getObjectDisplayName(itemData.object());
        QString suggestedFilename = getObjectSuggestedFilename(object);
        QStringList filters;
        filters << "XML Files (*.xml)" << "All Files (*.*)";

        QFileDialog dlg(this);
        dlg.setFileMode(QFileDialog::AnyFile);
        dlg.setAcceptMode(QFileDialog::AcceptSave);
        dlg.setNameFilters(filters);
        dlg.setDefaultSuffix("xml");
        dlg.setWindowTitle( tr("Save %1").arg(displayName));
        //dlg.setDirectory(fileinfo.absoluteDir());
        dlg.selectFile(suggestedFilename);
        if(dlg.exec())
        {
            bool success;
            QString fileName = dlg.selectedFiles().front();
            std::ofstream outfile(fileName.toStdString().c_str());
            XMLDumpNodeVisitor dnv(outfile);
            if(node)
            {
                const_cast<osg::Node*>(node)->accept(dnv);
                success = true;
            }
            else
                success = false;
            if(!success)
            {
                QMessageBox::warning(this, windowTitle(),
                    tr("Failed to save %1 to %2").arg(displayName).arg(fileName));
            }
        }
    }
}


