/*
    SPDX-FileCopyrightText: 2025 Albert Astals Cid <aacid@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ksortfilterproxyroleoptimizedlistmodel.h"

#include <QIdentityProxyModel>

#include <algorithm>

class HelperProxyModel : public QIdentityProxyModel
{
public:
    HelperProxyModel()
    {
        setHandleSourceDataChanges(false);
    }

    void setSourceModel(QAbstractItemModel *model) override
    {
        QObject::disconnect(m_dataChangedConnection);

        QIdentityProxyModel::setSourceModel(model);

        m_dataChangedConnection = connect(model, &QAbstractItemModel::dataChanged, this, &HelperProxyModel::handleDataChanged);
    }

    void handleDataChanged(const QModelIndex &source_top_left, const QModelIndex &source_bottom_right, const QList<int> &roles)
    {
        Q_ASSERT(source_top_left.column() == 0);
        Q_ASSERT(source_bottom_right.column() == 0);
        const bool interestingRole = roles.isEmpty() || std::any_of(m_roles.begin(), m_roles.end(), [roles](int role) {
                                         return roles.contains(role);
                                     });

        if (interestingRole) {
            Q_EMIT dataChanged(source_top_left, source_bottom_right, roles);
        } else {
            // we still need to emit dataChanged but we are going to emit directly the one in QSortFilterProxyModel
            // so that it does not call lessThan or filterAccepts (i.e bypass _q_sourceDataChanged altogether)

            QList<QModelIndex> indicesChanged;

            for (int row = source_top_left.row(); row <= source_bottom_right.row(); ++row) {
                const QModelIndex thisModelIndex = mapFromSource(source_top_left.siblingAtRow(row));
                const QModelIndex parentModelIndex = m_parentModel->mapFromSource(thisModelIndex);
                if (parentModelIndex.isValid()) {
                    indicesChanged << parentModelIndex;
                }
            }

            if (indicesChanged.isEmpty()) {
                return;
            }

            std::ranges::sort(indicesChanged, [](const QModelIndex &a, const QModelIndex &b) {
                return a.row() < b.row();
            });

            QList<QModelIndex> currentBatchOfIndicesToEmit;
            for (const QModelIndex &index : std::as_const(indicesChanged)) {
                if (!currentBatchOfIndicesToEmit.isEmpty() && index.row() != currentBatchOfIndicesToEmit.last().row() + 1) {
                    Q_EMIT m_parentModel->dataChanged(currentBatchOfIndicesToEmit.first(), currentBatchOfIndicesToEmit.last(), roles);

                    currentBatchOfIndicesToEmit.clear();
                }
                currentBatchOfIndicesToEmit << index;
            }
            if (!currentBatchOfIndicesToEmit.isEmpty()) {
                Q_EMIT m_parentModel->dataChanged(currentBatchOfIndicesToEmit.first(), currentBatchOfIndicesToEmit.last(), roles);
            }
        }
    }

    QMetaObject::Connection m_dataChangedConnection;
    QList<int> m_roles;
    QSortFilterProxyModel *m_parentModel;
};

class KSortFilterProxyRoleOptimizedListModelPrivate
{
public:
    HelperProxyModel m_helper;
};

KSortFilterProxyRoleOptimizedListModel::KSortFilterProxyRoleOptimizedListModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d(new KSortFilterProxyRoleOptimizedListModelPrivate())
{
    d->m_helper.m_parentModel = this;
    QSortFilterProxyModel::setSourceModel(&d->m_helper);
}

KSortFilterProxyRoleOptimizedListModel::~KSortFilterProxyRoleOptimizedListModel()
{
    delete d;
}

QAbstractItemModel *KSortFilterProxyRoleOptimizedListModel::model() const
{
    return d->m_helper.sourceModel();
}

void KSortFilterProxyRoleOptimizedListModel::setModel(QAbstractItemModel *sourceModel)
{
    d->m_helper.setSourceModel(sourceModel);
}

void KSortFilterProxyRoleOptimizedListModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    qWarning() << "You should NOT call KSortFilterProxyRoleOptimizedListModel::setSourceModel. Call KSortFilterProxyRoleOptimizedListModel::setModel instead";
    Q_ASSERT(false);
    QSortFilterProxyModel::setSourceModel(sourceModel);
}

void KSortFilterProxyRoleOptimizedListModel::setInterestingRoles(const QList<int> &roles)
{
    if (roles != d->m_helper.m_roles) {
        d->m_helper.m_roles = roles;
        invalidate();
    }
}
