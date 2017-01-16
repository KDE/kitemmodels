/*
    Copyright (c) 2016 Ableton AG <info@ableton.com>
        Author Stephen Kelly <stephen.kelly@ableton.com>

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
#include <QTest>
#include <QStringListModel>
#include <QIdentityProxyModel>
#include <QItemSelection>

#include "kmodelindexproxymapper.h"

class ModelIndexProxyMapperTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void init();

    void testIndexMapping();
    void testSelectionMapping();
    void selfConnection();
    void connectedChangedSimple();
    void connectedChangedComplex();
    void crossWires();
    void isConnected();

private:
    QStringListModel baseModel;

    QIdentityProxyModel proxy_common1;

    QIdentityProxyModel proxy_left1;
    QIdentityProxyModel proxy_left2;
    QIdentityProxyModel proxy_left3;

    QIdentityProxyModel proxy_right1;
    QIdentityProxyModel proxy_right2;
    QIdentityProxyModel proxy_right3;
    QIdentityProxyModel proxy_right4;
};

void ModelIndexProxyMapperTest::init()
{
    baseModel.setStringList({"Monday", "Tuesday", "Wednesday"});

    proxy_common1.setSourceModel(&baseModel);

    proxy_left1.setSourceModel(&proxy_common1);
    proxy_left2.setSourceModel(&proxy_left1);
    proxy_left3.setSourceModel(&proxy_left2);

    proxy_right1.setSourceModel(&proxy_common1);
    proxy_right2.setSourceModel(&proxy_right1);
    proxy_right3.setSourceModel(&proxy_right2);
    proxy_right4.setSourceModel(&proxy_right3);
}

void ModelIndexProxyMapperTest::testIndexMapping()
{
    KModelIndexProxyMapper mapper(&proxy_left3, &proxy_right4);

    auto leftIdx = proxy_left3.index(0, 0);
    auto rightIdx = proxy_right4.index(0, 0);

    QVERIFY(leftIdx.isValid());
    QVERIFY(rightIdx.isValid());

    QCOMPARE(mapper.mapLeftToRight(leftIdx), rightIdx);
    QCOMPARE(mapper.mapRightToLeft(rightIdx), leftIdx);
}

void ModelIndexProxyMapperTest::testSelectionMapping()
{
    KModelIndexProxyMapper mapper(&proxy_left3, &proxy_right4);

    auto leftIdx = proxy_left3.index(0, 0);
    auto rightIdx = proxy_right4.index(0, 0);

    auto leftSel = QItemSelection(leftIdx, leftIdx);
    auto rightSel = QItemSelection(rightIdx, rightIdx);

    QCOMPARE(leftSel.indexes().size(), 1);
    QCOMPARE(rightSel.indexes().size(), 1);

    QVERIFY(leftSel.indexes().first().isValid());
    QVERIFY(rightSel.indexes().first().isValid());

    QCOMPARE(mapper.mapSelectionLeftToRight(leftSel), rightSel);
    QCOMPARE(mapper.mapSelectionRightToLeft(rightSel), leftSel);
}

void ModelIndexProxyMapperTest::selfConnection()
{
    KModelIndexProxyMapper mapper(&baseModel, &baseModel);
    QVERIFY(mapper.isConnected());

    auto idx = baseModel.index(0, 0);
    QVERIFY(idx.isValid());

    QCOMPARE(mapper.mapLeftToRight(idx), idx);
}

void ModelIndexProxyMapperTest::connectedChangedSimple()
{
    QIdentityProxyModel proxy1;
    Q_SET_OBJECT_NAME(proxy1);

    KModelIndexProxyMapper mapper(&proxy1, &baseModel);

    QSignalSpy spy(&mapper, SIGNAL(isConnectedChanged()));

    QVERIFY(!mapper.isConnected());
    proxy1.setSourceModel(&baseModel);

    QVERIFY(mapper.isConnected());
    QCOMPARE(spy.count(), 1);
}

void ModelIndexProxyMapperTest::connectedChangedComplex()
{
    KModelIndexProxyMapper mapper(&proxy_left3, &proxy_right4);

    QSignalSpy spy(&mapper, SIGNAL(isConnectedChanged()));

    QVERIFY(mapper.isConnected());

    proxy_right2.setSourceModel(nullptr);

    QVERIFY(!mapper.isConnected());
    QCOMPARE(spy.count(), 1);

    proxy_right2.setSourceModel(&proxy_right1);

    QVERIFY(mapper.isConnected());
    QCOMPARE(spy.count(), 2);

    auto leftIdx = proxy_left3.index(0, 0);
    QVERIFY(leftIdx.isValid());
    auto rightIdx = mapper.mapLeftToRight(leftIdx);
    QVERIFY(rightIdx.isValid());
    QCOMPARE(mapper.mapRightToLeft(rightIdx), leftIdx);

    QIdentityProxyModel replacement_right1;
    replacement_right1.setSourceModel(&proxy_right1);
    proxy_right2.setSourceModel(&replacement_right1);

    QVERIFY(mapper.isConnected());
    QCOMPARE(spy.count(), 2);
}

void ModelIndexProxyMapperTest::crossWires()
{
    KModelIndexProxyMapper mapper(&proxy_left3, &proxy_right4);

    QSignalSpy spy(&mapper, SIGNAL(isConnectedChanged()));

    QVERIFY(mapper.isConnected());

    proxy_left3.setSourceModel(&proxy_right3);

    QVERIFY(mapper.isConnected());
    QCOMPARE(spy.count(), 0);

    {
    auto leftIdx = proxy_left3.index(0, 0);
    auto rightIdx = proxy_right4.index(0, 0);
    QCOMPARE(mapper.mapLeftToRight(leftIdx), rightIdx);
    }

    proxy_right4.setSourceModel(&proxy_left2);

    QVERIFY(mapper.isConnected());
    QCOMPARE(spy.count(), 0);

    {
    auto leftIdx = proxy_left3.index(0, 0);
    auto rightIdx = proxy_right4.index(0, 0);
    QCOMPARE(mapper.mapLeftToRight(leftIdx), rightIdx);
    }
}

void ModelIndexProxyMapperTest::isConnected()
{
    KModelIndexProxyMapper mapper1(&proxy_left1, &baseModel);
    QVERIFY(mapper1.isConnected());
    KModelIndexProxyMapper mapper2(&baseModel, &proxy_left1);
    QVERIFY(mapper2.isConnected());
}

QTEST_MAIN(ModelIndexProxyMapperTest)
#include "kmodelindexproxymappertest.moc"
