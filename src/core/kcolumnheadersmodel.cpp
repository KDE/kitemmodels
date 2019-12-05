/*
    Copyright (c) 2019 Arjen Hiemstra <ahiemstra@heimr.nl>

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

#include "kcolumnheadersmodel.h"

class KColumnHeadersModelPrivate
{
public:
    QAbstractItemModel *sourceModel = nullptr;
};

KColumnHeadersModel::KColumnHeadersModel(QObject *parent)
    : QAbstractListModel(parent), d(new KColumnHeadersModelPrivate)
{
}

KColumnHeadersModel::~KColumnHeadersModel()
{
}

int KColumnHeadersModel::rowCount(const QModelIndex& parent) const
{
    if (!d->sourceModel || parent.isValid()) {
        return 0;
    }

    return d->sourceModel->columnCount();
}

QVariant KColumnHeadersModel::data(const QModelIndex& index, int role) const
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

void KColumnHeadersModel::setSourceModel(QAbstractItemModel* newSourceModel)
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
        connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeInserted, this, [this](const QModelIndex&, int first, int last) {
            beginInsertRows(QModelIndex{}, first, last);
        });
        connect(newSourceModel, &QAbstractItemModel::columnsInserted, this, [this]() { endInsertRows(); });
        connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeMoved, this, [this](const QModelIndex&, int start, int end, const QModelIndex&, int destination) {
            beginMoveRows(QModelIndex{}, start, end, QModelIndex{}, destination);
        });
        connect(newSourceModel, &QAbstractItemModel::columnsMoved, this, [this]() { endMoveRows(); });
        connect(newSourceModel, &QAbstractItemModel::columnsAboutToBeRemoved, this, [this](const QModelIndex &, int first, int last) {
            beginRemoveRows(QModelIndex{}, first, last);
        });
        connect(newSourceModel, &QAbstractItemModel::columnsRemoved, this, [this]() { endRemoveRows(); });
        connect(newSourceModel, &QAbstractItemModel::headerDataChanged, this, [this](Qt::Orientation orientation, int first, int last) {
            if (orientation == Qt::Horizontal) {
                Q_EMIT dataChanged(index(first, 0), index(last, 0));
            }
        });
        connect(newSourceModel, &QAbstractItemModel::layoutAboutToBeChanged, this, &QAbstractItemModel::layoutAboutToBeChanged);
        connect(newSourceModel, &QAbstractItemModel::layoutChanged, this, &QAbstractItemModel::layoutChanged);
        connect(newSourceModel, &QAbstractItemModel::modelAboutToBeReset, this, [this]() { beginResetModel(); });
        connect(newSourceModel, &QAbstractItemModel::modelReset, this, [this]() { endResetModel(); });
    }
}
