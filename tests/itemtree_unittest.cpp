#include "itemtree_unittest.h"
#include "itemtree_unittest.moc"

#include <QDebug>
#include <QDialog>
#include <QMenu>
#include <QThread>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/AutoLoadQt>
#include <sgi/SceneGraphDialog>

using namespace std;
using namespace sgi;

inline QDebug operator<< (QDebug dbg, const std::string & s)
{
    return dbg << QString::fromStdString(s);
}


void itemtree_unittest::initTestCase()
{
}

void itemtree_unittest::cleanupTestCase()
{
}

void itemtree_unittest::test_qobject()
{
    QObject * obj = new QObject;
    obj->setObjectName("test_qobject");

    QObject * child1 = new QObject(obj);
    child1->setObjectName("child1");
    QObject * child2 = new QObject(obj);
    child2->setObjectName("child2");

    SGIHostItemQt hostItem(obj);

    ISceneGraphDialogPtr dialog = showSceneGraphDialog<autoload::Qt>(NULL, &hostItem, NULL);
    dialog->show();
    QDialog * qdialog = dialog->getDialog();
    qdialog->exec();
}

QTEST_MAIN(itemtree_unittest)
