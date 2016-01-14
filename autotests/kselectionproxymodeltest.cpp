/*
    Copyright (c) 2015 Stephen Kelly <steveire@gmail.com>
    Copyright (c) 2015 David Faure <faure@kde.org>

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

#include <QTest>
#include <QStringListModel>
#include <QIdentityProxyModel>
#include <QSignalSpy>
#include <kselectionproxymodel.h>

#include "test_model_helpers.h"
using namespace TestModelHelpers;

class KSelectionProxyModelTest : public QObject
{
  Q_OBJECT
public:
  KSelectionProxyModelTest(QObject* parent = 0)
    : QObject(parent),
      days({
          QStringLiteral("Monday"),
          QStringLiteral("Tuesday"),
          QStringLiteral("Wednesday"),
          QStringLiteral("Thursday")
      })
  {
  }

private Q_SLOTS:
  void columnCountShouldBeStable();
  void selectOnSourceReset();

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
  void selectionModelModelChange();
#endif

private:
  const QStringList days;
};

void KSelectionProxyModelTest::columnCountShouldBeStable()
{
    // Given a KSelectionProxy on top of a stringlist model
    QStringListModel strings(days);
    QItemSelectionModel selectionModel(&strings);
    KSelectionProxyModel proxy(&selectionModel);
    proxy.setSourceModel(&strings);

    QSignalSpy rowATBISpy(&proxy, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)));
    QSignalSpy rowInsertedSpy(&proxy, SIGNAL(rowsInserted(QModelIndex,int,int)));

    // No selection => the proxy should have 0 rows, 1 column
    // (if it had 0 columns, it would have to emit column insertions, too much trouble)
    QCOMPARE(proxy.rowCount(), 0);
    QCOMPARE(proxy.columnCount(), 1);
    QCOMPARE(rowATBISpy.count(), 0);
    QCOMPARE(rowInsertedSpy.count(), 0);

    // Select second entry -> the proxy should have 1 rows, 1 column
    selectionModel.select(QItemSelection(strings.index(1, 0), strings.index(1, 0)), QItemSelectionModel::Select);
    QCOMPARE(proxy.rowCount(), 1);
    QCOMPARE(proxy.columnCount(), 1);
    QCOMPARE(rowSpyToText(rowATBISpy), QStringLiteral("0,0"));
    QCOMPARE(rowSpyToText(rowInsertedSpy), QStringLiteral("0,0"));
}

void KSelectionProxyModelTest::selectOnSourceReset()
{
  QStringListModel strings(days);
  QItemSelectionModel selectionModel(&strings);

  connect(&strings, &QAbstractItemModel::modelReset, [&] {
    selectionModel.select(QItemSelection(strings.index(0, 0), strings.index(2, 0)),
                          QItemSelectionModel::Select);
  });

  KSelectionProxyModel proxy(&selectionModel);
  proxy.setSourceModel(&strings);

  selectionModel.select(QItemSelection(strings.index(0, 0), strings.index(2, 0)),
                        QItemSelectionModel::Select);

  QCOMPARE(proxy.rowCount(), 3);
  for (int i = 0; i < 3; ++i)
    QCOMPARE(proxy.index(i, 0).data().toString(), days.at(i));

  QStringList numbers = { QStringLiteral("One"), QStringLiteral("Two"), QStringLiteral("Three"), QStringLiteral("Four") };
  strings.setStringList(numbers);

  QCOMPARE(proxy.rowCount(), 3);
  for (int i = 0; i < 3; ++i)
    QCOMPARE(proxy.index(i, 0).data().toString(), numbers.at(i));

  QVERIFY(selectionModel.selection().contains(strings.index(0, 0)));
  QVERIFY(selectionModel.selection().contains(strings.index(1, 0)));
  QVERIFY(selectionModel.selection().contains(strings.index(2, 0)));
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
void KSelectionProxyModelTest::selectionModelModelChange()
{
  QStringListModel strings(days);
  QItemSelectionModel selectionModel(&strings);

  QIdentityProxyModel identity;
  identity.setSourceModel(&strings);

  KSelectionProxyModel proxy(&selectionModel);
  proxy.setSourceModel(&identity);
  selectionModel.select(strings.index(0, 0), QItemSelectionModel::Select);

  QCOMPARE(proxy.rowCount(), 1);
  QCOMPARE(proxy.index(0, 0).data().toString(), QStringLiteral("Monday"));


  QStringListModel strings2({
    QStringLiteral("Today"),
    QStringLiteral("Tomorrow")
  });

  selectionModel.setModel(&strings2);
  proxy.setSourceModel(&strings2);
  selectionModel.select(strings2.index(0, 0), QItemSelectionModel::Select);

  QCOMPARE(proxy.rowCount(), 1);
  QCOMPARE(proxy.index(0, 0).data().toString(), QStringLiteral("Today"));

  QSignalSpy spy(&proxy, SIGNAL(modelReset()));

  QStringList numbers = { QStringLiteral("One"), QStringLiteral("Two"), QStringLiteral("Three"), QStringLiteral("Four") };
  strings.setStringList(numbers);

  QCOMPARE(spy.count(), 0);

  strings2.setStringList(numbers);

  QCOMPARE(spy.count(), 1);

  QCOMPARE(proxy.rowCount(), 0);
  QVERIFY(!selectionModel.hasSelection());

  selectionModel.select(strings2.index(0, 0), QItemSelectionModel::Select);

  QCOMPARE(proxy.rowCount(), 1);
  QCOMPARE(proxy.index(0, 0).data().toString(), numbers.at(0));
}
#endif

QTEST_MAIN(KSelectionProxyModelTest)

#include "kselectionproxymodeltest.moc"
