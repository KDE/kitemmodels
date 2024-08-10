/*
 *   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QAbstractItemModel>
#include <QJsonObject>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTest>

#ifdef IMPORT_ITEMMODELSPLUGIN
#include <QPluginLoader>
Q_IMPORT_PLUGIN(org_kde_kitemmodelsPlugin)
#endif

using namespace Qt::StringLiterals;

class tst_IterQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testRepeater();
    void testInstantiator();
    void testModelIndices();
    void testModelData();

private:
    QAbstractItemModel *createTestListModel(QQmlEngine *engine);
    QAbstractItemModel *createTestItemModel(QObject *parent);

    QModelIndexList toIndexList(const QVariant &list);
    QList<int> toIntList(const QVariant &list);
};

void tst_IterQml::testRepeater()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        import QtQuick
        import org.kde.kitemmodels as KItemModels

        // Repeater needs to be wrapped in some Item, otherwise it won't create any delegates
        Item {
            id: root

            property int spy

            Repeater {
                id: repeater
                model: 3
                delegate: Item {}
            }

            function test_count() {
                spy = 0;
                for (const item of KItemModels.Iter.repeater(repeater)) {
                    spy += 1;
                }
            }

            function test_null() {
                spy = 0;
                for (const item of KItemModels.Iter.repeater(null)) {
                    spy += 1;
                }
            }

            function test_change() {
                spy = 0;
                repeater.model = 3;
                for (const item of KItemModels.Iter.repeater(repeater)) {
                    spy += 1;
                    repeater.model = null;
                }
                repeater.model = 3;
            }

            function test_array() {
                return Array.from(KItemModels.Iter.repeater(repeater));
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto repeater = app.rootObjects().first();

    QMetaObject::invokeMethod(repeater, "test_count", Qt::DirectConnection);
    QCOMPARE(repeater->property("spy").toInt(), 3);

    QMetaObject::invokeMethod(repeater, "test_null", Qt::DirectConnection);
    QCOMPARE(repeater->property("spy").toInt(), 0);

    QMetaObject::invokeMethod(repeater, "test_change", Qt::DirectConnection);
    QCOMPARE(repeater->property("spy").toInt(), 1);

    QVariant arrayVariant;
    QMetaObject::invokeMethod(repeater, "test_array", Qt::DirectConnection, qReturnArg(arrayVariant));
    QVariantList array = arrayVariant.toList();
    QCOMPARE(array.length(), 3);
    for (const auto &variant : array) {
        QVERIFY(variant.value<QObject *>());
    }
}

void tst_IterQml::testInstantiator()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        import QtQml
        import QtQml.Models
        import org.kde.kitemmodels as KItemModels

        Instantiator {
            id: root

            property int spy

            model: 3
            delegate: QtObject {}

            function test_count() {
                spy = 0;
                for (const item of KItemModels.Iter.instantiator(this)) {
                    spy += 1;
                }
            }

            function test_null() {
                spy = 0;
                for (const item of KItemModels.Iter.instantiator(null)) {
                    spy += 1;
                }
            }

            function test_change() {
                spy = 0;
                model = 3;
                for (const item of KItemModels.Iter.instantiator(this)) {
                    spy += 1;
                    model = null;
                }
                model = 3;
            }

            function test_array() {
                return Array.from(KItemModels.Iter.instantiator(this));
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto instantiator = app.rootObjects().first();

    QMetaObject::invokeMethod(instantiator, "test_count", Qt::DirectConnection);
    QCOMPARE(instantiator->property("spy").toInt(), 3);

    QMetaObject::invokeMethod(instantiator, "test_null", Qt::DirectConnection);
    QCOMPARE(instantiator->property("spy").toInt(), 0);

    QMetaObject::invokeMethod(instantiator, "test_change", Qt::DirectConnection);
    QCOMPARE(instantiator->property("spy").toInt(), 1);

    QVariant arrayVariant;
    QMetaObject::invokeMethod(instantiator, "test_array", Qt::DirectConnection, qReturnArg(arrayVariant));
    QVariantList array = arrayVariant.toList();
    QCOMPARE(array.length(), 3);
    for (const auto &variant : array) {
        QVERIFY(variant.value<QObject *>());
    }
}

void tst_IterQml::testModelIndices()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        import QtQml
        import org.kde.kitemmodels as KItemModels

        QtObject {
            property var model
            property int spy

            function test_count() {
                spy = 0;
                for (const index of KItemModels.Iter.modelIndices(model)) {
                    spy += 1;
                }
            }

            function test_array() {
                return Array.from(KItemModels.Iter.modelIndices(model));
            }

            function test_array_with(column: int, parentIndex: /*QModelIndex*/ var): /*list<QModelIndex>*/ var {
                return Array.from(KItemModels.Iter.modelIndices(model, column, parentIndex));
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto tester = app.rootObjects().first();

    {
        const auto model = createTestListModel(&app);
        tester->setProperty("model", QVariant::fromValue(model));

        QMetaObject::invokeMethod(tester, "test_count", Qt::DirectConnection);
        QCOMPARE(tester->property("spy").toInt(), 3);

        QVariant returned;
        QMetaObject::invokeMethod(tester, "test_array", Qt::DirectConnection, qReturnArg(returned));
        const auto indices = toIndexList(returned);
        QCOMPARE(indices.length(), 3);

        QCOMPARE(indices[0].row(), 0);
        QCOMPARE(indices[0].column(), 0);
        QCOMPARE(indices[0].model(), model);
        QVERIFY(!indices[0].parent().isValid());

        QCOMPARE(indices[1].row(), 1);
        QCOMPARE(indices[1].column(), 0);
        QCOMPARE(indices[1].model(), model);
        QVERIFY(!indices[1].parent().isValid());

        QCOMPARE(indices[2].row(), 2);
        QCOMPARE(indices[2].column(), 0);
        QCOMPARE(indices[2].model(), model);
        QVERIFY(!indices[2].parent().isValid());
    }
    {
        const auto model = createTestItemModel(&app);
        tester->setProperty("model", QVariant::fromValue(model));

        QMetaObject::invokeMethod(tester, "test_count", Qt::DirectConnection);
        QCOMPARE(tester->property("spy").toInt(), 4);

        {
            QVariant returned;
            QMetaObject::invokeMethod(tester, "test_array", Qt::DirectConnection, qReturnArg(returned));
            const auto indices = toIndexList(returned);
            QCOMPARE(indices.length(), 4);
            QCOMPARE(indices[1].data(), u"February"_s);
        }
        {
            const auto parentIndex = model->index(2, 0);
            QCOMPARE(parentIndex.data(), u"March"_s);
            QVariant returned;
            QMetaObject::invokeMethod(tester, "test_array_with", Qt::DirectConnection, qReturnArg(returned), 0, QVariant(parentIndex));
            const auto indices = toIndexList(returned);
            QCOMPARE(indices.length(), 3);
            QCOMPARE(indices[1].data(), u"Tuesday"_s);
        }
        {
            // Second column has no data under root index
            QVariant returned;
            QMetaObject::invokeMethod(tester, "test_array_with", Qt::DirectConnection, qReturnArg(returned), 1, QVariant());
            QVERIFY(returned.toList().isEmpty());
        }
        {
            // Second column has data under April's index
            const auto parentIndex = model->index(3, 0);
            QCOMPARE(parentIndex.data(), u"April"_s);
            QVariant returned;
            QMetaObject::invokeMethod(tester, "test_array_with", Qt::DirectConnection, qReturnArg(returned), 1, QVariant(parentIndex));
            const auto indices = toIndexList(returned);
            QCOMPARE(indices.length(), 2);
            QCOMPARE(indices[1].data(), u"Oranges"_s);
        }
    }
}

