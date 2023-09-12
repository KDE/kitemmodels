/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QObject>
#include <QString>

#include <QSignalSpy>
#include <QTest>

#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlPropertyMap>

#include <QAbstractItemModel>
#include <QStandardItemModel>

#include <memory>
#include <qtestcase.h>

using namespace Qt::StringLiterals;

class KModelIndexObserverTest : public QObject
{
    Q_OBJECT
    enum Roles {
        CounterRole = Qt::UserRole,
        TemperatureRole,
    };
private Q_SLOTS:
    void testDefaults();
    void testBasicProperties();
    void testCreate();

private:
    static std::unique_ptr<QStandardItemModel> createSomeSourceModel();
};

void KModelIndexObserverTest::testDefaults()
{
    QQmlApplicationEngine app;
    QQmlComponent component(&app);
    component.loadFromModule(u"org.kde.kitemmodels"_s, u"KModelIndexObserver"_s);
    std::unique_ptr<QObject> observer(component.create());

    const auto sourceModel = observer->property("sourceModel").value<QObject *>();
    QCOMPARE_EQ(sourceModel, nullptr);

    const auto row = observer->property("row").toInt();
    QCOMPARE_EQ(row, -1);

    const auto column = observer->property("column").toInt();
    QCOMPARE_EQ(column, 0);

    const auto parentIndex = observer->property("parentIndex").toModelIndex();
    QCOMPARE_EQ(parentIndex, QModelIndex());

    const auto index = observer->property("index").toModelIndex();
    QCOMPARE_EQ(index, QModelIndex());

    const auto roles = observer->property("roles").value<QList<int>>();
    QVERIFY(roles.isEmpty());

    const auto roleNames = observer->property("roleNames").value<QList<QByteArray>>();
    QVERIFY(roleNames.isEmpty());

    const auto modelData = observer->property("modelData").value<QQmlPropertyMap *>();
    QCOMPARE_EQ(modelData, nullptr);
}

void KModelIndexObserverTest::testBasicProperties()
{
    QQmlApplicationEngine app;
    QQmlComponent component(&app);
    component.loadFromModule(u"org.kde.kitemmodels"_s, u"KModelIndexObserver"_s);
    std::unique_ptr<QObject> observer(component.create());
    auto model = createSomeSourceModel();

    const auto index = observer->property("index").toModelIndex();
    QVERIFY(!index.isValid());

    // Writing a row or a column without a model should still preserve them
    const auto parentIndex = model->index(7, 0);
    QVERIFY(observer->setProperty("row", 42));
    QVERIFY(observer->setProperty("column", 3));
    QVERIFY(observer->setProperty("parentIndex", parentIndex));
    QCOMPARE(observer->property("row").toInt(), 42);
    QCOMPARE(observer->property("column").toInt(), 3);
    QCOMPARE(observer->property("parentIndex").toModelIndex(), parentIndex);
    QCOMPARE(observer->property("index").toModelIndex(), QModelIndex());

    const QStringList roleNames = {u"display"_s, u"temperature"_s};
    QVERIFY(observer->setProperty("roleNames", roleNames));
    QCOMPARE(observer->property("roleNames").toStringList(), roleNames);
    QCOMPARE(observer->property("roles").value<QList<int>>(), QList<int>({-1, -1}));

    QVERIFY(observer->setProperty("sourceModel", QVariant::fromValue(&*model)));
    QCOMPARE(observer->property("sourceModel").value<QObject *>(), &*model);
    QCOMPARE(observer->property("roleNames").toStringList(), roleNames);
    QCOMPARE(observer->property("roles").value<QList<int>>(), QList<int>({Qt::DisplayRole, TemperatureRole}));
}

void KModelIndexObserverTest::testCreate()
{
    auto model = createSomeSourceModel();
}

std::unique_ptr<QStandardItemModel> KModelIndexObserverTest::createSomeSourceModel()
{
    auto testModel = std::make_unique<QStandardItemModel>();
    for (int i = 1; i <= 12; i++) {
        auto entry = new QStandardItem();
        entry->setData(QLocale::c().monthName(i), Qt::DisplayRole);
        entry->setData(i, CounterRole);
        entry->setData(i * 2, TemperatureRole);
        testModel->appendRow(entry);
    }
    testModel->setItemRoleNames({
        {Qt::DisplayRole, "display"},
        {CounterRole, "counter"},
        {TemperatureRole, "temperature"},

    });
    return testModel;
}

QTEST_GUILESS_MAIN(KModelIndexObserverTest)

#include "kmodelindexobservertest_qml.moc"
