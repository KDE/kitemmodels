
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
  QStringListModel strings({
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday"
  });
  QItemSelectionModel selectionModel(&strings);

  connect(&strings, &QAbstractItemModel::modelReset, [&] {
    selectionModel.select(QItemSelection(strings.index(0, 0), strings.index(2, 0)),
                          QItemSelectionModel::Select);
  });

  KSelectionProxyModel proxy(&selectionModel);
  proxy.setSourceModel(&strings);

  selectionModel.select(QItemSelection(strings.index(0, 0), strings.index(2, 0)),
                        QItemSelectionModel::Select);

  strings.setStringList({ "One", "Two", "Three", "Four" });

  QVERIFY(selectionModel.selection().contains(strings.index(0, 0)));
  QVERIFY(selectionModel.selection().contains(strings.index(1, 0)));
  QVERIFY(selectionModel.selection().contains(strings.index(2, 0)));
}

QTEST_MAIN(KSelectionProxyModelTest)

#include "kselectionproxymodeltest.moc"
