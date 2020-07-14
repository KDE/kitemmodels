/*
    SPDX-FileCopyrightText: 2016 Sune Vuorela <sune@debian.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kdescendantsproxymodel.h"

#include <QStandardItemModel>
#include <QTest>
#include <QSignalSpy>

class tst_KDescendantProxyModel : public QObject
{
    Q_OBJECT
    QAbstractItemModel *createTree(const QString &prefix)
    {
        /*
         * |- parent1
         * |  |- child1
         * |  `- child2
         * `- parent2
         *    |- child1
         *    `- child2
         */
        QStandardItemModel *model = new QStandardItemModel();
        for (int i = 0; i < 2 ; i++) {
            QStandardItem *item = new QStandardItem();
            item->setData(QString(prefix + QString::number(i)), Qt::DisplayRole);
            for (int j = 0 ; j < 2 ; j++) {
                QStandardItem *child = new QStandardItem();
                child->setData(QString(prefix + QString::number(i) + "-" + QString::number(j)), Qt::DisplayRole);
                item->appendRow(child);
            }
            model->appendRow(item);
        }
        return model;
    }
private Q_SLOTS:
    void testResetModelContent();
    void testChangeSeparator();
    void testChangeInvisibleSeparator();
    void testRemoveSeparator();

    void testResetCollapsedModelContent();
    void testInsertInCollapsedModel();
    void testRemoveInCollapsedModel();
};

