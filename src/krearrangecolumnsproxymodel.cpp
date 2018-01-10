/*
    Copyright (c) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Authors: David Faure <david.faure@kdab.com>

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

#include "krearrangecolumnsproxymodel.h"

class KRearrangeColumnsProxyModelPrivate
{
public:
    QVector<int> m_sourceColumns;
};

KRearrangeColumnsProxyModel::KRearrangeColumnsProxyModel(QObject *parent)
    : QIdentityProxyModel(parent),
      d_ptr(new KRearrangeColumnsProxyModelPrivate)
{
}

KRearrangeColumnsProxyModel::~KRearrangeColumnsProxyModel()
{
}

void KRearrangeColumnsProxyModel::setSourceColumns(const QVector<int> &columns)
{
    d_ptr->m_sourceColumns = columns;
}

int KRearrangeColumnsProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (!sourceModel()) {
        return 0;
    }
    return d_ptr->m_sourceColumns.count();
}

int KRearrangeColumnsProxyModel::rowCount(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    if (!sourceModel()) {
        return 0;
    }
    // The parent in the source model is on column 0, whatever swapping we are doing
    const QModelIndex sourceParent = mapToSource(parent).sibling(parent.row(), 0);
    return sourceModel()->rowCount(sourceParent);
}

// We derive from QIdentityProxyModel simply to be able to use
// its mapFromSource method which has friend access to createIndex() in the source model.

QModelIndex KRearrangeColumnsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_ASSERT(row >= 0);
    Q_ASSERT(column >= 0);

    // The parent in the source model is on column 0, whatever swapping we are doing
    const QModelIndex sourceParent = mapToSource(parent).sibling(parent.row(), 0);

    // Find the child in the source model, we need its internal pointer
    const QModelIndex sourceIndex = sourceModel()->index(row, sourceColumnForProxyColumn(column), sourceParent);
    if (!sourceIndex.isValid()) {
        return QModelIndex();
    }

    return createIndex(row, column, sourceIndex.internalPointer());
}

QModelIndex KRearrangeColumnsProxyModel::parent(const QModelIndex &child) const
{
    Q_ASSERT(child.isValid() ? child.model() == this : true);
    const QModelIndex sourceIndex = mapToSource(child);
    const QModelIndex sourceParent = sourceIndex.parent();
    if (!sourceParent.isValid()) {
        return QModelIndex();
    }
    return createIndex(sourceParent.row(), 0, sourceParent.internalPointer());
}

QVariant KRearrangeColumnsProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        const int sourceCol = sourceColumnForProxyColumn(section);
        return sourceModel()->headerData(sourceCol, orientation, role);
    } else {
        return QIdentityProxyModel::headerData(section, orientation, role);
    }
}

QModelIndex KRearrangeColumnsProxyModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (column >= d_ptr->m_sourceColumns.count()) {
        return QModelIndex();
    }
    return index(row, column, idx.parent());
}

QModelIndex KRearrangeColumnsProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid()) {
        return QModelIndex();
    }
    Q_ASSERT(sourceIndex.model() == sourceModel());
    const int proxyColumn = proxyColumnForSourceColumn(sourceIndex.column());
    return createIndex(sourceIndex.row(), proxyColumn, sourceIndex.internalPointer());
}

QModelIndex KRearrangeColumnsProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }
    // This is just an indirect way to call sourceModel->createIndex(row, sourceColumn, pointer)
    const QModelIndex fakeIndex = createIndex(proxyIndex.row(), sourceColumnForProxyColumn(proxyIndex.column()), proxyIndex.internalPointer());
    return QIdentityProxyModel::mapToSource(fakeIndex);
}

int KRearrangeColumnsProxyModel::proxyColumnForSourceColumn(int sourceColumn) const
{
    // If this is too slow, we could add a second QVector with index=logical_source_column value=desired_pos_in_proxy.
    return d_ptr->m_sourceColumns.indexOf(sourceColumn);
}

int KRearrangeColumnsProxyModel::sourceColumnForProxyColumn(int proxyColumn) const
{
    Q_ASSERT(proxyColumn >= 0);
    Q_ASSERT(proxyColumn < d_ptr->m_sourceColumns.size());
    return d_ptr->m_sourceColumns.at(proxyColumn);
}
