
#include <QTest>
#include <QStringListModel>
#include <QIdentityProxyModel>
#include <QSignalSpy>

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
  void selectionModelModelChange();
#endif
};

void KSelectionProxyModelTest::selectOnSourceReset()
{
  QStringList days = {
    QStringLiteral("Monday"),
    QStringLiteral("Tuesday"),
    QStringLiteral("Wednesday"),
    QStringLiteral("Thursday")
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
  QStringListModel strings({
    QStringLiteral("Monday"),
    QStringLiteral("Tuesday"),
    QStringLiteral("Wednesday"),
    QStringLiteral("Thursday")
  });
  QItemSelectionModel selectionModel(&strings);

  QIdentityProxyModel identity;
  identity.setSourceModel(&strings);

  KSelectionProxyModel proxy(&selectionModel);
  proxy.setSourceModel(&identity);
  selectionModel.select(strings.index(0, 0), QItemSelectionModel::Select);

  QCOMPARE(proxy.rowCount(), 1);
  QCOMPARE(proxy.index(0, 0).data().toString(), QString::fromLatin1("Monday"));


  QStringListModel strings2({
    QStringLiteral("Today"),
    QStringLiteral("Tomorrow")
  });

  selectionModel.setModel(&strings2);
  proxy.setSourceModel(&strings2);
  selectionModel.select(strings2.index(0, 0), QItemSelectionModel::Select);

  QCOMPARE(proxy.rowCount(), 1);
  QCOMPARE(proxy.index(0, 0).data().toString(), QString::fromLatin1("Today"));

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