void tst_IterQml::testModelData()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        import QtQml
        import org.kde.kitemmodels as KItemModels

        QtObject {
            property var model
            property int spy

            // function getRole

            function test_count() {
                spy = 0;
                for (const index of KItemModels.Iter.modelData(model)) {
                    spy += 1;
                }
            }

            function test_array() {
                return Array.from(KItemModels.Iter.modelData(model));
            }

            function test_array_with(role: int, column: int, parentIndex: /*QModelIndex*/ var): /*list<var>*/ var {
                return Array.from(KItemModels.Iter.modelData(model, role, column, parentIndex));
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto tester = app.rootObjects().first();

    {
        const auto model = createTestListModel(&app);
        tester->setProperty("model", QVariant::fromValue(model));

        QMetaObject::invokeMethod(tester, "test_count", Qt::DirectConnection);
        QCOMPARE(tester->property("spy").toInt(), 3);

        // ListModel does some tricks with its model roles, so we can't rely on standard roles
        const int displayRole = model->roleNames().key(QByteArrayLiteral("display"));

        {
            QVariant returned;
            QMetaObject::invokeMethod(tester, "test_array_with", Qt::DirectConnection, qReturnArg(returned), displayRole, 0, QVariant(QModelIndex()));
            const auto data = returned.toStringList();
            const QList expected{u"one"_s, u"two"_s, u"three"_s};
            QCOMPARE(data, expected);
        }
        {
            // No data for other roles
            const auto otherRole = displayRole + 1;
            QVariant returned;
            QMetaObject::invokeMethod(tester, "test_array_with", Qt::DirectConnection, qReturnArg(returned), otherRole, 0, QVariant(QModelIndex()));
            const auto data = returned.toList();
            for (const auto &variant : data) {
                QVERIFY(!variant.isValid());
            }
        }
        {
            // No data for non-zero column
            QVariant returned;
            QMetaObject::invokeMethod(tester, "test_array_with", Qt::DirectConnection, qReturnArg(returned), displayRole, 1, QVariant(QModelIndex()));
            const auto data = returned.toList();
            for (const auto &variant : data) {
                QVERIFY(!variant.isValid());
            }
        }
        {
            // No data for non-root index
            QVariant returned;
            auto parentIndex = model->index(1, 0);
            QMetaObject::invokeMethod(tester, "test_array_with", Qt::DirectConnection, qReturnArg(returned), displayRole, 0, QVariant(parentIndex));
            const auto data = returned.toList();
            for (const auto &variant : data) {
                QVERIFY(!variant.isValid());
            }
        }
    }
    {
        const auto model = createTestItemModel(&app);
        tester->setProperty("model", QVariant::fromValue(model));

        QMetaObject::invokeMethod(tester, "test_count", Qt::DirectConnection);
        QCOMPARE(tester->property("spy").toInt(), 4);

        {
            QVariant returned;
            QMetaObject::invokeMethod(tester, "test_array", Qt::DirectConnection, qReturnArg(returned));
            const auto data = returned.toStringList();
            const QList expected{u"January"_s, u"February"_s, u"March"_s, u"April"_s};
            QCOMPARE(data, expected);
        }
        {
            // Data for UserRole
            QVariant returned;
            QMetaObject::invokeMethod(tester,
                                      "test_array_with",
                                      Qt::DirectConnection,
                                      qReturnArg(returned),
                                      static_cast<int>(Qt::UserRole),
                                      0,
                                      QVariant(QModelIndex()));
            const auto data = toIntList(returned);
            const QList<int> expected{1, 2, 3, 4};
            QCOMPARE(data, expected);
        }
        {
            // Second column has data under April's index
            const auto parentIndex = model->index(3, 0);
            QCOMPARE(parentIndex.data(), u"April"_s);
            QVariant returned;
            QMetaObject::invokeMethod(tester,
                                      "test_array_with",
                                      Qt::DirectConnection,
                                      qReturnArg(returned),
                                      static_cast<int>(Qt::DisplayRole),
                                      1,
                                      QVariant(parentIndex));
            const auto data = returned.toStringList();
            const QList expected{u"Apples"_s, u"Oranges"_s};
            QCOMPARE(data, expected);
        }
    }
}

QAbstractItemModel *tst_IterQml::createTestListModel(QQmlEngine *engine)
{
    QQmlComponent component(engine);
    component.setData(R"(
        import QtQml.Models

        ListModel {
            ListElement { display: "one" }
            ListElement { display: "two" }
            ListElement { display: "three" }
        }
    )",
                      QUrl());
    const auto object = component.create();
    const auto model = qobject_cast<QAbstractItemModel *>(object);
    return model;
}

