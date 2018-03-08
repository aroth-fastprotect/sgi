#include "itemtree_unittest.h"
#include "itemtree_unittest.moc"

#include <QDebug>
#include <QDialog>
#include <QMenu>
#include <QThread>

#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/plugins/SGIHostItemQt.h>
#include <sgi/AutoLoadQt>
#include <sgi/Shutdown>
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
    shutdown<autoload::Qt>();
}

void itemtree_unittest::test_no_object()
{
    ISceneGraphDialogPtr dialog;
    dialog = showSceneGraphDialog<autoload::Qt>(NULL, (SGIHostItemQt*)NULL, NULL);
    QVERIFY(dialog.valid() == false);
}

enum ObjectType {
    ObjectTypeRoot,
    ObjectTypeChild,
};
template<int named_type>
class QObjectT : public QObject
{
public:
    QObjectT(const char * name, QObject * parent = NULL)
        : QObject(parent)
    {
        setObjectName(name);
    }
};

void itemtree_unittest::test_qobject()
{
    ISceneGraphDialogPtr dialog;

    QObject * obj = new QObjectT<ObjectTypeRoot>("test_qobject");
    QObject * child1 = new QObjectT<ObjectTypeChild>("child1", obj);
    QObject * child2 = new QObjectT<ObjectTypeChild>("child2", obj);

    SGIHostItemQt hostItem(obj);

    dialog = showSceneGraphDialog<autoload::Qt>(NULL, &hostItem, NULL);
    QVERIFY(dialog.valid() != false);

    dialog->show();
    QDialog * qdialog = dialog->getDialog();
    qdialog->exec();

    ISceneGraphDialog * raw = dialog.get();
    // ensure that the dialog pointer has exactly one reference
    QCOMPARE(getRefCount(raw), 1);

    // release dialog ptr (should be deallocated immediately)
    dialog = nullptr;
}

QTEST_MAIN(itemtree_unittest)
