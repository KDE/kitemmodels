#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QTest>
#include <QStandardItemModel>
#include <QIdentityProxyModel>
#include <QItemSelectionModel>

#include <kconcatenaterowsproxymodel.h>
#include "test_model_helpers.h"
using namespace TestModelHelpers;

Q_DECLARE_METATYPE(QModelIndex)

class tst_KConcatenateRowsProxyModel : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
    }

    void init()
    {
        // Prepare some source models to use later on
        mod.clear();
        mod.insertRow(0, makeStandardItems(QStringList() << QStringLiteral("A") << QStringLiteral("B") << QStringLiteral("C")));
        mod.setHorizontalHeaderLabels(QStringList() << QStringLiteral("H1") << QStringLiteral("H2") << QStringLiteral("H3"));
        mod.setVerticalHeaderLabels(QStringList() << QStringLiteral("One"));

        mod2.clear();
        mod2.insertRow(0, makeStandardItems(QStringList() << QStringLiteral("D") << QStringLiteral("E") << QStringLiteral("F")));
        mod2.setHorizontalHeaderLabels(QStringList() << QStringLiteral("H1") << QStringLiteral("H2") << QStringLiteral("H3"));
        mod2.setVerticalHeaderLabels(QStringList() << QStringLiteral("Two"));
    }

    void shouldAggregateTwoModelsCorrectly()
    {
        // Given a combining proxy
        KConcatenateRowsProxyModel pm;

        // When adding two source models
        pm.addSourceModel(&mod);
        pm.addSourceModel(&mod2);

        // Then the proxy should show 2 rows
        QCOMPARE(pm.rowCount(), 2);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEF"));

        // ... and correct headers
        QCOMPARE(pm.headerData(0, Qt::Horizontal).toString(), QStringLiteral("H1"));
        QCOMPARE(pm.headerData(1, Qt::Horizontal).toString(), QStringLiteral("H2"));
        QCOMPARE(pm.headerData(2, Qt::Horizontal).toString(), QStringLiteral("H3"));
        QCOMPARE(pm.headerData(0, Qt::Vertical).toString(), QStringLiteral("One"));
        QCOMPARE(pm.headerData(1, Qt::Vertical).toString(), QStringLiteral("Two"));

        QVERIFY(!pm.canFetchMore(QModelIndex()));
    }

    void shouldAggregateThenRemoveTwoEmptyModelsCorrectly()
    {
        // Given a combining proxy
        KConcatenateRowsProxyModel pm;

        // When adding two empty models
        QSignalSpy rowATBISpy(&pm, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
        QSignalSpy rowInsertedSpy(&pm, SIGNAL(rowsInserted(QModelIndex,int,int)));
        QSignalSpy rowATBRSpy(&pm, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
        QSignalSpy rowRemovedSpy(&pm, SIGNAL(rowsRemoved(QModelIndex,int,int)));
        QIdentityProxyModel i1, i2;
        pm.addSourceModel(&i1);
        pm.addSourceModel(&i2);

        // Then the proxy should still be empty (and no signals emitted)
        QCOMPARE(pm.rowCount(), 0);
        QCOMPARE(pm.columnCount(), 0);
        QCOMPARE(rowATBISpy.count(), 0);
        QCOMPARE(rowInsertedSpy.count(), 0);

        // When removing the empty models
        pm.removeSourceModel(&i1);
        pm.removeSourceModel(&i2);

        // Then the proxy should still be empty (and no signals emitted)
        QCOMPARE(pm.rowCount(), 0);
        QCOMPARE(pm.columnCount(), 0);
        QCOMPARE(rowATBRSpy.count(), 0);
        QCOMPARE(rowRemovedSpy.count(), 0);
    }

    void shouldAggregateTwoEmptyModelsWhichThenGetFilled()
    {
        // Given a combining proxy
        KConcatenateRowsProxyModel pm;

        // When adding two empty models
        QIdentityProxyModel i1, i2;
        pm.addSourceModel(&i1);
        pm.addSourceModel(&i2);

        QCOMPARE(pm.rowCount(), 0);
        QCOMPARE(pm.columnCount(), 0);

        i1.setSourceModel(&mod);
        i2.setSourceModel(&mod2);

        // Then the proxy should show 2 rows
        QCOMPARE(pm.rowCount(), 2);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEF"));

        // ... and correct headers
        QCOMPARE(pm.headerData(0, Qt::Horizontal).toString(), QStringLiteral("H1"));
        QCOMPARE(pm.headerData(1, Qt::Horizontal).toString(), QStringLiteral("H2"));
        QCOMPARE(pm.headerData(2, Qt::Horizontal).toString(), QStringLiteral("H3"));
        QCOMPARE(pm.headerData(0, Qt::Vertical).toString(), QStringLiteral("One"));
        QCOMPARE(pm.headerData(1, Qt::Vertical).toString(), QStringLiteral("Two"));

        QVERIFY(!pm.canFetchMore(QModelIndex()));
    }

    void shouldHandleDataChanged()
    {
        // Given two models combined
        KConcatenateRowsProxyModel pm;
        pm.addSourceModel(&mod);
        pm.addSourceModel(&mod2);
        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When a cell in a source model changes
        mod.item(0, 0)->setData("a", Qt::EditRole);

        // Then the change should be notified to the proxy
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), pm.index(0, 0));
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("aBC"));

        // Same test with the other model
        mod2.item(0, 2)->setData("f", Qt::EditRole);

        QCOMPARE(dataChangedSpy.count(), 2);
        QCOMPARE(dataChangedSpy.at(1).at(0).toModelIndex(), pm.index(1, 2));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEf"));
    }

    void shouldHandleSetData()
    {
        // Given two models combined
        KConcatenateRowsProxyModel pm;
        pm.addSourceModel(&mod);
        pm.addSourceModel(&mod2);
        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

        // When changing a cell using setData
        pm.setData(pm.index(0, 0), "a", Qt::EditRole);

        // Then the change should be notified to the proxy
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(dataChangedSpy.at(0).at(0).toModelIndex(), pm.index(0, 0));
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("aBC"));

        // Same test with the other model
        pm.setData(pm.index(1, 2), "f", Qt::EditRole);

        QCOMPARE(dataChangedSpy.count(), 2);
        QCOMPARE(dataChangedSpy.at(1).at(0).toModelIndex(), pm.index(1, 2));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEf"));
    }

    void shouldHandleRowInsertionAndRemoval()
    {
        // Given two models combined
        KConcatenateRowsProxyModel pm;
        pm.addSourceModel(&mod);
        pm.addSourceModel(&mod2);
        QSignalSpy rowATBISpy(&pm, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
        QSignalSpy rowInsertedSpy(&pm, SIGNAL(rowsInserted(QModelIndex,int,int)));

        // When a source model inserts a new row
        QList<QStandardItem *> row;
        row.append(new QStandardItem(QStringLiteral("1")));
        row.append(new QStandardItem(QStringLiteral("2")));
        row.append(new QStandardItem(QStringLiteral("3")));
        mod2.insertRow(0, row);

        // Then the proxy should notify its users and show changes
        QCOMPARE(rowSpyToText(rowATBISpy), QStringLiteral("1,1"));
        QCOMPARE(rowSpyToText(rowInsertedSpy), QStringLiteral("1,1"));
        QCOMPARE(pm.rowCount(), 3);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("123"));
        QCOMPARE(extractRowTexts(&pm, 2), QStringLiteral("DEF"));

        // When removing that row
        QSignalSpy rowATBRSpy(&pm, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
        QSignalSpy rowRemovedSpy(&pm, SIGNAL(rowsRemoved(QModelIndex,int,int)));
        mod2.removeRow(0);

        // Then the proxy should notify its users and show changes
        QCOMPARE(rowATBRSpy.count(), 1);
        QCOMPARE(rowATBRSpy.at(0).at(1).toInt(), 1);
        QCOMPARE(rowATBRSpy.at(0).at(2).toInt(), 1);
        QCOMPARE(rowRemovedSpy.count(), 1);
        QCOMPARE(rowRemovedSpy.at(0).at(1).toInt(), 1);
        QCOMPARE(rowRemovedSpy.at(0).at(2).toInt(), 1);
        QCOMPARE(pm.rowCount(), 2);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEF"));

        // When removing the last row from mod2
        rowATBRSpy.clear();
        rowRemovedSpy.clear();
        mod2.removeRow(0);

        // Then the proxy should notify its users and show changes
        QCOMPARE(rowATBRSpy.count(), 1);
        QCOMPARE(rowATBRSpy.at(0).at(1).toInt(), 1);
        QCOMPARE(rowATBRSpy.at(0).at(2).toInt(), 1);
        QCOMPARE(rowRemovedSpy.count(), 1);
        QCOMPARE(rowRemovedSpy.at(0).at(1).toInt(), 1);
        QCOMPARE(rowRemovedSpy.at(0).at(2).toInt(), 1);
        QCOMPARE(pm.rowCount(), 1);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
    }

    void shouldAggregateAnotherModelThenRemoveModels()
    {
        // Given two models combined, and a third model
        KConcatenateRowsProxyModel pm;
        pm.addSourceModel(&mod);
        pm.addSourceModel(&mod2);

        QStandardItemModel mod3;
        mod3.appendRow(makeStandardItems(QStringList() << QStringLiteral("1") << QStringLiteral("2") << QStringLiteral("3")));
        mod3.appendRow(makeStandardItems(QStringList() << QStringLiteral("4") << QStringLiteral("5") << QStringLiteral("6")));

        QSignalSpy rowATBISpy(&pm, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
        QSignalSpy rowInsertedSpy(&pm, SIGNAL(rowsInserted(QModelIndex,int,int)));

        // When adding the new source model
        pm.addSourceModel(&mod3);

        // Then the proxy should notify its users about the two rows inserted
        QCOMPARE(rowSpyToText(rowATBISpy), QStringLiteral("2,3"));
        QCOMPARE(rowSpyToText(rowInsertedSpy), QStringLiteral("2,3"));
        QCOMPARE(pm.rowCount(), 4);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEF"));
        QCOMPARE(extractRowTexts(&pm, 2), QStringLiteral("123"));
        QCOMPARE(extractRowTexts(&pm, 3), QStringLiteral("456"));

        // When removing that source model again
        QSignalSpy rowATBRSpy(&pm, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
        QSignalSpy rowRemovedSpy(&pm, SIGNAL(rowsRemoved(QModelIndex,int,int)));
        pm.removeSourceModel(&mod3);

        // Then the proxy should notify its users about the row removed
        QCOMPARE(rowATBRSpy.count(), 1);
        QCOMPARE(rowATBRSpy.at(0).at(1).toInt(), 2);
        QCOMPARE(rowATBRSpy.at(0).at(2).toInt(), 3);
        QCOMPARE(rowRemovedSpy.count(), 1);
        QCOMPARE(rowRemovedSpy.at(0).at(1).toInt(), 2);
        QCOMPARE(rowRemovedSpy.at(0).at(2).toInt(), 3);
        QCOMPARE(pm.rowCount(), 2);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEF"));

        // When removing model 2
        rowATBRSpy.clear();
        rowRemovedSpy.clear();
        pm.removeSourceModel(&mod2);
        QCOMPARE(rowATBRSpy.count(), 1);
        QCOMPARE(rowATBRSpy.at(0).at(1).toInt(), 1);
        QCOMPARE(rowATBRSpy.at(0).at(2).toInt(), 1);
        QCOMPARE(rowRemovedSpy.count(), 1);
        QCOMPARE(rowRemovedSpy.at(0).at(1).toInt(), 1);
        QCOMPARE(rowRemovedSpy.at(0).at(2).toInt(), 1);
        QCOMPARE(pm.rowCount(), 1);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));

        // When removing model 1
        rowATBRSpy.clear();
        rowRemovedSpy.clear();
        pm.removeSourceModel(&mod);
        QCOMPARE(rowATBRSpy.count(), 1);
        QCOMPARE(rowATBRSpy.at(0).at(1).toInt(), 0);
        QCOMPARE(rowATBRSpy.at(0).at(2).toInt(), 0);
        QCOMPARE(rowRemovedSpy.count(), 1);
        QCOMPARE(rowRemovedSpy.at(0).at(1).toInt(), 0);
        QCOMPARE(rowRemovedSpy.at(0).at(2).toInt(), 0);
        QCOMPARE(pm.rowCount(), 0);
    }

    void shouldHandleColumnInsertionAndRemoval()
    {
        // Given two models combined
        KConcatenateRowsProxyModel pm;
        pm.addSourceModel(&mod);
        pm.addSourceModel(&mod2);
        QSignalSpy colATBISpy(&pm, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)));
        QSignalSpy colInsertedSpy(&pm, SIGNAL(columnsInserted(QModelIndex,int,int)));

        // When the first source model inserts a new column
        QCOMPARE(mod.columnCount(), 3);
        mod.setColumnCount(4);

        // Then the proxy should notify its users and show changes
        QCOMPARE(rowSpyToText(colATBISpy), QStringLiteral("3,3"));
        QCOMPARE(rowSpyToText(colInsertedSpy), QStringLiteral("3,3"));
        QCOMPARE(pm.rowCount(), 2);
        QCOMPARE(pm.columnCount(), 4);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC "));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEF "));
    }

    void shouldPropagateLayoutChanged()
    {
        // Given two source models, the second one being a QSFPM
        KConcatenateRowsProxyModel pm;
        pm.addSourceModel(&mod);

        QStandardItemModel mod3;
        QList<QStandardItem *> row;
        row.append(new QStandardItem(QStringLiteral("1")));
        row.append(new QStandardItem(QStringLiteral("2")));
        row.append(new QStandardItem(QStringLiteral("3")));
        mod3.insertRow(0, row);
        row.clear();
        row.append(new QStandardItem(QStringLiteral("4")));
        row.append(new QStandardItem(QStringLiteral("5")));
        row.append(new QStandardItem(QStringLiteral("6")));
        mod3.appendRow(row);

        QSortFilterProxyModel qsfpm;
        qsfpm.setSourceModel(&mod3);
        pm.addSourceModel(&qsfpm);

        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("123"));
        QCOMPARE(extractRowTexts(&pm, 2), QStringLiteral("456"));

        // And a selection (row 1)
        QItemSelectionModel selection(&pm);
        selection.select(pm.index(1, 0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
        const QModelIndexList lst = selection.selectedIndexes();
        QCOMPARE(lst.count(), 3);
        for (int col = 0; col < lst.count(); ++col) {
            QCOMPARE(lst.at(col).row(), 1);
            QCOMPARE(lst.at(col).column(), col);
        }

        QSignalSpy layoutATBCSpy(&pm, SIGNAL(layoutAboutToBeChanged()));
        QSignalSpy layoutChangedSpy(&pm, SIGNAL(layoutChanged()));

        // When changing the sorting in the QSFPM
        qsfpm.sort(0, Qt::DescendingOrder);

        // Then the proxy should emit the layoutChanged signals, and show re-sorted data
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("456"));
        QCOMPARE(extractRowTexts(&pm, 2), QStringLiteral("123"));
        QCOMPARE(layoutATBCSpy.count(), 1);
        QCOMPARE(layoutChangedSpy.count(), 1);

        // And the selection should be updated accordingly (it became row 2)
        const QModelIndexList lstAfter = selection.selectedIndexes();
        QCOMPARE(lstAfter.count(), 3);
        for (int col = 0; col < lstAfter.count(); ++col) {
            QCOMPARE(lstAfter.at(col).row(), 2);
            QCOMPARE(lstAfter.at(col).column(), col);
        }
    }

    void shouldReactToModelReset()
    {
        // Given two source models, the second one being a QSFPM
        KConcatenateRowsProxyModel pm;
        pm.addSourceModel(&mod);

        QStandardItemModel mod3;
        QList<QStandardItem *> row;
        row.append(new QStandardItem(QStringLiteral("1")));
        row.append(new QStandardItem(QStringLiteral("2")));
        row.append(new QStandardItem(QStringLiteral("3")));
        mod3.insertRow(0, row);
        row.clear();
        row.append(new QStandardItem(QStringLiteral("4")));
        row.append(new QStandardItem(QStringLiteral("5")));
        row.append(new QStandardItem(QStringLiteral("6")));
        mod3.appendRow(row);

        QSortFilterProxyModel qsfpm;
        qsfpm.setSourceModel(&mod3);
        pm.addSourceModel(&qsfpm);

        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("123"));
        QCOMPARE(extractRowTexts(&pm, 2), QStringLiteral("456"));
        QSignalSpy rowATBRSpy(&pm, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)));
        QSignalSpy rowRemovedSpy(&pm, SIGNAL(rowsRemoved(QModelIndex,int,int)));
        QSignalSpy rowATBISpy(&pm, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
        QSignalSpy rowInsertedSpy(&pm, SIGNAL(rowsInserted(QModelIndex,int,int)));

        // When changing the source model of the QSFPM
        qsfpm.setSourceModel(&mod2);

        // Then the proxy should emit the row removed/inserted signals, and show the new data
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("ABC"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("DEF"));
        QCOMPARE(rowSpyToText(rowATBRSpy), QStringLiteral("1,2"));
        QCOMPARE(rowSpyToText(rowRemovedSpy), QStringLiteral("1,2"));
        QCOMPARE(rowSpyToText(rowATBISpy), QStringLiteral("1,1"));
        QCOMPARE(rowSpyToText(rowInsertedSpy), QStringLiteral("1,1"));
    }

private:
    QStandardItemModel mod;
    QStandardItemModel mod2;
};

QTEST_MAIN(tst_KConcatenateRowsProxyModel)

#include "kconcatenaterowsproxymodeltest.moc"
