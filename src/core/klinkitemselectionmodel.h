/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
    SPDX-FileContributor: Stephen Kelly <stephen@kdab.com>
    SPDX-FileCopyrightText: 2016 Ableton AG <info@ableton.com>
    SPDX-FileContributor: Stephen Kelly <stephen.kelly@ableton.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KLINKITEMSELECTIONMODEL_H
#define KLINKITEMSELECTIONMODEL_H

#include <QItemSelectionModel>

#include "kitemmodels_export.h"

#include <memory>

class KLinkItemSelectionModelPrivate;

/*!
  \class KLinkItemSelectionModel
  \inmodule KItemModels
  \brief Makes it possible to share a selection in multiple views which do not have the same source model.

  Although \l {https://doc.qt.io/qt-6/model-view-programming.html#handling-selections-of-items}{multiple views can share the same QItemSelectionModel},
  the views then need to have the same source model.

  If there is a proxy model between the model and one of the views, or different proxy models in each, this class makes
  it possible to share the selection between the views.

  \image kproxyitemselectionmodel-simple.png "Sharing a QItemSelectionModel between views on the same model is trivial"
  \image kproxyitemselectionmodel-error.png "If a proxy model is used, it is no longer possible to share the QItemSelectionModel directly"
  \image kproxyitemselectionmodel-solution.png "A KLinkItemSelectionModel can be used to map the selection through the proxy model"

  \code
    QAbstractItemModel *model = getModel();

    QSortFilterProxyModel *proxy = new QSortFilterProxyModel();
    proxy->setSourceModel(model);

    QTreeView *view1 = new QTreeView(splitter);
    view1->setModel(model);

    KLinkItemSelectionModel *view2SelectionModel = new KLinkItemSelectionModel( proxy, view1->selectionModel());

    QTreeView *view2 = new QTreeView(splitter);
    // Note that the QAbstractItemModel passed to KLinkItemSelectionModel must be the same as what is used in the view
    view2->setModel(proxy);
    view2->setSelectionModel( view2SelectionModel );
  \endcode

  \image kproxyitemselectionmodel-complex.png "Arbitrarily complex proxy configurations on the same root model can be used"

  \code
    QAbstractItemModel *model = getModel();

    QSortFilterProxyModel *proxy1 = new QSortFilterProxyModel();
    proxy1->setSourceModel(model);
    QSortFilterProxyModel *proxy2 = new QSortFilterProxyModel();
    proxy2->setSourceModel(proxy1);
    QSortFilterProxyModel *proxy3 = new QSortFilterProxyModel();
    proxy3->setSourceModel(proxy2);

    QTreeView *view1 = new QTreeView(splitter);
    view1->setModel(proxy3);

    QSortFilterProxyModel *proxy4 = new QSortFilterProxyModel();
    proxy4->setSourceModel(model);
    QSortFilterProxyModel *proxy5 = new QSortFilterProxyModel();
    proxy5->setSourceModel(proxy4);

    KLinkItemSelectionModel *view2SelectionModel = new KLinkItemSelectionModel( proxy5, view1->selectionModel());

    QTreeView *view2 = new QTreeView(splitter);
    // Note that the QAbstractItemModel passed to KLinkItemSelectionModel must be the same as what is used in the view
    view2->setModel(proxy5);
    view2->setSelectionModel( view2SelectionModel );
  \endcode

  See also \l {https://commits.kde.org/kitemmodels?path=tests/proxymodeltestapp/proxyitemselectionwidget.cpp}{kitemmodels:
  tests/proxymodeltestapp/proxyitemselectionwidget.cpp}.

  \since 4.5
*/
class KITEMMODELS_EXPORT KLinkItemSelectionModel : public QItemSelectionModel
{
    Q_OBJECT

    /*!
     * \property KLinkItemSelectionModel::linkedItemSelectionModel
     */
    Q_PROPERTY(
        QItemSelectionModel *linkedItemSelectionModel READ linkedItemSelectionModel WRITE setLinkedItemSelectionModel NOTIFY linkedItemSelectionModelChanged)
public:
    /*!
      Constructor.
    */
    KLinkItemSelectionModel(QAbstractItemModel *targetModel, QItemSelectionModel *linkedItemSelectionModel, QObject *parent = nullptr);

    /*!
     *
     */
    explicit KLinkItemSelectionModel(QObject *parent = nullptr);

    ~KLinkItemSelectionModel() override;

    QItemSelectionModel *linkedItemSelectionModel() const;
    void setLinkedItemSelectionModel(QItemSelectionModel *selectionModel);

    void select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) override;
    void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command) override;

Q_SIGNALS:
    void linkedItemSelectionModelChanged();

protected:
    std::unique_ptr<KLinkItemSelectionModelPrivate> const d_ptr;

private:
    Q_DECLARE_PRIVATE(KLinkItemSelectionModel)
    Q_PRIVATE_SLOT(d_func(), void sourceSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected))
    Q_PRIVATE_SLOT(d_func(), void sourceCurrentChanged(const QModelIndex &current))
    Q_PRIVATE_SLOT(d_func(), void slotCurrentChanged(const QModelIndex &current))
};

#endif
