#include "item_unittest.h"
#include "item_unittest.moc"
#define SGI_NO_HOSTITEM_GENERATOR
#include <sgi/GenerateItem>

using namespace std;
using namespace sgi;

class TestItem : public SGIItemBase
{
public:
    static int ItemCount;
    TestItem()
    {
        ++ItemCount;
    }

    virtual ~TestItem()
    {
        --ItemCount;
    }
};

int TestItem::ItemCount = 0;

void item_unittest::verifyRefPtr()
{
    SGIItemBasePtr item, item2, item3;
    QCOMPARE(TestItem::ItemCount, 0);
    item = new TestItem;
    QCOMPARE(TestItem::ItemCount, 1);
    item = NULL;
    QCOMPARE(TestItem::ItemCount, 0);

    item = new TestItem;
    item2 = new TestItem;
    item3 = new TestItem;
    QCOMPARE(TestItem::ItemCount, 3);
    item->insertAfter(item2);
    item2->insertAfter(item3);
    item2 = NULL;
    item3 = NULL;
    QCOMPARE((int)item->listSize(), 2);
    QCOMPARE(TestItem::ItemCount, 3);
    item = NULL;
    QCOMPARE(TestItem::ItemCount, 0);

}

void item_unittest::insertItemSingle()
{
    {
        SGIItemBasePtr first = new SGIItemBase(SGIItemTypeInvalid, 0, 100);
        QCOMPARE(first->listSize(), (size_t)0);
        SGIItemBasePtr second = new SGIItemBase(SGIItemTypeInvalid, 0, 50);
        QCOMPARE(second->listSize(), (size_t)0);

        SGIItemBasePtr front;
        first->insertByScore(second, front);

        QVERIFY(first->score() > first->nextBase()->score());
        QCOMPARE(first->listSize(), (size_t)1);
    }
    {
        SGIItemBasePtr first = new SGIItemBase(SGIItemTypeInvalid, 0, 100);
        SGIItemBasePtr second = new SGIItemBase(SGIItemTypeInvalid, 0, 50);

        SGIItemBasePtr front;
        first->insertByScore(second, front);

        QVERIFY(first->score() > first->nextBase()->score());
    }
}

void item_unittest::insertItemTwoChains()
{
    SGIItemBasePtr firstChain;
    SGIItemBasePtr secondChain;
    const unsigned numItems = 50;
    {
        SGIItemBasePtr prev = NULL;
        for(unsigned n = 0; n <= (numItems * 2); n+=2)
        {
            SGIItemBasePtr item = new SGIItemBase(SGIItemTypeInvalid, 0, n);
            if(prev)
                prev->insertBefore(item);
            prev = item;
        }
        firstChain = prev;
        QVERIFY(firstChain->isListValid());
        QCOMPARE(firstChain->listSize(), (size_t)numItems);
    }

    {
        SGIItemBasePtr prev = NULL;
        for(unsigned n = 1; n <= (numItems * 2) + 1; n+=2)
        {
            SGIItemBasePtr item = new SGIItemBase(SGIItemTypeInvalid, 0, n);
            if(prev)
                prev->insertBefore(item);
            prev = item;
        }
        secondChain = prev;
        QVERIFY(secondChain->isListValid());
        QCOMPARE(secondChain->listSize(), (size_t)numItems);
    }
    SGIItemBasePtr newFront;
    firstChain->insertByScore(secondChain, newFront);
    QCOMPARE(newFront->score(), (numItems * 2) + 1);
    QVERIFY(newFront->isListValid());
    QCOMPARE(newFront->listSize(), (size_t)(2*numItems) + 1);
}

void item_unittest::insertItemSameScore()
{
    SGIItemBasePtr firstChain;
    SGIItemBasePtr secondChain;
    const unsigned numItems = 5;
    {
        SGIItemBasePtr prev = NULL;
        for(unsigned n = 0; n <= (numItems * 2); n+=2)
        {
            SGIItemBasePtr item = new SGIItemBase(SGIItemTypeInvalid, n, 17);
            if(prev)
                prev->insertBefore(item);
            prev = item;
        }
        firstChain = prev;
        QVERIFY(firstChain->isListValid());
        QCOMPARE(firstChain->listSize(), (size_t)numItems);
    }

    {
        SGIItemBasePtr prev = NULL;
        for(unsigned n = 1; n <= (numItems * 2) + 1; n+=2)
        {
            SGIItemBasePtr item = new SGIItemBase(SGIItemTypeInvalid, n, 17);
            if(prev)
                prev->insertBefore(item);
            prev = item;
        }
        secondChain = prev;
        QVERIFY(secondChain->isListValid());
        QCOMPARE(secondChain->listSize(), (size_t)numItems);
    }
    SGIItemBasePtr newFront;
    firstChain->insertByScore(secondChain, newFront);
    QCOMPARE(newFront->score(), 17u);
    QVERIFY(newFront->isListValid());
    QCOMPARE(newFront->listSize(), (size_t)(2*numItems) + 1);
}

QTEST_MAIN(item_unittest)
