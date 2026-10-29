/*
    SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
    SPDX-FileContributor: David Faure <david.faure@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QAbstractItemModelTester>
#include <QDebug>
#include <QMimeData>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QTest>

#include "test_model_helpers.h"
#include <krearrangecolumnsproxymodel.h>
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
        mod.appendRow(makeStandardItems(QStringList() << QStringLiteral("A") << QStringLiteral("B") << QStringLiteral("C") << QStringLiteral("D")
                                                      << QStringLiteral("E")));
        mod.item(0, 0)->appendRow(makeStandardItems(QStringList() << QStringLiteral("m") << QStringLiteral("n") << QStringLiteral("o") << QStringLiteral("p")
                                                                  << QStringLiteral("-")));
        mod.item(0, 0)->appendRow(makeStandardItems(QStringList() << QStringLiteral("q") << QStringLiteral("r") << QStringLiteral("s") << QStringLiteral("t")
                                                                  << QStringLiteral("-")));
        mod.appendRow(makeStandardItems(QStringList() << QStringLiteral("E") << QStringLiteral("F") << QStringLiteral("G") << QStringLiteral("H")
                                                      << QStringLiteral("I")));
        mod.item(1, 0)->appendRow(makeStandardItems(QStringList() << QStringLiteral("x") << QStringLiteral("y") << QStringLiteral("z") << QStringLiteral(".")
                                                                  << QStringLiteral("-")));
        mod.setHorizontalHeaderLabels(QStringList() << QStringLiteral("H1") << QStringLiteral("H2") << QStringLiteral("H3") << QStringLiteral("H4")
                                                    << QStringLiteral("H5"));

        QCOMPARE(extractRowTexts(&mod, 0), QStringLiteral("ABCDE"));
        QCOMPARE(extractRowTexts(&mod, 0, mod.index(0, 0)), QStringLiteral("mnop-"));
        QCOMPARE(extractRowTexts(&mod, 1, mod.index(0, 0)), QStringLiteral("qrst-"));
        QCOMPARE(extractRowTexts(&mod, 1), QStringLiteral("EFGHI"));
        QCOMPARE(extractRowTexts(&mod, 0, mod.index(1, 0)), QStringLiteral("xyz.-"));
        QCOMPARE(extractHorizontalHeaderTexts(&mod), QStringLiteral("H1H2H3H4H5"));

        // test code to see the model
        // showModel(&mod);
    }

    void shouldShowNothingIfNoSourceModel()
    {
        // Given a rearrange-columns proxy with no source model
        KRearrangeColumnsProxyModel pm;

        // Then the proxy should show nothing (no columns selected)
        QCOMPARE(pm.rowCount(), 0);
        QCOMPARE(pm.columnCount(), 0);
        QVERIFY(!pm.index(0, 0).isValid());
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
        QVERIFY(!pm.hasChildren());
    }

    void shouldMapColumns()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        pm.setSourceColumns(QList<int>() << 3 << 1 << 0);

        // When using that proxy on top of an empty source model
        QStandardItemModel sourceModel;
        sourceModel.setColumnCount(4);
        pm.setSourceModel(&sourceModel);

        // Then the mapping methods should work
        QCOMPARE(pm.proxyColumnForSourceColumn(0), 2);
        QCOMPARE(pm.proxyColumnForSourceColumn(1), 1);
        QCOMPARE(pm.proxyColumnForSourceColumn(2), -1);
        QCOMPARE(pm.proxyColumnForSourceColumn(3), 0);
        QCOMPARE(pm.sourceColumnForProxyColumn(0), 3);
        QCOMPARE(pm.sourceColumnForProxyColumn(1), 1);
        QCOMPARE(pm.sourceColumnForProxyColumn(2), 0);

        // And mapFromSource should return invalid for unmapped cells
        QVERIFY(!pm.mapFromSource(sourceModel.index(0, 2)).isValid());
    }

    void shouldShowNothingIfNoRows()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        pm.setSourceColumns(QList<int>() << 2 << 3 << 1 << 0);

        // When using that proxy on top of an empty source model
        QStandardItemModel sourceModel;
        sourceModel.setColumnCount(4);
        pm.setSourceModel(&sourceModel);

        // Then the proxy should show nothing
        QCOMPARE(pm.rowCount(), 0);
        QCOMPARE(pm.columnCount(), 4);
        QCOMPARE(pm.index(0, 0), pm.index(0, 0)); // like QAbstractItemView::setModel does in a Q_ASSERT_X
    }

    void shouldRearrangeColumns()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);

        // When setting it to a source model, with columns rearranged
        setup(pm);

        // Then the proxy should show columns reordered
        QCOMPARE(pm.rowCount(), 2);

        // (verify that the mapFromSource(mapToSource(x)) == x roundtrip works)
        for (int row = 0; row < pm.rowCount(); ++row) {
            for (int col = 0; col < pm.columnCount(); ++col) {
                // qDebug() << "row" << row << "col" << col;
                QCOMPARE(pm.mapFromSource(pm.mapToSource(pm.index(row, col))), pm.index(row, col));
            }
        }
        QCOMPARE(indexRowCol(pm.index(0, 0)), QStringLiteral("0,0"));

        QCOMPARE(pm.rowCount(pm.index(0, 0)), 2);
        QCOMPARE(pm.index(0, 0).parent(), QModelIndex());

        QCOMPARE(pm.mapToSource(pm.index(0, 0)).column(), 2); // column 0 points to C
        QCOMPARE(pm.mapToSource(pm.index(0, 1)).column(), 3); // column 1 points to D

        QCOMPARE(pm.sibling(0, 1, pm.index(0, 0)).column(), 1);
        QCOMPARE(pm.sibling(0, 0, pm.index(0, 1)).column(), 0);

        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("CDBA"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(0, 0)), QStringLiteral("opnm"));
        QCOMPARE(extractRowTexts(&pm, 1, pm.index(0, 0)), QStringLiteral("strq"));
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("GHFE"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QStringLiteral("z.yx"));
        QCOMPARE(extractHorizontalHeaderTexts(&pm), QStringLiteral("H3H4H2H1"));

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

        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex, QModelIndex)));

        // When a cell in a source model changes
        mod.item(0, 2)->setData("c", Qt::EditRole);
        mod.item(0, 3)->setData("d", Qt::EditRole);

        // Then the change should be notified to the proxy
        QCOMPARE(dataChangedSpy.count(), 2);
        QCOMPARE(indexToText(dataChangedSpy.at(0).at(0).toModelIndex()), indexToText(pm.index(0, 0)));
        QCOMPARE(indexToText(dataChangedSpy.at(1).at(0).toModelIndex()), indexToText(pm.index(0, 1)));
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("cdBA"));
    }

    void shouldHandleDataChangedInChild()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        setup(pm);

        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex, QModelIndex)));

        // When a cell in a source model changes
        mod.item(1, 0)->child(0, 3)->setData(",", Qt::EditRole);

        // Then the change should be notified to the proxy
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(indexToText(dataChangedSpy.at(0).at(0).toModelIndex()), indexToText(pm.index(0, 1, pm.index(1, 0))));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QStringLiteral("z,yx"));
    }

    void shouldSupportSetData()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        setup(pm);

        QSignalSpy dataChangedSpy(&pm, SIGNAL(dataChanged(QModelIndex, QModelIndex)));

        // When changing data via the proxy
        const QModelIndex idx = pm.index(0, 2);
        QCOMPARE(idx.data().toString(), QStringLiteral("B"));
        pm.setData(idx, QStringLiteral("Z"));
        QCOMPARE(idx.data().toString(), QStringLiteral("Z"));
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("CDZA"));
        QCOMPARE(extractRowTexts(&mod, 0), QStringLiteral("AZCDE"));
    }

    void shouldNotNestReset()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        connect(&mod, &QAbstractItemModel::modelReset, &pm, [&] {
            pm.setSourceColumns({});
        });
        setup(pm);
        QSignalSpy resetSpy(&pm, SIGNAL(modelReset()));

        // When calling setSourceColumns in the middle of a reset
        mod.clear();

        // Then no nested signals should be emitted
        QCOMPARE(resetSpy.count(), 1);
        QCOMPARE(pm.rowCount(), 0);
        QCOMPARE(pm.columnCount(), 0);
    }

    void shouldHandleChildRowInsertionAndRemoval_data()
    {
        QTest::addColumn<QList<int>>("sourceColumns");
        QTest::addColumn<QString>("expectedChildTexts");

        // Source column 0 shown in the last proxy column, or not shown at all
        QTest::newRow("column0-moved") << QList<int>{2, 3, 1, 0} << QStringLiteral("ghfe");
        QTest::newRow("column0-hidden") << QList<int>{2, 3, 1} << QStringLiteral("ghf");
    }

    void shouldHandleChildRowInsertionAndRemoval()
    {
        QFETCH(QList<int>, sourceColumns);
        QFETCH(QString, expectedChildTexts);

        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        pm.setSourceColumns(sourceColumns);
        pm.setSourceModel(&mod);
        QSignalSpy rowsInsertedSpy(&pm, &QAbstractItemModel::rowsInserted);
        QSignalSpy rowsRemovedSpy(&pm, &QAbstractItemModel::rowsRemoved);

        // When inserting a child row in the source model
        mod.item(0, 0)->appendRow(makeStandardItems({QStringLiteral("e"), QStringLiteral("f"), QStringLiteral("g"), QStringLiteral("h"), QStringLiteral("-")}));

        // Then the proxy should notify it under the parent in proxy column 0
        const QModelIndex proxyParent = pm.index(0, 0);
        QCOMPARE(rowsInsertedSpy.count(), 1);
        QCOMPARE(rowsInsertedSpy.at(0).at(0).toModelIndex(), proxyParent);
        QCOMPARE(rowSpyToText(rowsInsertedSpy), QStringLiteral("2,2"));
        QCOMPARE(pm.rowCount(proxyParent), 3);
        QCOMPARE(extractRowTexts(&pm, 2, proxyParent), expectedChildTexts);

        // When removing it
        mod.item(0, 0)->removeRow(2);

        // Then the proxy should notify that too
        QCOMPARE(rowsRemovedSpy.count(), 1);
        QCOMPARE(rowsRemovedSpy.at(0).at(0).toModelIndex(), proxyParent);
        QCOMPARE(rowSpyToText(rowsRemovedSpy), QStringLiteral("2,2"));
        QCOMPARE(pm.rowCount(proxyParent), 2);
    }

    void shouldIgnoreChildrenOfOtherColumns()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        setup(pm);
        QSignalSpy rowsInsertedSpy(&pm, &QAbstractItemModel::rowsInserted);
        QSignalSpy dataChangedSpy(&pm, &QAbstractItemModel::dataChanged);

        // When source column 1 gets children, which the proxy doesn't show
        mod.item(0, 1)->appendRow(makeStandardItems({QStringLiteral("u"), QStringLiteral("v")}));
        mod.item(0, 1)->child(0, 0)->setText(QStringLiteral("w"));
        mod.item(0, 1)->child(0, 0)->appendRow(makeStandardItems({QStringLiteral("x")}));

        // Then nothing should be notified
        QCOMPARE(rowsInsertedSpy.count(), 0);
        QCOMPARE(dataChangedSpy.count(), 0);
        QCOMPARE(pm.rowCount(pm.index(0, 2)), 0);
    }

    void shouldHandleRowMoves()
    {
        // Given a rearrange-columns proxy on a list model
        QStringListModel sourceModel({QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")});
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        pm.setSourceColumns({0});
        pm.setSourceModel(&sourceModel);
        QSignalSpy rowsMovedSpy(&pm, &QAbstractItemModel::rowsMoved);

        // When moving a row in the source model
        QVERIFY(sourceModel.moveRows(QModelIndex(), 0, 1, QModelIndex(), 3));

        // Then the proxy should notify the move
        QCOMPARE(rowsMovedSpy.count(), 1);
        QCOMPARE(pm.index(2, 0).data().toString(), QStringLiteral("a"));
    }

    void shouldHandleRowMovesFromAndToHiddenChildren()
    {
        // Given a rearrange-columns proxy, and children under source column 1, which the proxy doesn't show
        RowMovingModel sourceModel;
        sourceModel.appendRow(makeStandardItems({QStringLiteral("A"), QStringLiteral("B")}));
        sourceModel.appendRow(makeStandardItems({QStringLiteral("C"), QStringLiteral("D")}));
        sourceModel.item(0, 0)->appendRow(makeStandardItems({QStringLiteral("a0"), QStringLiteral("a1")}));
        sourceModel.item(0, 0)->appendRow(makeStandardItems({QStringLiteral("b0"), QStringLiteral("b1")}));
        sourceModel.item(0, 1)->appendRow(makeStandardItems({QStringLiteral("h0"), QStringLiteral("h1")}));
        sourceModel.item(0, 1)->appendRow(makeStandardItems({QStringLiteral("i0"), QStringLiteral("i1")}));
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        pm.setSourceColumns({1, 0});
        pm.setSourceModel(&sourceModel);
        QSignalSpy rowsInsertedSpy(&pm, &QAbstractItemModel::rowsInserted);
        QSignalSpy rowsRemovedSpy(&pm, &QAbstractItemModel::rowsRemoved);
        QSignalSpy rowsMovedSpy(&pm, &QAbstractItemModel::rowsMoved);
        const QModelIndex shownParent = sourceModel.index(0, 0);
        const QModelIndex hiddenParent = sourceModel.index(0, 1);

        // When moving shown children to hidden ones
        QVERIFY(sourceModel.moveRowToParent(shownParent, 0, hiddenParent, 0));

        // Then the proxy should remove them
        QCOMPARE(rowsRemovedSpy.count(), 1);
        QCOMPARE(rowsRemovedSpy.at(0).at(0).toModelIndex(), pm.index(0, 0));
        QCOMPARE(rowSpyToText(rowsRemovedSpy), QStringLiteral("0,0"));
        QCOMPARE(pm.rowCount(pm.index(0, 0)), 1);

        // When moving hidden children to shown ones
        QVERIFY(sourceModel.moveRowToParent(hiddenParent, 2, sourceModel.index(1, 0), 0));

        // Then the proxy should insert them
        QCOMPARE(rowsInsertedSpy.count(), 1);
        QCOMPARE(rowsInsertedSpy.at(0).at(0).toModelIndex(), pm.index(1, 0));
        QCOMPARE(rowSpyToText(rowsInsertedSpy), QStringLiteral("0,0"));
        QCOMPARE(extractRowTexts(&pm, 0, pm.index(1, 0)), QStringLiteral("i1i0"));

        // When moving hidden children among themselves
        QVERIFY(sourceModel.moveRowToParent(hiddenParent, 0, hiddenParent, 2));

        // Then nothing should be notified
        QCOMPARE(rowsInsertedSpy.count(), 1);
        QCOMPARE(rowsRemovedSpy.count(), 1);
        QCOMPARE(rowsMovedSpy.count(), 0);
    }

    void shouldHandleRowMovesBetweenShownParents()
    {
        // Given a rearrange-columns proxy showing source column 0 in proxy column 1
        RowMovingModel sourceModel;
        sourceModel.appendRow(makeStandardItems({QStringLiteral("A"), QStringLiteral("B")}));
        sourceModel.appendRow(makeStandardItems({QStringLiteral("C"), QStringLiteral("D")}));
        sourceModel.item(0, 0)->appendRow(makeStandardItems({QStringLiteral("a0"), QStringLiteral("a1")}));
        sourceModel.item(1, 0)->appendRow(makeStandardItems({QStringLiteral("c0"), QStringLiteral("c1")}));
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        pm.setSourceColumns({1, 0});
        pm.setSourceModel(&sourceModel);
        QSignalSpy rowsMovedSpy(&pm, &QAbstractItemModel::rowsMoved);

        // When moving a child to another parent
        QVERIFY(sourceModel.moveRowToParent(sourceModel.index(0, 0), 0, sourceModel.index(1, 0), 1));

        // Then the proxy should notify the move between the parents in proxy column 0
        QCOMPARE(rowsMovedSpy.count(), 1);
        QCOMPARE(rowsMovedSpy.at(0).at(0).toModelIndex(), pm.index(0, 0));
        QCOMPARE(rowsMovedSpy.at(0).at(3).toModelIndex(), pm.index(1, 0));
        QCOMPARE(rowsMovedSpy.at(0).at(4).toInt(), 1);
        QCOMPARE(pm.rowCount(pm.index(0, 0)), 0);
        QCOMPARE(extractRowTexts(&pm, 1, pm.index(1, 0)), QStringLiteral("a1a0"));
    }

    void shouldIgnoreLayoutChangesOfHiddenChildren()
    {
        // Given a rearrange-columns proxy, and children under source column 1, which the proxy doesn't show
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        setup(pm);
        mod.item(0, 1)->appendRow(makeStandardItems({QStringLiteral("v")}));
        mod.item(0, 1)->appendRow(makeStandardItems({QStringLiteral("u")}));
        QSignalSpy layoutChangedSpy(&pm, &QAbstractItemModel::layoutChanged);

        // When sorting these children
        mod.item(0, 1)->sortChildren(0);

        // Then nothing should be notified
        QCOMPARE(mod.item(0, 1)->child(0)->text(), QStringLiteral("u"));
        QCOMPARE(layoutChangedSpy.count(), 0);
    }

    void shouldMapDataChangedRanges()
    {
        // Given a rearrange-columns proxy showing source columns 2, 3, 1, 0
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        setup(pm);
        QSignalSpy dataChangedSpy(&pm, &QAbstractItemModel::dataChanged);

        // When source columns 1 and 2 change (proxy columns 2 and 0)
        Q_EMIT mod.dataChanged(mod.index(0, 1), mod.index(1, 2));

        // Then the proxy should notify each range of consecutive proxy columns
        QCOMPARE(dataChangedSpy.count(), 2);
        QCOMPARE(indexToText(dataChangedSpy.at(0).at(0).toModelIndex()), indexToText(pm.index(0, 0)));
        QCOMPARE(indexToText(dataChangedSpy.at(0).at(1).toModelIndex()), indexToText(pm.index(1, 0)));
        QCOMPARE(indexToText(dataChangedSpy.at(1).at(0).toModelIndex()), indexToText(pm.index(0, 2)));
        QCOMPARE(indexToText(dataChangedSpy.at(1).at(1).toModelIndex()), indexToText(pm.index(1, 2)));
        dataChangedSpy.clear();

        // When all the source columns change
        Q_EMIT mod.dataChanged(mod.index(0, 0), mod.index(0, 4));

        // Then the proxy should notify all its columns at once
        QCOMPARE(dataChangedSpy.count(), 1);
        QCOMPARE(indexToText(dataChangedSpy.at(0).at(0).toModelIndex()), indexToText(pm.index(0, 0)));
        QCOMPARE(indexToText(dataChangedSpy.at(0).at(1).toModelIndex()), indexToText(pm.index(0, 3)));
        dataChangedSpy.clear();

        // When only a column that isn't shown changes
        mod.item(0, 4)->setText(QStringLiteral("X"));

        // Then nothing should be notified
        QCOMPARE(dataChangedSpy.count(), 0);
    }

#if QT_VERSION < QT_VERSION_CHECK(7, 0, 0)
    void shouldRespectHandleSourceDataChanges()
    {
        // Given a rearrange-columns proxy that handles data changes itself
        class SelfHandlingProxy : public KRearrangeColumnsProxyModel
        {
        public:
            SelfHandlingProxy()
            {
                setHandleSourceDataChanges(false);
            }
        };
        SelfHandlingProxy pm;
        new QAbstractItemModelTester(&pm, &pm);
        setup(pm);
        QSignalSpy dataChangedSpy(&pm, &QAbstractItemModel::dataChanged);

        // When a cell in the source model changes
        mod.item(0, 2)->setText(QStringLiteral("c"));

        // Then the proxy shouldn't notify it
        QCOMPARE(dataChangedSpy.count(), 0);
    }
#endif

    void shouldEditRowsThroughTheProxy()
    {
        // Given a rearrange-columns proxy
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        setup(pm);

        // When inserting and removing top-level rows through the proxy
        QVERIFY(pm.insertRows(1, 1));
        QCOMPARE(mod.rowCount(), 3);
        QVERIFY(pm.removeRows(1, 1));
        QCOMPARE(mod.rowCount(), 2);
        QCOMPARE(extractRowTexts(&pm, 1), QStringLiteral("GHFE"));

        // When inserting and removing child rows, under proxy column 0 which shows source column 2
        const QModelIndex proxyParent = pm.index(0, 0);
        QVERIFY(pm.insertRows(0, 1, proxyParent));
        QCOMPARE(mod.rowCount(mod.index(0, 0)), 3);
        QCOMPARE(pm.rowCount(proxyParent), 3);
        QVERIFY(pm.removeRows(0, 1, proxyParent));
        QCOMPARE(mod.rowCount(mod.index(0, 0)), 2);
        QCOMPARE(extractRowTexts(&pm, 0, proxyParent), QStringLiteral("opnm"));

        // When using a parent in proxy column 1, which has no children
        QVERIFY(!pm.insertRows(0, 1, pm.index(0, 1)));
        QVERIFY(!pm.removeRows(0, 1, pm.index(0, 1)));
        QCOMPARE(mod.rowCount(mod.index(0, 0)), 2);

        // When moving rows through the proxy
        QStringListModel listModel({QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")});
        KRearrangeColumnsProxyModel listProxy;
        listProxy.setSourceColumns({0});
        listProxy.setSourceModel(&listModel);
        QVERIFY(listProxy.moveRows(QModelIndex(), 0, 1, QModelIndex(), 3));
        QCOMPARE(listModel.stringList(), QStringList({QStringLiteral("b"), QStringLiteral("c"), QStringLiteral("a")}));
    }

    void shouldDropUnderTheRightParent()
    {
        // Given a rearrange-columns proxy showing source columns 2, 3, 1, 0
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        setup(pm);
        const QModelIndex proxyParent = pm.index(0, 0);
        std::unique_ptr<QMimeData> rowData(mod.mimeData({mod.index(1, 0), mod.index(1, 1)}));
        std::unique_ptr<QMimeData> cellData(mod.mimeData({mod.index(1, 2)}));

        // When dropping a row under proxy item (0, 0), which shows source column 2
        QVERIFY(pm.canDropMimeData(rowData.get(), Qt::CopyAction, 2, -1, proxyParent));
        QVERIFY(pm.dropMimeData(rowData.get(), Qt::CopyAction, 2, -1, proxyParent));

        // Then the row should be a child of source column 0, which the proxy shows
        QCOMPARE(mod.rowCount(mod.index(0, 0)), 3);
        QCOMPARE(pm.rowCount(proxyParent), 3);

        // When dropping a cell in proxy column 1, which shows source column 3
        QVERIFY(pm.dropMimeData(cellData.get(), Qt::CopyAction, 0, 1, proxyParent));

        // Then the cell should be in source column 3
        QCOMPARE(pm.rowCount(proxyParent), 4);
        QCOMPARE(mod.index(0, 3, mod.index(0, 0)).data().toString(), QStringLiteral("G"));
        QCOMPARE(pm.index(0, 1, proxyParent).data().toString(), QStringLiteral("G"));
    }

    void shouldDropOntoTheRightItem()
    {
        // Given a rearrange-columns proxy showing source column 1 in proxy column 0, on a table model
        TableModel sourceModel({{QStringLiteral("a0"), QStringLiteral("a1")}, {QStringLiteral("b0"), QStringLiteral("b1")}});
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        pm.setSourceColumns({1, 0});
        pm.setSourceModel(&sourceModel);
        std::unique_ptr<QMimeData> cellData(sourceModel.mimeData({sourceModel.index(0, 0)}));

        // When dropping a cell onto proxy item (1, 0)
        QVERIFY(pm.dropMimeData(cellData.get(), Qt::CopyAction, -1, -1, pm.index(1, 0)));

        // Then the source cell shown there should be replaced
        QCOMPARE(sourceModel.index(1, 1).data().toString(), QStringLiteral("a0"));
        QCOMPARE(sourceModel.index(1, 0).data().toString(), QStringLiteral("b0"));
    }

    void shouldMapHeaderDataChanged()
    {
        // Given a rearrange-columns proxy showing source columns 2, 3, 1, 0
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        setup(pm);
        QSignalSpy headerDataChangedSpy(&pm, &QAbstractItemModel::headerDataChanged);

        // When a shown and a hidden source column header change
        mod.setHeaderData(3, Qt::Horizontal, QStringLiteral("X"));
        mod.setHeaderData(4, Qt::Horizontal, QStringLiteral("Y"));

        // Then only the shown one should be notified, for its proxy column
        QCOMPARE(headerDataChangedSpy.count(), 1);
        QCOMPARE(headerDataChangedSpy.at(0).at(1).toInt(), 1);
        QCOMPARE(headerDataChangedSpy.at(0).at(2).toInt(), 1);
        QCOMPARE(extractHorizontalHeaderTexts(&pm), QStringLiteral("H3XH2H1"));
    }

    void shouldUpdatePersistentIndexesOnLayoutChange()
    {
        // Given a rearrange-columns proxy and a persistent index on "H"
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        setup(pm);
        const QPersistentModelIndex persistent = pm.index(1, 1);
        QCOMPARE(persistent.data().toString(), QStringLiteral("H"));
        QSignalSpy layoutChangedSpy(&pm, &QAbstractItemModel::layoutChanged);

        // When sorting the source model in reverse order
        mod.sort(0, Qt::DescendingOrder);

        // Then the persistent index should follow "H"
        QCOMPARE(layoutChangedSpy.count(), 1);
        QCOMPARE(extractRowTexts(&pm, 0), QStringLiteral("GHFE"));
        QCOMPARE(persistent.row(), 0);
        QCOMPARE(persistent.column(), 1);
        QCOMPARE(persistent.data().toString(), QStringLiteral("H"));
    }

    void shouldHandleSourceColumnChanges_data()
    {
        QTest::addColumn<QString>("change");
        QTest::addColumn<int>("column");
        QTest::addColumn<bool>("topLevel");
        QTest::addColumn<bool>("expectReset");

        // The proxy shows source columns 2, 3, 1, 0
        for (bool topLevel : {true, false}) {
            const char *table = topLevel ? "" : "-in-child-table";
            QTest::addRow("insert%s", table) << QStringLiteral("insert") << 0 << topLevel << true;
            QTest::addRow("append%s", table) << QStringLiteral("insert") << 5 << topLevel << false;
            QTest::addRow("remove%s", table) << QStringLiteral("remove") << 1 << topLevel << true;
            QTest::addRow("remove-last%s", table) << QStringLiteral("remove") << 4 << topLevel << false;
            QTest::addRow("move%s", table) << QStringLiteral("move") << 0 << topLevel << true;
            QTest::addRow("move-last-to-front%s", table) << QStringLiteral("move-to-front") << 4 << topLevel << true;
        }
    }

    void shouldHandleSourceColumnChanges()
    {
        QFETCH(QString, change);
        QFETCH(int, column);
        QFETCH(bool, topLevel);
        QFETCH(bool, expectReset);

        // Given a rearrange-columns proxy, and a persistent index on a grandchild
        ColumnMovingModel sourceModel(2, 5);
        sourceModel.setItem(0, 0, new QStandardItem(QStringLiteral("A")));
        sourceModel.item(0, 0)->appendRow(
            makeStandardItems({QStringLiteral("c0"), QStringLiteral("c1"), QStringLiteral("c2"), QStringLiteral("c3"), QStringLiteral("c4")}));
        sourceModel.item(0, 0)->child(0, 0)->appendRow(
            makeStandardItems({QStringLiteral("g0"), QStringLiteral("g1"), QStringLiteral("g2"), QStringLiteral("g3"), QStringLiteral("g4")}));
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        pm.setSourceColumns({2, 3, 1, 0});
        pm.setSourceModel(&sourceModel);
        const QPersistentModelIndex persistentGrandChild = pm.index(0, 0, pm.index(0, 0, pm.index(0, 0)));
        QCOMPARE(persistentGrandChild.data().toString(), QStringLiteral("g2"));
        QSignalSpy modelResetSpy(&pm, &QAbstractItemModel::modelReset);
        QSignalSpy columnsInsertedSpy(&pm, &QAbstractItemModel::columnsInserted);
        QSignalSpy columnsRemovedSpy(&pm, &QAbstractItemModel::columnsRemoved);
        QSignalSpy columnsMovedSpy(&pm, &QAbstractItemModel::columnsMoved);

        // When the source columns change
        const QModelIndex parent = topLevel ? QModelIndex() : sourceModel.index(0, 0);
        if (change == QLatin1String("insert")) {
            sourceModel.insertColumn(column, parent);
        } else if (change == QLatin1String("remove")) {
            sourceModel.removeColumn(column, parent);
        } else {
            QVERIFY(sourceModel.moveColumnsWithoutData(parent, column, column, change == QLatin1String("move") ? 3 : 0));
        }

        // Then the proxy should reset, since it shows source column numbers and the children come
        // from source column 0, unless the change is after the shown columns
        QCOMPARE(modelResetSpy.count(), expectReset ? 1 : 0);
        QCOMPARE(persistentGrandChild.isValid(), !expectReset);
        QCOMPARE(columnsInsertedSpy.count(), 0);
        QCOMPARE(columnsRemovedSpy.count(), 0);
        QCOMPARE(columnsMovedSpy.count(), 0);
    }

    void shouldResetOnColumnMovesBetweenTables_data()
    {
        QTest::addColumn<bool>("fromTopLevel");
        QTest::addColumn<int>("first");
        QTest::addColumn<int>("destination");
        QTest::addColumn<bool>("expectReset");

        // The proxy shows source columns 2, 3, 1, 0
        QTest::newRow("from-top-level-shown") << true << 1 << 5 << true;
        QTest::newRow("to-top-level-shown") << false << 4 << 1 << true;
        QTest::newRow("to-child-table-shown") << true << 4 << 1 << true;
        QTest::newRow("from-child-table-shown") << false << 1 << 5 << true;
        QTest::newRow("unshown") << true << 4 << 5 << false;
    }

    void shouldResetOnColumnMovesBetweenTables()
    {
        QFETCH(bool, fromTopLevel);
        QFETCH(int, first);
        QFETCH(int, destination);
        QFETCH(bool, expectReset);

        // Given a rearrange-columns proxy
        ColumnMovingModel sourceModel(2, 5);
        sourceModel.setItem(0, 0, new QStandardItem(QStringLiteral("A")));
        sourceModel.item(0, 0)->appendRow(makeStandardItems({QStringLiteral("c0"), QStringLiteral("c1"), QStringLiteral("c2"), QStringLiteral("c3")}));
        KRearrangeColumnsProxyModel pm;
        new QAbstractItemModelTester(&pm, &pm);
        pm.setSourceColumns({2, 3, 1, 0});
        pm.setSourceModel(&sourceModel);
        QSignalSpy modelResetSpy(&pm, &QAbstractItemModel::modelReset);

        // When moving a column between the top level and a child table
        const QModelIndex childTable = sourceModel.index(0, 0);
        QVERIFY(sourceModel
                    .moveColumnsWithoutData(fromTopLevel ? QModelIndex() : childTable, first, first, destination, fromTopLevel ? childTable : QModelIndex()));

        // Then the proxy should reset if a shown column of either table changed
        QCOMPARE(modelResetSpy.count(), expectReset ? 1 : 0);
    }

private:
    // Moves columns without moving the data, to test the column mapping
    class ColumnMovingModel : public QStandardItemModel
    {
    public:
        using QStandardItemModel::QStandardItemModel;
        bool moveColumnsWithoutData(const QModelIndex &parent, int first, int last, int destination, const QModelIndex &destParent)
        {
            if (!beginMoveColumns(parent, first, last, destParent, destination)) {
                return false;
            }
            endMoveColumns();
            return true;
        }

        bool moveColumnsWithoutData(const QModelIndex &parent, int first, int last, int destination)
        {
            return moveColumnsWithoutData(parent, first, last, destination, parent);
        }
    };

    class TableModel : public QAbstractTableModel
    {
    public:
        explicit TableModel(const QList<QStringList> &cells)
            : m_cells(cells)
        {
        }
        int rowCount(const QModelIndex &parent = {}) const override
        {
            return parent.isValid() ? 0 : m_cells.size();
        }
        int columnCount(const QModelIndex &parent = {}) const override
        {
            return parent.isValid() ? 0 : m_cells.first().size();
        }
        QVariant data(const QModelIndex &index, int role) const override
        {
            return role == Qt::DisplayRole || role == Qt::EditRole ? QVariant(m_cells.at(index.row()).at(index.column())) : QVariant();
        }
        bool setData(const QModelIndex &index, const QVariant &value, int role) override
        {
            if (role != Qt::DisplayRole && role != Qt::EditRole) {
                return false;
            }
            m_cells[index.row()][index.column()] = value.toString();
            Q_EMIT dataChanged(index, index, {role});
            return true;
        }

    private:
        QList<QStringList> m_cells;
    };

    // Moves rows to another parent, which QStandardItemModel::moveRows doesn't support
    class RowMovingModel : public QStandardItemModel
    {
    public:
        bool moveRowToParent(const QModelIndex &sourceParent, int row, const QModelIndex &destParent, int destRow)
        {
            if (!beginMoveRows(sourceParent, row, row, destParent, destRow)) {
                return false;
            }
            {
                // Only the move should be notified
                const QSignalBlocker blocker(this);
                QStandardItem *destItem = itemFromIndex(destParent);
                if (destParent == sourceParent && destRow > row) {
                    --destRow;
                }
                destItem->insertRow(destRow, itemFromIndex(sourceParent)->takeRow(row));
            }
            endMoveRows();
            return true;
        }
    };

    // setup proxy
    void setup(KRearrangeColumnsProxyModel &pm)
    {
        pm.setSourceColumns(QList<int>() << 2 << 3 << 1 << 0);
        pm.setSourceModel(&mod);
        pm.sort(0); // don't forget this!
    }

    static QString indexRowCol(const QModelIndex &index)
    {
        if (!index.isValid()) {
            return QStringLiteral("invalid");
        }
        return QString::number(index.row()) + "," + QString::number(index.column());
    }

    static QString indexToText(const QModelIndex &index)
    {
        if (!index.isValid()) {
            return QStringLiteral("invalid");
        }
        return QString::number(index.row()) + "," + QString::number(index.column()) + ","
            + QString::number(reinterpret_cast<qulonglong>(index.internalPointer()), 16) + " in "
            + QString::number(reinterpret_cast<qulonglong>(index.model()), 16);
    }

    QStandardItemModel mod;
};

QTEST_MAIN(tst_KRearrangeColumnsProxyModel)

#include "krearrangecolumnsproxymodeltest.moc"
