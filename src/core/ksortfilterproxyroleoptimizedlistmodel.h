/*
    SPDX-FileCopyrightText: 2025 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSORTFILTERPROXYROLEOPTIMIZEDLISTMODEL_H
#define KSORTFILTERPROXYROLEOPTIMIZEDLISTMODEL_H

#include <QSortFilterProxyModel>

#include "kitemmodels_export.h"

/*!
  \class KSortFilterProxyRoleOptimizedListModel
  \inmodule KItemModels
  \brief TODO.

  TODO

  \since TODO
*/
class KSortFilterProxyRoleOptimizedListModelPrivate;

class KITEMMODELS_EXPORT KSortFilterProxyRoleOptimizedListModel : public QSortFilterProxyModel
{
public:
    explicit KSortFilterProxyRoleOptimizedListModel(QObject *parent = nullptr);
    ~KSortFilterProxyRoleOptimizedListModel() override;

    QAbstractItemModel *model() const;
    void setModel(QAbstractItemModel *sourceModel);

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    void setInterestingRoles(const QList<int> &roles);

private:
    KSortFilterProxyRoleOptimizedListModelPrivate *d = nullptr;
};

#endif
