/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
    SPDX-FileContributor: Stephen Kelly <stephen@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KBREADCRUMBSELECTIONMODEL_H
#define KBREADCRUMBSELECTIONMODEL_H

#include <QItemSelectionModel>

#include "kitemmodels_export.h"

#include <memory>

class KBreadcrumbSelectionModelPrivate;

/*!
  \class KBreadcrumbSelectionModel
  \inmodule KItemModels
  \brief Selects the parents of selected items to create breadcrumbs.

  For example, if the tree is
  \code
    - A
    - B
    - - C
    - - D
    - - - E
    - - - - F
  \endcode

  and E is selected, the selection can contain

  \code
    - B
    - D
  \endcode

  or

  \code
    - B
    - D
    - E
  \endcode

  if isActualSelectionIncluded is true.

  The depth of the selection may also be set. For example if the breadcrumbLength is 1:

  \code
    - D
    - E
  \endcode

  And if breadcrumbLength is 2:

  \code
    - B
    - D
    - E
  \endcode

  A KBreadcrumbsSelectionModel with a breadcrumbLength of 0 and including the actual selection is
  the same as a KSelectionProxyModel in the KSelectionProxyModel::ExactSelection configuration.

  \code
    view1->setModel(rootModel);

    QItemSelectionModel *breadcrumbSelectionModel = new QItemSelectionModel(rootModel, this);

    KBreadcrumbSelectionModel *breadcrumbProxySelector = new KBreadcrumbSelectionModel(breadcrumbSelectionModel, rootModel, this);

    view1->setSelectionModel(breadcrumbProxySelector);

    KSelectionProxyModel *breadcrumbSelectionProxyModel = new KSelectionProxyModel( breadcrumbSelectionModel, this);
    breadcrumbSelectionProxyModel->setSourceModel( rootModel );
    breadcrumbSelectionProxyModel->setFilterBehavior( KSelectionProxyModel::ExactSelection );

    view2->setModel(breadcrumbSelectionProxyModel);
  \endcode

  \image kbreadcrumbselectionmodel.png "KBreadcrumbSelectionModel in several configurations"

  This can work in two directions. One option is for a single selection in the KBreadcrumbSelectionModel to invoke
  the breadcrumb selection in its constructor argument.

  The other is for a selection in the itemselectionmodel in the constructor argument to cause a breadcrumb selection
  in this.

  \since 4.5

*/
class KITEMMODELS_EXPORT KBreadcrumbSelectionModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    /*!
     * \value MakeBreadcrumbSelectionInOther
     * \value MakeBreadcrumbSelectionInSelf
     */
    enum BreadcrumbTarget {
        MakeBreadcrumbSelectionInOther,
        MakeBreadcrumbSelectionInSelf,
    };

    /*!
     *
     */
    explicit KBreadcrumbSelectionModel(QItemSelectionModel *selectionModel, QObject *parent = nullptr);

    /*!
     *
     */
    KBreadcrumbSelectionModel(QItemSelectionModel *selectionModel, BreadcrumbTarget target, QObject *parent = nullptr);

    ~KBreadcrumbSelectionModel() override;

    /*!
      Returns whether the actual selection in included in the proxy.

      The default is true.
    */
    bool isActualSelectionIncluded() const;

    /*!
      Set whether the actual selection in included in the proxy to \a isActualSelectionIncluded.
    */
    void setActualSelectionIncluded(bool isActualSelectionIncluded);

    /*!
      Returns the depth that the breadcrumb selection should go to.
    */
    int breadcrumbLength() const;

    /*!
      Sets the depth that the breadcrumb selection should go to.

      If the \a breadcrumbLength is -1, all breadcrumbs are selected.
      The default is -1
    */
    void setBreadcrumbLength(int breadcrumbLength);

    void select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command) override;

    void select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command) override;

protected:
    std::unique_ptr<KBreadcrumbSelectionModelPrivate> const d_ptr;

private:
    Q_DECLARE_PRIVATE(KBreadcrumbSelectionModel)
};

#endif