QAbstractItemModel *tst_IterQml::createTestItemModel(QObject *parent)
{
    // - [0] January
    // - [1] February
    // - [2] March
    //   + Monday
    //   + Tuesday
    //   + Wednesday
    // - [3] April
    //   + Saturday | Apples
    //   + Sunday   | Oranges
    auto model = new QStandardItemModel(parent);
    const auto makeEntry = [](QStandardItem *parent, int row, int column, const QString &display) {
        auto entry = new QStandardItem();
        entry->setData(display, Qt::DisplayRole);
        entry->setData(row + 1, Qt::UserRole);
        parent->setChild(row, column, entry);
        return entry;
    };
    for (int i = 0; i < 4; i++) {
        auto entry = makeEntry(model->invisibleRootItem(), i, 0, QLocale::c().monthName(i + 1));
        if (i == 2) {
            makeEntry(entry, 0, 0, u"Monday"_s);
            makeEntry(entry, 1, 0, u"Tuesday"_s);
            makeEntry(entry, 2, 0, u"Wednesday"_s);
        } else if (i == 3) {
            entry->setColumnCount(2);
            makeEntry(entry, 0, 0, u"Saturday"_s);
            makeEntry(entry, 1, 0, u"Sunday"_s);
            makeEntry(entry, 0, 1, u"Apples"_s);
            makeEntry(entry, 1, 1, u"Oranges"_s);
        }
    }
    model->setItemRoleNames({{Qt::UserRole, "user"}, {Qt::DisplayRole, "display"}});
    return model;
}

QModelIndexList tst_IterQml::toIndexList(const QVariant &list)
{
    QModelIndexList result;
    const auto variantList = list.toList();
    result.reserve(variantList.size());
    for (const auto &variant : variantList) {
        result.append(variant.toModelIndex());
    }
    return result;
}

QList<int> tst_IterQml::toIntList(const QVariant &list)
{
    QList<int> result;
    const auto variantList = list.toList();
    result.reserve(variantList.size());
    for (const auto &variant : variantList) {
        result.append(variant.toInt());
    }
    return result;
}

QTEST_GUILESS_MAIN(tst_IterQml)

#include "iter_qml.moc"
