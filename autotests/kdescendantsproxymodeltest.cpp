/*
    Copyright (C) 2016 Sune Vuorela <sune@debian.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
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

QTEST_MAIN(tst_KDescendantProxyModel)

#include "kdescendantsproxymodeltest.moc"

