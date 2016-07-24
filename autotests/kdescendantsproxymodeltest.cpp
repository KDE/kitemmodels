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

QTEST_MAIN(tst_KDescendantProxyModel)

#include "kdescendantsproxymodeltest.moc"

