/*
    Copyright (C) 2019 David Edmundson <davidedmundson@kde.org>

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

#include <QObject>

#include <QSignalSpy>
#include <QTest>

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QAbstractItemModel>
#include <QStandardItemModel>

class tst_KSortFilterProxyModelQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testFilterCallback();
    void testSortRole_data();
    void testSortRole();
    void testFilterRegExp();
    void testFilterRegExpRole();

private:
    QAbstractItemModel *createMonthTestModel(QObject *parent);
};

QAbstractItemModel *tst_KSortFilterProxyModelQml::createMonthTestModel(QObject *parent)
{
    auto testModel = new QStandardItemModel(parent);
    for (int i = 1; i <= 12; i++) {
        auto entry = new QStandardItem();
        entry->setData(QLocale::c().monthName(i), Qt::DisplayRole);
        entry->setData(i, Qt::UserRole);
        testModel->appendRow(entry);
    }
    testModel->setItemRoleNames({{Qt::UserRole, "user"}, {Qt::DisplayRole, "display"}});
    return testModel;
}

void tst_KSortFilterProxyModelQml::testFilterCallback()
{
    QQmlApplicationEngine app;
    app.loadData(
        "import QtQml 2.0\n"
        "import org.kde.kitemmodels 1.0\n"
        "KSortFilterProxyModel\n"
        "{\n"
        "    property int modulo: 2\n"
        "    sourceModel: KNumberModel {\n"
        "        minimumValue: 1\n"
        "        maximumValue: 10\n"
        "    }\n"
        "    filterRowCallback: function(source_row, source_parent) {\n"
        "        return sourceModel.data(sourceModel.index(source_row, 0, source_parent), Qt.DisplayRole) % modulo == 1;\n"
        "    };\n"
        "}\n");
    QCOMPARE(app.rootObjects().count(), 1);

    auto filterModel = qobject_cast<QAbstractItemModel *>(app.rootObjects().first());
    QVERIFY(filterModel);

    QCOMPARE(filterModel->rowCount(), 5);
    QCOMPARE(filterModel->data(filterModel->index(0, 0)).toString(), "1");
    QCOMPARE(filterModel->data(filterModel->index(1, 0)).toString(), "3");
    QCOMPARE(filterModel->data(filterModel->index(2, 0)).toString(), "5");
    QCOMPARE(filterModel->data(filterModel->index(3, 0)).toString(), "7");
    QCOMPARE(filterModel->data(filterModel->index(4, 0)).toString(), "9");

    filterModel->setProperty("modulo", 3);

    // Nothing should change until we call invalidateFilter
    QCOMPARE(filterModel->rowCount(), 5);
    QCOMPARE(filterModel->data(filterModel->index(0, 0)).toString(), "1");
    QCOMPARE(filterModel->data(filterModel->index(1, 0)).toString(), "3");
    QCOMPARE(filterModel->data(filterModel->index(2, 0)).toString(), "5");
    QCOMPARE(filterModel->data(filterModel->index(3, 0)).toString(), "7");
    QCOMPARE(filterModel->data(filterModel->index(4, 0)).toString(), "9");

    // Simulate call from QML by going through metaobject rather than calling it directly
    const bool invalidateFilterCallOk = QMetaObject::invokeMethod(filterModel, "invalidateFilter");
    QVERIFY(invalidateFilterCallOk);

    QCOMPARE(filterModel->rowCount(), 4);
    QCOMPARE(filterModel->data(filterModel->index(0, 0)).toString(), "1");
    QCOMPARE(filterModel->data(filterModel->index(1, 0)).toString(), "4");
    QCOMPARE(filterModel->data(filterModel->index(2, 0)).toString(), "7");
    QCOMPARE(filterModel->data(filterModel->index(3, 0)).toString(), "10");
}

void tst_KSortFilterProxyModelQml::testSortRole_data()
{
    // test model consists of all month names + month number as Display and UserRoler respectively

    QTest::addColumn<QString>("qmlContents");
    QTest::addColumn<QString>("result");

    QTest::newRow("sort by role name - display") << "KSortFilterProxyModel {"
                                                    " sourceModel: testModel;"
                                                    " sortRole: \"display\";"
                                                    "}"
                                                 << "April";
    QTest::newRow("sort by role name - value") << "KSortFilterProxyModel {"
                                                  " sourceModel: testModel;"
                                                  " sortRole: \"user\";"
                                                  "}"
                                               << "January";
    QTest::newRow("sort by role name - reset") << "KSortFilterProxyModel {"
                                                  " sourceModel: testModel;"
                                                  " sortRole: \"\";"
                                                  " Component.onCompleted: sortRole = \"\";"
                                                  "}"
                                               << "January";
}

void tst_KSortFilterProxyModelQml::testSortRole()
{
    QQmlApplicationEngine app;
    QFETCH(QString, qmlContents);
    QFETCH(QString, result);

    qmlContents =
        "import org.kde.kitemmodels 1.0\n"
        "import QtQuick 2.0\n"
        + qmlContents;

    app.rootContext()->setContextProperty("testModel", createMonthTestModel(&app));

    app.loadData(qmlContents.toLatin1());

    QCOMPARE(app.rootObjects().count(), 1);
    auto filterModel = qobject_cast<QAbstractItemModel *>(app.rootObjects().first());
    QVERIFY(filterModel);
    QCOMPARE(filterModel->rowCount(), 12);
    QCOMPARE(filterModel->data(filterModel->index(0, 0), Qt::DisplayRole).toString(), result);
}

void tst_KSortFilterProxyModelQml::testFilterRegExp()
{
    // filterRegExp comes from the QSortFilterProxyModel directly, confirm it still works
    QQmlApplicationEngine app;

    app.rootContext()->setContextProperty("testModel", createMonthTestModel(&app));

    app.loadData(
        "import QtQml 2.0\n"
        "import org.kde.kitemmodels 1.0\n"
        "KSortFilterProxyModel {\n"
        " sourceModel: testModel\n"
        " filterRegExp: /Ma.*/\n"
        "}\n");

    QCOMPARE(app.rootObjects().count(), 1);
    auto filterModel = qobject_cast<QAbstractItemModel *>(app.rootObjects().first());
    QVERIFY(filterModel);
    QCOMPARE(filterModel->rowCount(), 2);
    QCOMPARE(filterModel->data(filterModel->index(0, 0), Qt::DisplayRole).toString(), "March");
    QCOMPARE(filterModel->data(filterModel->index(1, 0), Qt::DisplayRole).toString(), "May");
}

