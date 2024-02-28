/*
 *  SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <QJsonValue>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSignalSpy>
#include <QTest>

#ifdef IMPORT_ITEMMODELSPLUGIN
#include <QPluginLoader>
Q_IMPORT_PLUGIN(Plugin)
#endif

class tst_KAggregateQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testAggregateSomeInit();
    void testAggregateSomeStatic();
    void testAggregateSomeDynamic();

    void testAggregateEveryInit();
    void testAggregateEveryStatic();
    void testAggregateEveryDynamic();
};

void tst_KAggregateQml::testAggregateSomeInit()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        import QtQml
        import org.kde.kitemmodels as KItemModels

        QtObject {
            readonly property bool value: aggregate.value

            readonly property KItemModels.KAggregateSome aggregate: KItemModels.KAggregateSome {
                id: aggregate
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto root = app.rootObjects().first();
    QCOMPARE(root->property("value").toBool(), false);
}

void tst_KAggregateQml::testAggregateSomeStatic()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        import QtQml
        import org.kde.kitemmodels as KItemModels

        QtObject {
            readonly property bool value: aggregate.value

            readonly property QtObject aggregateAttached0: object0.KItemModels.KAggregateSome
            readonly property QtObject aggregateAttached1: object1.KItemModels.KAggregateSome
            readonly property QtObject aggregateAttached2: object2.KItemModels.KAggregateSome

            readonly property KItemModels.KAggregateSome aggregate: KItemModels.KAggregateSome {
                id: aggregate
            }

            readonly property QtObject object0: QtObject {
                KItemModels.KAggregateSome.group: aggregate
                // test default value
            }

            readonly property QtObject object1: QtObject {
                KItemModels.KAggregateSome.group: aggregate
                KItemModels.KAggregateSome.value: false
            }

            readonly property QtObject object2: QtObject {
                KItemModels.KAggregateSome.group: aggregate
                KItemModels.KAggregateSome.value: true
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto root = app.rootObjects().first();
    const auto aggregate = root->property("aggregate").value<QObject *>();
    const auto aggregateAttached0 = root->property("aggregateAttached0").value<QObject *>();
    const auto aggregateAttached1 = root->property("aggregateAttached1").value<QObject *>();
    const auto aggregateAttached2 = root->property("aggregateAttached2").value<QObject *>();
    QVERIFY(aggregate && aggregateAttached0 && aggregateAttached1 && aggregateAttached2);

    QCOMPARE(aggregateAttached0->property("group").value<QObject *>(), aggregate);
    QCOMPARE(aggregateAttached1->property("group").value<QObject *>(), aggregate);
    QCOMPARE(aggregateAttached2->property("group").value<QObject *>(), aggregate);

    QCOMPARE(aggregateAttached0->property("value").toBool(), false);
    QCOMPARE(aggregateAttached1->property("value").toBool(), false);
    QCOMPARE(aggregateAttached2->property("value").toBool(), true);

    QCOMPARE(aggregate->property("value").toBool(), true);
    QCOMPARE(root->property("value").toBool(), true);

    QSignalSpy spy(aggregate, SIGNAL(valueChanged()));
    QVERIFY(spy.isValid());

    QVERIFY(aggregateAttached2->setProperty("value", false));
    QCOMPARE(aggregate->property("value").toBool(), false);
    QCOMPARE(spy.count(), 1);

    QVERIFY(aggregateAttached1->setProperty("value", true));
    QCOMPARE(aggregate->property("value").toBool(), true);
    QCOMPARE(spy.count(), 2);

    QVERIFY(aggregateAttached2->setProperty("value", true));
    QCOMPARE(aggregate->property("value").toBool(), true);
    QCOMPARE(spy.count(), 2); // did not fire again
}

void tst_KAggregateQml::testAggregateSomeDynamic()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        pragma ComponentBehavior: Bound

        import QtQml
        import org.kde.kitemmodels as KItemModels

        QtObject {
            id: root

            readonly property bool value: aggregate.value

            property list<bool> model: []

            readonly property Instantiator instantiator: Instantiator {
                model: root.model

                QtObject {
                    required property int index
                    required property bool modelData

                    KItemModels.KAggregateSome.group: aggregate
                    KItemModels.KAggregateSome.value: modelData
                }
            }

            readonly property KItemModels.KAggregateSome aggregate: KItemModels.KAggregateSome {
                id: aggregate
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto root = app.rootObjects().first();
    const auto aggregate = root->property("aggregate").value<QObject *>();
    QVERIFY(aggregate);

    QSignalSpy spy(aggregate, SIGNAL(valueChanged()));
    QVERIFY(spy.isValid());

    auto model = QList<bool>{true, false, true, false, false};
    root->setProperty("model", QVariant::fromValue(model));

    QCOMPARE(aggregate->property("value").toBool(), true);
    QCOMPARE(spy.count(), 1);
}

void tst_KAggregateQml::testAggregateEveryInit()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        import QtQml
        import org.kde.kitemmodels as KItemModels

        QtObject {
            readonly property bool value: aggregate.value

            readonly property KItemModels.KAggregateEvery aggregate: KItemModels.KAggregateEvery {
                id: aggregate
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto root = app.rootObjects().first();
    QCOMPARE(root->property("value").toBool(), true);
}

void tst_KAggregateQml::testAggregateEveryStatic()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        import QtQml
        import org.kde.kitemmodels as KItemModels

        QtObject {
            readonly property bool value: aggregate.value

            readonly property QtObject aggregateAttached0: object0.KItemModels.KAggregateEvery
            readonly property QtObject aggregateAttached1: object1.KItemModels.KAggregateEvery
            readonly property QtObject aggregateAttached2: object2.KItemModels.KAggregateEvery

            readonly property KItemModels.KAggregateEvery aggregate: KItemModels.KAggregateEvery {
                id: aggregate
            }

            readonly property QtObject object0: QtObject {
                KItemModels.KAggregateEvery.group: aggregate
                // test default value
            }

            readonly property QtObject object1: QtObject {
                KItemModels.KAggregateEvery.group: aggregate
                KItemModels.KAggregateEvery.value: false
            }

            readonly property QtObject object2: QtObject {
                KItemModels.KAggregateEvery.group: aggregate
                KItemModels.KAggregateEvery.value: true
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto root = app.rootObjects().first();
    const auto aggregate = root->property("aggregate").value<QObject *>();
    const auto aggregateAttached0 = root->property("aggregateAttached0").value<QObject *>();
    const auto aggregateAttached1 = root->property("aggregateAttached1").value<QObject *>();
    const auto aggregateAttached2 = root->property("aggregateAttached2").value<QObject *>();
    QVERIFY(aggregate && aggregateAttached0 && aggregateAttached1 && aggregateAttached2);

    QCOMPARE(aggregateAttached0->property("group").value<QObject *>(), aggregate);
    QCOMPARE(aggregateAttached1->property("group").value<QObject *>(), aggregate);
    QCOMPARE(aggregateAttached2->property("group").value<QObject *>(), aggregate);

    QCOMPARE(aggregateAttached0->property("value").toBool(), false);
    QCOMPARE(aggregateAttached1->property("value").toBool(), false);
    QCOMPARE(aggregateAttached2->property("value").toBool(), true);

    QCOMPARE(aggregate->property("value").toBool(), false);
    QCOMPARE(root->property("value").toBool(), false);

    QSignalSpy spy(aggregate, SIGNAL(valueChanged()));
    QVERIFY(spy.isValid());

    QVERIFY(aggregateAttached2->setProperty("value", false));
    QCOMPARE(aggregate->property("value").toBool(), false);
    QCOMPARE(spy.count(), 0);

    QVERIFY(aggregateAttached0->setProperty("value", true));
    QVERIFY(aggregateAttached1->setProperty("value", true));
    QVERIFY(aggregateAttached2->setProperty("value", true));
    QCOMPARE(aggregate->property("value").toBool(), true);
    QCOMPARE(spy.count(), 1);

    QVERIFY(aggregateAttached1->setProperty("value", false));
    QCOMPARE(aggregate->property("value").toBool(), false);
    QCOMPARE(spy.count(), 2);
}

void tst_KAggregateQml::testAggregateEveryDynamic()
{
    QQmlApplicationEngine app;
    app.loadData(R"(
        pragma ComponentBehavior: Bound

        import QtQml
        import org.kde.kitemmodels as KItemModels

        QtObject {
            id: root

            readonly property bool value: aggregate.value

            property list<bool> model: []

            readonly property Instantiator instantiator: Instantiator {
                model: root.model

                QtObject {
                    required property int index
                    required property bool modelData

                    KItemModels.KAggregateEvery.group: aggregate
                    KItemModels.KAggregateEvery.value: modelData
                }
            }

            readonly property KItemModels.KAggregateEvery aggregate: KItemModels.KAggregateEvery {
                id: aggregate
            }
        }
    )");
    QCOMPARE(app.rootObjects().count(), 1);
    const auto root = app.rootObjects().first();
    const auto aggregate = root->property("aggregate").value<QObject *>();
    QVERIFY(aggregate);

    QSignalSpy spy(aggregate, SIGNAL(valueChanged()));
    QVERIFY(spy.isValid());

    auto model = QList<bool>{true, false, true, false, false};
    root->setProperty("model", QVariant::fromValue(model));

    QCOMPARE(aggregate->property("value").toBool(), false);
    QCOMPARE(spy.count(), 3);
}

QTEST_GUILESS_MAIN(tst_KAggregateQml)

#include "kaggregate_qml.moc"
