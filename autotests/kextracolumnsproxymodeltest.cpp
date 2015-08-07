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

#include <kextracolumnsproxymodel.h>
#include "test_model_helpers.h"
using namespace TestModelHelpers;

Q_DECLARE_METATYPE(QModelIndex)

class tst_KExtraColumnsProxyModel : public QObject
{
    Q_OBJECT

private:
    class NoExtraColumns : public KExtraColumnsProxyModel
    {
        QVariant extraColumnData(const QModelIndex &, int, int, int) const Q_DECL_OVERRIDE
        {
            Q_ASSERT(0);
            return QVariant();
        }
    };

    class TwoExtraColumnsProxyModel : public KExtraColumnsProxyModel
    {
    public:
        TwoExtraColumnsProxyModel() : KExtraColumnsProxyModel(), m_extraColumnData('Z')
        {
            appendColumn("H5");
            appendColumn("H6");
        }
        QVariant extraColumnData(const QModelIndex &, int row, int extraColumn, int role) const Q_DECL_OVERRIDE
        {
            Q_ASSERT(role == Qt::DisplayRole);
            switch (extraColumn) {
            case 0:
                return QString(m_extraColumnData);
            case 1:
                return QString::number(row);
            default:
                Q_ASSERT(0);
                return QVariant();
            }
        }
        bool setExtraColumnData(const QModelIndex &parent, int row, int extraColumn, const QVariant &data, int role) Q_DECL_OVERRIDE {
            if (extraColumn == 0 && role == Qt::EditRole)
            {
                m_extraColumnData = data.toString().at(0);
                extraColumnDataChanged(QModelIndex(), 0, extraColumn, QVector<int>() << Qt::EditRole);
                return true;
            }
            return KExtraColumnsProxyModel::setExtraColumnData(parent, row, extraColumn, data, role);
        }
        void changeExtraColumnData()
        {
            m_extraColumnData = '<';
            extraColumnDataChanged(QModelIndex(), 0, 0, QVector<int>() << Qt::EditRole);
        }
    private:
        QChar m_extraColumnData;
    };

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<QModelIndex>();
    }

    void init()
    {
        // Prepare the source model to use later on
        mod.clear();
        mod.appendRow(makeStandardItems(QStringList() << "A" << "B" << "C" << "D"));
        mod.item(0, 0)->appendRow(makeStandardItems(QStringList() << "m" << "n" << "o" << "p"));
        mod.item(0, 0)->appendRow(makeStandardItems(QStringList() << "q" << "r" << "s" << "t"));
        mod.appendRow(makeStandardItems(QStringList() << "E" << "F" << "G" << "H"));
        mod.item(1, 0)->appendRow(makeStandardItems(QStringList() << "x" << "y" << "z" << "."));
        mod.setHorizontalHeaderLabels(QStringList() << "H1" << "H2" << "H3" << "H4");

        QCOMPARE(extractRowTexts(&mod, 0), QString("ABCD"));
        QCOMPARE(extractRowTexts(&mod, 0, mod.index(0, 0)), QString("mnop"));
        QCOMPARE(extractRowTexts(&mod, 1, mod.index(0, 0)), QString("qrst"));
        QCOMPARE(extractRowTexts(&mod, 1), QString("EFGH"));
        QCOMPARE(extractRowTexts(&mod, 0, mod.index(1, 0)), QString("xyz."));
        QCOMPARE(extractHorizontalHeaderTexts(&mod), QString("H1H2H3H4"));

        // test code to see the model
        // showModel(&mod);
    }

    void shouldDoNothingIfNoExtraColumns()
    {
        // Given a extra-columns proxy
        NoExtraColumns pm;

        // When setting it to a source model
        pm.setSourceModel(&mod);

        // Then the proxy should show the same as the model
        QCOMPARE(pm.rowCount(), mod.rowCount());
        QCOMPARE(pm.columnCount(), mod.columnCount());

        QCOMPARE(pm.rowCount(pm.index(0, 0)), 2);
        QCOMPARE(pm.index(0, 0).parent(), QModelIndex());

        // (verify that the mapFromSource(mapToSource(x)) == x roundtrip works)
        for (int row = 0; row < pm.rowCount(); ++row) {
            for (int col = 0; col < pm.columnCount(); ++col) {
                QCOMPARE(pm.mapFromSource(pm.mapToSource(pm.index(row, col))), pm.index(row, col));
            }
        }

        QCOMPARE(extractRowTexts(&pm, 0), QString("ABCD"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(0, 0)), QString("mnop"));
        QCOMPARE(extractRowTexts(&pm, 1, pm.index(0, 0)), QString("qrst"));
        QCOMPARE(extractRowTexts(&pm, 1), QString("EFGH"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QString("xyz."));
        QCOMPARE(extractHorizontalHeaderTexts(&pm), QString("H1H2H3H4"));
    }

    void shouldShowExtraColumns()
    {
        // Given a extra-columns proxy with two extra columns
        TwoExtraColumnsProxyModel pm;

        // When setting it to a source model
        pm.setSourceModel(&mod);

        // Then the proxy should show the extra column
        QCOMPARE(extractRowTexts(&pm, 0), QString("ABCDZ0"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(0, 0)), QString("mnopZ0"));
        QCOMPARE(extractRowTexts(&pm, 1, pm.index(0, 0)), QString("qrstZ1"));
        QCOMPARE(extractRowTexts(&pm, 1), QString("EFGHZ1"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QString("xyz.Z0"));
        QCOMPARE(extractHorizontalHeaderTexts(&pm), QString("H1H2H3H4H5H6"));

        // Verify tree structure of proxy
        const QModelIndex secondParent = pm.index(1, 0);
        QVERIFY(!secondParent.parent().isValid());
        QCOMPARE(indexToText(pm.index(0, 0, secondParent).parent()), indexToText(secondParent));
        QCOMPARE(indexToText(pm.index(0, 3, secondParent).parent()), indexToText(secondParent));
        QVERIFY(indexToText(pm.index(0, 4)).startsWith("0,4,"));
        QCOMPARE(indexToText(pm.index(0, 4, secondParent).parent()), indexToText(secondParent));
        QVERIFY(indexToText(pm.index(0, 5)).startsWith("0,5,"));
        QCOMPARE(indexToText(pm.index(0, 5, secondParent).parent()), indexToText(secondParent));

        QCOMPARE(pm.index(0, 0).sibling(0, 4).column(), 4);
        QCOMPARE(pm.index(0, 4).sibling(0, 1).column(), 1);

        QVERIFY(!pm.canFetchMore(QModelIndex()));
    }

    void shouldHandleDataChanged()
    {
        // Given a extra-columns proxy, with two extra columns
        TwoExtraColumnsProxyModel pm;
        setup(pm);
        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When a cell in a source model changes
        mod.item(0, 2)->setData("c", Qt::EditRole);

        // Then the change should be notified to the proxy
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), pm.index(0, 2));
        QCOMPARE(extractRowTexts(&pm, 0), QString("ABcDZ0"));
    }

    void shouldHandleDataChangedInExtraColumn()
    {
        // Given a extra-columns proxy, with two extra columns
        TwoExtraColumnsProxyModel pm;
        setup(pm);
        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When the proxy wants to signal a change in an extra column
        pm.changeExtraColumnData();

        // Then the change should be available and notified
        QCOMPARE(extractRowTexts(&pm, 0), QString("ABCD<0"));
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), pm.index(0, 4));
    }

    void shouldHandleSetDataInNormalColumn()
    {
        // Given a extra-columns proxy, with two extra columns
        TwoExtraColumnsProxyModel pm;
        setup(pm);
        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When editing a cell in the proxy
        QVERIFY(pm.setData(pm.index(0, 2), "c", Qt::EditRole));

        // Then the change should be available and notified
        QCOMPARE(extractRowTexts(&pm, 0), QString("ABcDZ0"));
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), pm.index(0, 2));
    }

    void shouldHandleSetDataInExtraColumn()
    {
        // Given a extra-columns proxy, with two extra columns
        TwoExtraColumnsProxyModel pm;
        setup(pm);
        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When editing a cell in the proxy
        QVERIFY(pm.setData(pm.index(0, 4), "-", Qt::EditRole));

        // Then the change should be available and notified
        QCOMPARE(extractRowTexts(&pm, 0), QString("ABCD-0"));
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(dataChangedSpy.at(0).at(0).value<QModelIndex>(), pm.index(0, 4));
    }

    void shouldHandleRowInsertion()
    {
        // Given a extra-columns proxy, with two extra columns
        TwoExtraColumnsProxyModel pm;
        setup(pm);

        QSignalSpy rowATBISpy(&pm, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
        QSignalSpy rowInsertedSpy(&pm, SIGNAL(rowsInserted(QModelIndex,int,int)));

        // When a source model inserts a new (child) row
        mod.item(1, 0)->appendRow(makeStandardItems(QStringList() << "1" << "2" << "3" << "4"));

        // Then the proxy should notify its users and show changes
        QCOMPARE(rowSpyToText(rowATBISpy), QString("1,1"));
        QCOMPARE(rowSpyToText(rowInsertedSpy), QString("1,1"));
        QCOMPARE(pm.rowCount(), 2);
        QCOMPARE(extractRowTexts(&pm, 0), QString("ABCDZ0"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(0, 0)), QString("mnopZ0"));
        QCOMPARE(extractRowTexts(&pm, 1, pm.index(0, 0)), QString("qrstZ1"));
        QCOMPARE(extractRowTexts(&pm, 1), QString("EFGHZ1"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QString("xyz.Z0"));
        QCOMPARE(extractRowTexts(&pm, 1, pm.index(1, 0)), QString("1234Z1"));
        QCOMPARE(extractHorizontalHeaderTexts(&pm), QString("H1H2H3H4H5H6"));
    }

    void shouldHandleColumnInsertion()
    {
        // Given a extra-columns proxy, with two extra columns
        TwoExtraColumnsProxyModel pm;
        setup(pm);

        QCOMPARE(pm.columnCount(), 6);
        QCOMPARE(mod.columnCount(), 4);

        QSignalSpy colATBISpy(&pm, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)));
        QSignalSpy colInsertedSpy(&pm, SIGNAL(columnsInserted(QModelIndex,int,int)));

        // When a source model inserts a new column
        mod.setColumnCount(5); // like QStandardItem::setChild does
        QCOMPARE(mod.columnCount(), 5);
        // QStandardItemModel is quite dumb, it records the number of columns in each item
        for (int row = 0; row < mod.rowCount(); ++row) {
            mod.item(row, 0)->setColumnCount(5);
        }

        // Then the proxy should notify its users and show changes
        QCOMPARE(rowSpyToText(colATBISpy), QString("4,4;4,4;4,4")); // QStandardItemModel emits it for each parent
        QCOMPARE(rowSpyToText(colInsertedSpy), QString("4,4;4,4;4,4"));
        QCOMPARE(pm.columnCount(), 7);
        QCOMPARE(extractRowTexts(&pm, 0), QString("ABCD Z0"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(0, 0)), QString("mnop Z0"));
        QCOMPARE(extractRowTexts(&pm, 1, pm.index(0, 0)), QString("qrst Z1"));
        QCOMPARE(extractRowTexts(&pm, 1), QString("EFGH Z1"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QString("xyz. Z0"));
        QCOMPARE(extractHorizontalHeaderTexts(&pm), QString("H1H2H3H45H5H6")); // '5' was inserted in there
    }

    // row removal, layoutChanged, modelReset -> same thing, works via QIdentityProxyModel
    // missing: test for mapSelectionToSource

    void shouldHandleLayoutChanged()
    {
        // Given a extra-columns proxy, with two extra columns
        TwoExtraColumnsProxyModel pm;
        // And a QSFPM underneath
        QSortFilterProxyModel proxy;
        proxy.setSourceModel(&mod);
        QCOMPARE(proxy.columnCount(), 4);
        pm.setSourceModel(&proxy);
        QCOMPARE(pm.columnCount(), 6);
        QCOMPARE(extractRowTexts(&pm, 0), QString("ABCDZ0"));
        // And a selection
        QItemSelectionModel selection(&pm);
        selection.select(pm.index(0, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        const QModelIndexList lst = selection.selectedIndexes();
        QCOMPARE(lst.count(), 6);
        for (int col = 0; col < lst.count(); ++col) {
            QCOMPARE(lst.at(col).row(), 0);
            QCOMPARE(lst.at(col).column(), col);
        }

        // When sorting
        pm.sort(0, Qt::DescendingOrder);

        // Then the proxy should be sorted
        QCOMPARE(extractRowTexts(&pm, 0), QString("EFGHZ0"));
        QCOMPARE(extractRowTexts(&pm, 1), QString("ABCDZ1"));
        // And the selection should be updated accordingly
        const QModelIndexList lstAfter = selection.selectedIndexes();
        QCOMPARE(lstAfter.count(), 6);
        for (int col = 0; col < lstAfter.count(); ++col) {
            QCOMPARE(lstAfter.at(col).row(), 1);
            QCOMPARE(lstAfter.at(col).column(), col);
        }
    }

private:

    void setup(KExtraColumnsProxyModel &pm)
    {
        pm.setSourceModel(&mod);
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

QTEST_MAIN(tst_KExtraColumnsProxyModel)

#include "kextracolumnsproxymodeltest.moc"