void tst_KSortFilterProxyModelQml::testFilterRegExpRole()
{
    // filterRegExp comes from the QSortFilterProxyModel directly, confirm it still works
    QQmlApplicationEngine app;

    app.rootContext()->setContextProperty("testModel", createMonthTestModel(&app));

    app.loadData(
        "import QtQml 2.0\n"
        "import org.kde.kitemmodels 1.0\n"
        "KSortFilterProxyModel {\n"
        " sourceModel: testModel\n"
        " filterRole: \"user\"\n"
        " filterRegExp: /1[0-9]/\n" // month value is 10 or more
        "}\n");

    QCOMPARE(app.rootObjects().count(), 1);
    auto filterModel = qobject_cast<QAbstractItemModel *>(app.rootObjects().first());
    QVERIFY(filterModel);
    QCOMPARE(filterModel->rowCount(), 3);
    QCOMPARE(filterModel->data(filterModel->index(0, 0), Qt::DisplayRole).toString(), "October");
    QCOMPARE(filterModel->data(filterModel->index(1, 0), Qt::DisplayRole).toString(), "November");
    QCOMPARE(filterModel->data(filterModel->index(2, 0), Qt::DisplayRole).toString(), "December");
}

QTEST_GUILESS_MAIN(tst_KSortFilterProxyModelQml)

#include "ksortfilterproxymodel_qml.moc"
