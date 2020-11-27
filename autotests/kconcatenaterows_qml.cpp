/*
    SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <QObject>

#include <QTest>
#include <QSignalSpy>

#include <QQmlApplicationEngine>
#include <QLoggingCategory>

#include <KConcatenateRowsProxyModel>

class tst_KConcatenateRowsQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        // avoid deprecation warning
        QLoggingCategory::setFilterRules(QStringLiteral("kf.itemmodels.quick.deprecations.*=false"));
    }
    void testQmlLoad();
};

void tst_KConcatenateRowsQml::testQmlLoad()
{
    QQmlApplicationEngine app;
    app.load(QFINDTESTDATA("concatenaterowstest.qml"));

    QCOMPARE(app.rootObjects().count(), 1);

    auto concatModel = qobject_cast<KConcatenateRowsProxyModel*>(app.rootObjects().first());
    QVERIFY(concatModel);

    QCOMPARE(concatModel->rowCount(), 4);

    QCOMPARE(concatModel->data(concatModel->index(0, 0)).toString(), "a");
    QCOMPARE(concatModel->data(concatModel->index(1, 0)).toString(), "b");
    QCOMPARE(concatModel->data(concatModel->index(2, 0)).toString(), "c");
    QCOMPARE(concatModel->data(concatModel->index(3, 0)).toString(), "d");
}

QTEST_MAIN(tst_KConcatenateRowsQml)

#include "kconcatenaterows_qml.moc"
