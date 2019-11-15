/*
 * Copyright (C) 2018 David Edmundson <davidedmundson@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#include <QSignalSpy>
#include <QTest>

#include <knumbermodel.h>
#include "test_model_helpers.h"
using namespace TestModelHelpers;

Q_DECLARE_METATYPE(QModelIndex)

class tst_KNumberModel: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init()
    {
        QLocale::setDefault(QLocale::c());
    }

    void basicTest()
    {
        KNumberModel m;
        m.setMinimumValue(3);
        m.setMaximumValue(5);
        QCOMPARE(m.rowCount(), 3);
        QCOMPARE(m.data(m.index(0, 0), Qt::DisplayRole), QVariant("3"));
        QCOMPARE(m.data(m.index(1, 0), Qt::DisplayRole), QVariant("4"));
        QCOMPARE(m.data(m.index(2, 0), Qt::DisplayRole), QVariant("5"));
    }

    void testUpdates()
    {
        KNumberModel m;
        m.setMinimumValue(3);
        m.setMaximumValue(5);
        QSignalSpy resetSpy(&m, &QAbstractItemModel::modelReset);
        m.setMaximumValue(7);
        QVERIFY(resetSpy.count() == 1);
    }

    void testStep()
    {
        KNumberModel m;
        m.setMinimumValue(3);
        m.setMaximumValue(4);
        m.setStepSize(0.4);
        QCOMPARE(m.rowCount(), 3);
        QCOMPARE(m.data(m.index(2, 0), Qt::DisplayRole), QVariant("3.8"));
    }

    void testLocale()
    {
        KNumberModel m;
        m.setMinimumValue(1000);
        m.setMaximumValue(1000);
        QCOMPARE(m.rowCount(), 1);
        QCOMPARE(m.data(m.index(0, 0), Qt::DisplayRole), QVariant("1,000"));

        m.setFormattingOptions(QLocale::OmitGroupSeparator);
        QCOMPARE(m.data(m.index(0, 0), Qt::DisplayRole), QVariant("1000"));

    }

private:
};

QTEST_MAIN(tst_KNumberModel)

#include "knumbermodeltest.moc"
