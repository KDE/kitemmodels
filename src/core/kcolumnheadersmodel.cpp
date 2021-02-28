/*
    SPDX-FileCopyrightText: 2019 Arjen Hiemstra <ahiemstra@heimr.nl>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kcolumnheadersmodel.h"

class KColumnHeadersModelPrivate
{
public:
    QAbstractItemModel *sourceModel = nullptr;
};

KColumnHeadersModel::KColumnHeadersModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new KColumnHeadersModelPrivate)
{
}

KColumnHeadersModel::~KColumnHeadersModel()
{
}

int KColumnHeadersModel::rowCount(const QModelIndex &parent) const
{
    if (!d->sourceModel || parent.isValid()) {
        return 0;
    }

    return d->sourceModel->columnCount();
}

QVariant KColumnHeadersModel::data(const QModelIndex &index, int role) const
{
    if (!d->sourceModel || !index.isValid()) {
        return QVariant{};
    }

    return sourceModel()->headerData(index.row(), Qt::Horizontal, role);
}

QHash<int, QByteArray> KColumnHeadersModel::roleNames() const
{
    if (!d->sourceModel) {
        return QHash<int, QByteArray>{};
    }

    return d->sourceModel->roleNames();
}

QAbstractItemModel *KColumnHeadersModel::sourceModel() const
{
    return d->sourceModel;
}

void KColumnHeadersModel::setSourceModel(QAbstractItemModel *newSourceModel)
{
    if (newSourceModel == d->sourceModel) {
        return;
    }

    if (d->sourceModel) {
        d->sourceModel->disconnect(this);
    }

    beginResetModel();
    d->sourceModel = newSourceModel;
    endResetModel();

    if (newSourceModel) {
        connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeInserted, this, [this](const QModelIndex &, int first, int last) {
            beginInsertRows(QModelIndex{}, first, last);
        });
        connect(newSourceModel, &QAbstractItemModel::columnsInserted, this, [this]() {
            endInsertRows();
        });
        connect(newSourceModel,
                &QAbstractItemModel::columnsAboutToBeMoved,
                this,
                [this](const QModelIndex &, int start, int end, const QModelIndex &, int destination) {
                    beginMoveRows(QModelIndex{}, start, end, QModelIndex{}, destination);
                });
        connect(newSourceModel, &QAbstractItemModel::columnsMoved, this, [this]() {
            endMoveRows();
        });
        connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeRemoved, this, [this](const QModelIndex &, int first, int last) {
            beginRemoveRows(QModelIndex{}, first, last);
        });
        connect(newSourceModel, &QAbstractItemModel::columnsRemoved, this, [this]() {
            endRemoveRows();
        });
        connect(newSourceModel, &QAbstractItemModel::headerDataChanged, this, [this](Qt::Orientation orientation, int first, int last) {
            if (orientation == Qt::Horizontal) {
                Q_EMIT dataChanged(index(first, 0), index(last, 0));
            }
        });
        connect(newSourceModel, &QAbstractItemModel::layoutAboutToBeChanged, this, &QAbstractItemModel::layoutAboutToBeChanged);
        connect(newSourceModel, &QAbstractItemModel::layoutChanged, this, &QAbstractItemModel::layoutChanged);
        connect(newSourceModel, &QAbstractItemModel::modelAboutToBeReset, this, [this]() {
            beginResetModel();
        });
        connect(newSourceModel, &QAbstractItemModel::modelReset, this, [this]() {
            endResetModel();
        });
    }
}
