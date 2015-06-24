/*
    Copyright (c) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Authors: David Faure <david.faure@kdab.com>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QTest>
#include <QDebug>
#include <QStandardItemModel>

#include <QTreeView>

#include <krearrangecolumnsproxymodel.h>
#include "test_model_helpers.h"
using namespace TestModelHelpers;

Q_DECLARE_METATYPE(QModelIndex)

class tst_KRearrangeColumnsProxyModel : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QModelIndex>();
    }

    void init()
    {
        // Prepare the source model to use later on
        mod.clear();
        mod.appendRow(makeStandardItems(QStringList() << "A" << "B" << "C" << "D" << "E"));
        mod.item(0, 0)->appendRow(makeStandardItems(QStringList() << "m" << "n" << "o" << "p" << "-"));
        mod.item(0, 0)->appendRow(makeStandardItems(QStringList() << "q" << "r" << "s" << "t" << "-"));
        mod.appendRow(makeStandardItems(QStringList() << "E" << "F" << "G" << "H" << "I"));
        mod.item(1, 0)->appendRow(makeStandardItems(QStringList() << "x" << "y" << "z" << "." << "-"));
        mod.setHorizontalHeaderLabels(QStringList() << "H1" << "H2" << "H3" << "H4" << "H5");

        QCOMPARE(extractRowTexts(&mod, 0), QString("ABCDE"));
        QCOMPARE(extractRowTexts(&mod, 0, mod.index(0, 0)), QString("mnop-"));
        QCOMPARE(extractRowTexts(&mod, 1, mod.index(0, 0)), QString("qrst-"));
        QCOMPARE(extractRowTexts(&mod, 1), QString("EFGHI"));
        QCOMPARE(extractRowTexts(&mod, 0, mod.index(1, 0)), QString("xyz.-"));
        QCOMPARE(extractHorizontalHeaderTexts(&mod), QString("H1H2H3H4H5"));

        // test code to see the model
        // showModel(&mod);
    }

    void shouldShowNothingIfNoColumnSelection()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;

        // When setting it to a source model
        pm.setSourceModel(&mod);

        // Then the proxy should show nothing (no columns selected)
        QCOMPARE(pm.rowCount(), mod.rowCount());
        QCOMPARE(pm.columnCount(), 0);
    }

    void shouldRearrangeColumns()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;

        // When setting it to a source model, with columns rearranged
        setup(pm);

        // Then the proxy should show columns reordered
        QCOMPARE(pm.rowCount(), 2);

        // (verify that the mapFromSource(mapToSource(x)) == x roundtrip works)
        for (int row = 0; row < pm.rowCount(); ++row) {
            for (int col = 0; col < pm.columnCount(); ++col) {
                //qDebug() << "row" << row << "col" << col;
                QCOMPARE(pm.mapFromSource(pm.mapToSource(pm.index(row, col))), pm.index(row, col));
            }
        }
        QCOMPARE(indexRowCol(pm.index(0, 0)), QString("0,0"));

        QCOMPARE(pm.rowCount(pm.index(0, 0)), 2);
        QCOMPARE(pm.index(0, 0).parent(), QModelIndex());

        QCOMPARE(pm.mapToSource(pm.index(0, 0)).column(), 2); // column 0 points to C
        QCOMPARE(pm.mapToSource(pm.index(0, 1)).column(), 3); // column 1 points to D

        QCOMPARE(extractRowTexts(&pm, 0), QString("CDBA"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(0, 0)), QString("opnm"));
        QCOMPARE(extractRowTexts(&pm, 1, pm.index(0, 0)), QString("strq"));
        QCOMPARE(extractRowTexts(&pm, 1), QString("GHFE"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QString("z.yx"));
        QCOMPARE(extractHorizontalHeaderTexts(&pm), QString("H3H4H2H1"));

        // Verify tree structure of proxy
        const QModelIndex secondParent = pm.index(1, 0);
        QVERIFY(!secondParent.parent().isValid());
        QCOMPARE(indexToText(pm.index(0, 0, secondParent).parent()), indexToText(secondParent));
        QCOMPARE(indexToText(pm.index(0, 3, secondParent).parent()), indexToText(secondParent));

        QVERIFY(!pm.canFetchMore(QModelIndex()));
    }

    void shouldHandleDataChanged()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        setup(pm);

        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When a cell in a source model changes
        mod.item(0, 2)->setData("c", Qt::EditRole);
        mod.item(0, 3)->setData("d", Qt::EditRole);

        // Then the change should be notified to the proxy
        QCOMPARE(dataChangedSpy.count(), 2);
        QCOMPARE(indexToText(dataChangedSpy.at(0).at(0).value<QModelIndex>()), indexToText(pm.index(0, 0)));
        QCOMPARE(indexToText(dataChangedSpy.at(1).at(0).value<QModelIndex>()), indexToText(pm.index(0, 1)));
        QCOMPARE(extractRowTexts(&pm, 0), QString("cdBA"));
    }

    void shouldHandleDataChangedInChild()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        setup(pm);

        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When a cell in a source model changes
        mod.item(1, 0)->child(0, 3)->setData(",", Qt::EditRole);

        // Then the change should be notified to the proxy
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(indexToText(dataChangedSpy.at(0).at(0).value<QModelIndex>()), indexToText(pm.index(1, 0).child(0, 1)));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QString("z,yx"));
    }

    void shouldSupportSetData()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        setup(pm);

        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When changing data via the proxy
        const QModelIndex idx = pm.index(0, 2);
        QCOMPARE(idx.data().toString(), QString("B"));
        pm.setData(idx, QString("Z"));
        QCOMPARE(idx.data().toString(), QString("Z"));
        QCOMPARE(extractRowTexts(&pm, 0), QString("CDZA"));
        QCOMPARE(extractRowTexts(&mod, 0), QString("AZCDE"));
    }

private:

    // setup proxy
    void setup(KRearrangeColumnsProxyModel &pm)
    {
        pm.setSourceColumns(QVector<int>() << 2 << 3 << 1 << 0);
        pm.setSourceModel(&mod);
        pm.sort(0); // don't forget this!
    }

    static QString indexRowCol(const QModelIndex &index)
    {
        if (!index.isValid()) {
            return "invalid";
        }
        return QString::number(index.row()) + "," + QString::number(index.column());
    }

    static QString indexToText(const QModelIndex &index)
    {
        if (!index.isValid()) {
            return "invalid";
        }
        return QString::number(index.row()) + "," + QString::number(index.column()) + ","
               + QString::number(reinterpret_cast<qulonglong>(index.internalPointer()), 16)
               + " in " + QString::number(reinterpret_cast<qulonglong>(index.model()), 16);
    }

    QStandardItemModel mod;
};

QTEST_MAIN(tst_KRearrangeColumnsProxyModel)

#include "krearrangecolumnsproxymodeltest.moc"
