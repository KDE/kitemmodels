
#include <QTest>
#include <QStringListModel>

#include <kselectionproxymodel.h>

class KSelectionProxyModelTest : public QObject
{
  Q_OBJECT
public:
  KSelectionProxyModelTest(QObject* parent = 0)
    : QObject(parent)
  {

  }

private Q_SLOTS:
  void selectOnSourceReset();
};

void KSelectionProxyModelTest::selectOnSourceReset()
{
  QStringList days = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday"
  };
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

  QStringList numbers = { "One", "Two", "Three", "Four" };
  strings.setStringList(numbers);

  QCOMPARE(proxy.rowCount(), 3);
  for (int i = 0; i < 3; ++i)
    QCOMPARE(proxy.index(i, 0).data().toString(), numbers.at(i));

  QVERIFY(selectionModel.selection().contains(strings.index(0, 0)));
  QVERIFY(selectionModel.selection().contains(strings.index(1, 0)));
  QVERIFY(selectionModel.selection().contains(strings.index(2, 0)));
}

QTEST_MAIN(KSelectionProxyModelTest)

#include "kselectionproxymodeltest.moc"
