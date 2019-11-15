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

#include <QTest>
#include <QSignalSpy>

#include <QQmlApplicationEngine>

#include <KConcatenateRowsProxyModel>

class tst_KConcatenateRowsQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
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