/// Tests that replacing the source model results in data getting changed
void tst_KDescendantProxyModel::testResetModelContent()
{
    auto model1 = createTree("FirstModel");
    KDescendantsProxyModel proxy;
    proxy.setSourceModel(model1);
    QCOMPARE(proxy.rowCount(), 6);

    {
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0-0"
                              << "FirstModel0-1"
                              << "FirstModel1"
                              << "FirstModel1-0"
                              << "FirstModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }
    auto model2 = createTree("SecondModel");
    {
        proxy.setSourceModel(model2);
        QStringList results = QStringList()
                              << "SecondModel0"
                              << "SecondModel0-0"
                              << "SecondModel0-1"
                              << "SecondModel1"
                              << "SecondModel1-0"
                              << "SecondModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }

    delete model2;
    delete model1;
}

/// tests that change separator works, as well as emits the relevant data changed signals
void tst_KDescendantProxyModel::testChangeSeparator()
{
    auto model1 = createTree("FirstModel");
    KDescendantsProxyModel proxy;
    proxy.setSourceModel(model1);
    proxy.setDisplayAncestorData(true);
    QSignalSpy dataChangedSpy(&proxy, &QAbstractItemModel::dataChanged);
    QCOMPARE(proxy.rowCount(), 6);
    {
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0 / FirstModel0-0"
                              << "FirstModel0 / FirstModel0-1"
                              << "FirstModel1"
                              << "FirstModel1 / FirstModel1-0"
                              << "FirstModel1 / FirstModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }
    proxy.setAncestorSeparator("LOL");
    QCOMPARE(dataChangedSpy.count(),1);
    {
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0LOLFirstModel0-0"
                              << "FirstModel0LOLFirstModel0-1"
                              << "FirstModel1"
                              << "FirstModel1LOLFirstModel1-0"
                              << "FirstModel1LOLFirstModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }

    delete model1;
}

/// tests that change separator that is not shown does not change the content and does not
/// emit data changed signals, since the data isn't changed
void tst_KDescendantProxyModel::testChangeInvisibleSeparator()
{
    auto model1 = createTree("FirstModel");
    KDescendantsProxyModel proxy;
    proxy.setSourceModel(model1);
    QSignalSpy dataChangedSpy(&proxy, &QAbstractItemModel::dataChanged);
    QCOMPARE(proxy.rowCount(), 6);
    {
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0-0"
                              << "FirstModel0-1"
                              << "FirstModel1"
                              << "FirstModel1-0"
                              << "FirstModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }
    proxy.setAncestorSeparator("LOL");
    QCOMPARE(dataChangedSpy.count(),0);
    {
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0-0"
                              << "FirstModel0-1"
                              << "FirstModel1"
                              << "FirstModel1-0"
                              << "FirstModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }

    delete model1;

}

/// tests that data is properly updated when separator is removed/hidden
/// and data changed signal is emitted
void tst_KDescendantProxyModel::testRemoveSeparator()
{
    auto model1 = createTree("FirstModel");
    KDescendantsProxyModel proxy;
    proxy.setSourceModel(model1);
    QSignalSpy dataChangedSpy(&proxy, &QAbstractItemModel::dataChanged);
    proxy.setDisplayAncestorData(true);
    QCOMPARE(dataChangedSpy.count(),1);
    dataChangedSpy.clear();
    QCOMPARE(proxy.rowCount(), 6);
    {
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0 / FirstModel0-0"
                              << "FirstModel0 / FirstModel0-1"
                              << "FirstModel1"
                              << "FirstModel1 / FirstModel1-0"
                              << "FirstModel1 / FirstModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }
    proxy.setDisplayAncestorData(false);
    QCOMPARE(dataChangedSpy.count(),1);
    {
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0-0"
                              << "FirstModel0-1"
                              << "FirstModel1"
                              << "FirstModel1-0"
                              << "FirstModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }

}

void tst_KDescendantProxyModel::testResetCollapsedModelContent()
{
    auto model1 = createTree("FirstModel");
    KDescendantsProxyModel proxy;
    proxy.setExpandsByDefault(false);
    proxy.setSourceModel(model1);
    QCOMPARE(proxy.rowCount(), 2);

    {
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }
    {
        QModelIndex idx = model1->index(0, 0);
        proxy.expandSourceIndex(idx);
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0-0"
                              << "FirstModel0-1"
                              << "FirstModel1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }
    {
        QModelIndex idx = model1->index(1, 0);
        proxy.expandSourceIndex(idx);
        QStringList results = QStringList()
                              << "FirstModel0"
                              << "FirstModel0-0"
                              << "FirstModel0-1"
                              << "FirstModel1"
                              << "FirstModel1-0"
                              << "FirstModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }

    auto model2 = createTree("SecondModel");
    {
        proxy.setSourceModel(model2);
        QModelIndex idx = model2->index(0, 0);
        proxy.expandSourceIndex(idx);
        idx = model2->index(1, 0);
        proxy.expandSourceIndex(idx);
        QStringList results = QStringList()
                              << "SecondModel0"
                              << "SecondModel0-0"
                              << "SecondModel0-1"
                              << "SecondModel1"
                              << "SecondModel1-0"
                              << "SecondModel1-1";
        QCOMPARE(proxy.rowCount(), results.count());
        for (int i  = 0 ; i < proxy.rowCount() ; i++) {
            QCOMPARE(proxy.index(i, 0).data(Qt::DisplayRole).toString(), results[i]);
        }
    }

    delete model2;
    delete model1;
}

void tst_KDescendantProxyModel::testInsertInCollapsedModel()
{
    QStandardItemModel *model1 = static_cast<QStandardItemModel *>(createTree("Model"));
    KDescendantsProxyModel proxy;
    proxy.setExpandsByDefault(false);
    proxy.setSourceModel(model1);
    QCOMPARE(proxy.rowCount(), 2);

    QSignalSpy insertSpy(&proxy, &QAbstractItemModel::rowsInserted);
    QCOMPARE(insertSpy.count(), 0);

    QStandardItem *parent = model1->item(0, 0);
    QVERIFY(parent);

    QStandardItem *child = new QStandardItem();
    child->setData(QString(QStringLiteral("Model") + QString::number(0) + "-" + QString::number(2)), Qt::DisplayRole);
    parent->appendRow(child);

    // Adding a child to the collapsed parent doesn't have an effect to the proxy 
    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(insertSpy.count(), 0);

    // If we expand everything inserted should be here
    QModelIndex idx = model1->index(0, 0);
    proxy.expandSourceIndex(idx);

    QCOMPARE(proxy.rowCount(), 5);
    QCOMPARE(insertSpy.count(), 1);

    QCOMPARE(proxy.index(3, 0).data(Qt::DisplayRole).toString(), QStringLiteral("Model0-2"));

    // Add another child to the expanded node, now the proxy is affected immediately
    child = new QStandardItem();
    child->setData(QString(QStringLiteral("Model") + QString::number(0) + "-" + QString::number(3)), Qt::DisplayRole);
    parent->appendRow(child);

    QCOMPARE(proxy.rowCount(), 6);
    QCOMPARE(insertSpy.count(), 2);
}

void tst_KDescendantProxyModel::testRemoveInCollapsedModel()
{
    QStandardItemModel *model1 = static_cast<QStandardItemModel *>(createTree("Model"));
    KDescendantsProxyModel proxy;
    proxy.setExpandsByDefault(false);
    proxy.setSourceModel(model1);
    QCOMPARE(proxy.rowCount(), 2);

    QSignalSpy removeSpy(&proxy, &QAbstractItemModel::rowsRemoved);
    QCOMPARE(removeSpy.count(), 0);

    QStandardItem *parent = model1->item(0, 0);
    QVERIFY(parent);

    parent->removeRow(0);

    // Adding a child to the collapsed parent doesn't have an effect to the proxy 
    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(removeSpy.count(), 0);

    // If we expand everything inserted should be here
    QModelIndex idx = model1->index(0, 0);
    proxy.expandSourceIndex(idx);

    QCOMPARE(proxy.rowCount(), 3);

    QCOMPARE(proxy.index(1, 0).data(Qt::DisplayRole).toString(), QStringLiteral("Model0-1"));
    parent->removeRow(0);

    QCOMPARE(proxy.rowCount(), 2);
    QCOMPARE(removeSpy.count(), 1);

    idx = model1->index(1, 0);
    proxy.expandSourceIndex(idx);
    QCOMPARE(proxy.rowCount(), 4);
}

QTEST_MAIN(tst_KDescendantProxyModel)

#include "kdescendantsproxymodeltest.moc"

