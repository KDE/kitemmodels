/*
    SPDX-FileCopyrightText: 2025 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QIdentityProxyModel>
#include <QItemSelection>
#include <QSignalSpy>
#include <QStringListModel>
#include <QTest>

#include "ksortfilterproxyroleoptimizedlistmodel.h"

struct Sensor {
    int id;
    QString name;
    int value;
};

class MySensorsModel : public QAbstractListModel
{
public:
    int rowCount(const QModelIndex & = QModelIndex()) const override
    {
        return list.count();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        const Sensor &sensor = list[index.row()];

        switch (role) {
        case IdRole:
            return sensor.id;
        case NameRole:
            return sensor.name;
        case ValueRole:
            return sensor.value;
        }

        return {};
    }

    void updateAllSensorValues()
    {
        if (list.isEmpty()) {
            return;
        }

        for (Sensor &sensor : list) {
            sensor.value++;
        }

        Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, 0), {ValueRole});
    }

    void updateSomeSensorValues(const QList<int> &rows)
    {
        for (int row : rows) {
            list[row].value++;
            Q_EMIT dataChanged(index(row, 0), index(row, 0), {ValueRole});
        }
    }

    void renameSensor(int row, const QString &name)
    {
        list[row].name = name;
        Q_EMIT dataChanged(index(row, 0), index(row, 0), {NameRole});
    }

    QList<Sensor> list;

    enum Roles {
        IdRole,
        NameRole,
        ValueRole
    };
};

class NormalFilter : public QSortFilterProxyModel
{
public:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        ++nTimesCalled;
        return sourceModel()->index(source_row, 0, source_parent).data(MySensorsModel::NameRole).toString().contains("l");
    }

    mutable int nTimesCalled = 0;
};

class BetterFilter : public KSortFilterProxyRoleOptimizedListModel
{
public:
    BetterFilter()
    {
        setInterestingRoles({MySensorsModel::NameRole});
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        ++nTimesCalled;
        return sourceModel()->index(source_row, 0, source_parent).data(MySensorsModel::NameRole).toString().contains("l");
    }

    mutable int nTimesCalled = 0;
};

class KSortFilterProxyRoleOptimizedListModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testOnlyFilterWhenNeeded();
};

void compareDataChangedParametersAndClearIfEqual(QSignalSpy *spyA, QSignalSpy *spyB)
{
    QCOMPARE(spyA->count(), spyB->count());
    for (int i = 0; i < spyA->count(); ++i) {
        const QVariantList dataChangedParametersA = spyA->at(i);
        const QVariantList dataChangedParametersB = spyB->at(i);
        // dataChanged parameters are QModelIndex topLeft, QModelIndex bottomRight, QList<int> roles
        // the model indexes we can't compare directly because they belong to different models
        QCOMPARE(dataChangedParametersA[0].toModelIndex().row(), dataChangedParametersB[0].toModelIndex().row());
        QCOMPARE(dataChangedParametersA[0].toModelIndex().column(), dataChangedParametersB[0].toModelIndex().column());
        QCOMPARE(dataChangedParametersA[1].toModelIndex().row(), dataChangedParametersB[1].toModelIndex().row());
        QCOMPARE(dataChangedParametersA[1].toModelIndex().column(), dataChangedParametersB[1].toModelIndex().column());
        QCOMPARE(dataChangedParametersA[2], dataChangedParametersB[2]);
    }
    spyA->clear();
    spyB->clear();
}

void KSortFilterProxyRoleOptimizedListModelTest::testOnlyFilterWhenNeeded()
{
    MySensorsModel baseModel;
    NormalFilter normalFilter;
    BetterFilter betterFilter;

    baseModel.list << Sensor{0, "Hello", 2};
    baseModel.list << Sensor{1, "there", 1};
    baseModel.list << Sensor{2, "are", -3};
    baseModel.list << Sensor{3, "you", 55};
    baseModel.list << Sensor{4, "melon", 525};
    baseModel.list << Sensor{5, "pineapple", 125};
    baseModel.list << Sensor{6, "bananan", 25};

    normalFilter.setSourceModel(&baseModel);
    betterFilter.setModel(&baseModel);

    QSignalSpy normalFilterDataChangedSpy(&normalFilter, &QAbstractItemModel::dataChanged);
    QSignalSpy betterFilterDataChangedSpy(&betterFilter, &QAbstractItemModel::dataChanged);
    QSignalSpy normalFilterRowsAboutToBeRemovedSpy(&normalFilter, &QAbstractItemModel::rowsAboutToBeRemoved);
    QSignalSpy betterFilterRowsAboutToBeRemovedSpy(&betterFilter, &QAbstractItemModel::rowsAboutToBeRemoved);

    // Calling row count must filter all the rows
    QCOMPARE(normalFilter.rowCount(), 3);
    QCOMPARE(betterFilter.rowCount(), 3);
    QCOMPARE(normalFilter.nTimesCalled, baseModel.rowCount());
    QCOMPARE(betterFilter.nTimesCalled, baseModel.rowCount());

    // Emitting changes for a role that we're not interested doesn't call filterAcceptsRow
    // so betterFilter.nTimesCalled has not increased
    baseModel.updateAllSensorValues();
    QCOMPARE(normalFilter.nTimesCalled, baseModel.rowCount() * 2);
    QCOMPARE(betterFilter.nTimesCalled, baseModel.rowCount());
    QCOMPARE(normalFilterDataChangedSpy.count(), 1);
    compareDataChangedParametersAndClearIfEqual(&normalFilterDataChangedSpy, &betterFilterDataChangedSpy);

    // Emitting changes for a role that we're not interested doesn't call filterAcceptsRow
    // so betterFilter.nTimesCalled has not increased
    baseModel.updateSomeSensorValues({0, 4});
    QCOMPARE(normalFilter.nTimesCalled, baseModel.rowCount() * 2 + 2);
    QCOMPARE(betterFilter.nTimesCalled, baseModel.rowCount());
    QCOMPARE(normalFilterDataChangedSpy.count(), 2);
    compareDataChangedParametersAndClearIfEqual(&normalFilterDataChangedSpy, &betterFilterDataChangedSpy);

    // Emitting changes for a role that we're interested calls filterAcceptsRow
    // so betterFilter.nTimesCalled gets increased by one
    baseModel.renameSensor(0, "Bye");
    QCOMPARE(normalFilter.nTimesCalled, baseModel.rowCount() * 2 + 2 + 1);
    QCOMPARE(betterFilter.nTimesCalled, baseModel.rowCount() + 1);
    QCOMPARE(normalFilterRowsAboutToBeRemovedSpy.count(), 1);
    // We can compare the lists directly because the QModelIndex is of the parent and thus same for both models
    QCOMPARE(betterFilterRowsAboutToBeRemovedSpy, normalFilterRowsAboutToBeRemovedSpy);

    // Calling rowCount doesn't really call filterAcceptsRow in this scenario
    QCOMPARE(normalFilter.rowCount(), 2);
    QCOMPARE(betterFilter.rowCount(), 2);
    QCOMPARE(normalFilter.nTimesCalled, baseModel.rowCount() * 2 + 2 + 1);
    QCOMPARE(betterFilter.nTimesCalled, baseModel.rowCount() + 1);
    QCOMPARE(normalFilterDataChangedSpy.count(), 0);
}

QTEST_MAIN(KSortFilterProxyRoleOptimizedListModelTest)
#include "ksortfilterproxyroleoptimizedlistmodeltest.moc"
