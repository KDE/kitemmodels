/*
    SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
    SPDX-FileContributor: David Faure <david.faure@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "krearrangecolumnsproxymodel.h"

#include <algorithm>
#include <functional>

#if QT_VERSION < QT_VERSION_CHECK(7, 0, 0)
using BaseClass = QIdentityProxyModel;
#else
using BaseClass = QAbstractProxyModel;
#endif

class KRearrangeColumnsProxyModelPrivate
{
public:
    explicit KRearrangeColumnsProxyModelPrivate(KRearrangeColumnsProxyModel *qq)
        : q(qq)
    {
    }

    // Only the children of source column 0 are shown, under proxy column 0
    bool isShownParent(const QModelIndex &sourceParent) const
    {
        if (sourceParent.isValid() && m_sourceColumns.isEmpty()) {
            return false;
        }
        for (QModelIndex parent = sourceParent; parent.isValid(); parent = parent.parent()) {
            if (parent.column() != 0) {
                return false;
            }
        }
        return true;
    }

    // The parent of the children is in proxy column 0, whatever source column it shows
    QModelIndex mapParentFromSource(const QModelIndex &sourceParent) const
    {
        if (!sourceParent.isValid()) {
            return QModelIndex();
        }
        // Like index(), take the internal pointer of the source cell shown in proxy column 0
        return q->createIndex(sourceParent.row(), 0, sourceParent.siblingAtColumn(m_sourceColumns.at(0)).internalPointer());
    }

    // The children are under source column 0, whatever source column the parent shows
    QModelIndex mapParentToSource(const QModelIndex &proxyParent) const
    {
        return proxyParent.isValid() ? q->mapToSource(proxyParent).siblingAtColumn(0) : QModelIndex();
    }

    int mapDropColumnToSource(int proxyColumn) const
    {
        return proxyColumn >= 0 && proxyColumn < m_sourceColumns.size() ? m_sourceColumns.at(proxyColumn) : -1;
    }

    // A drop onto an item goes to the cell shown there, a drop between rows goes under source column 0
    QModelIndex mapDropParentToSource(int row, int column, const QModelIndex &parent) const
    {
        return row == -1 && column == -1 ? q->mapToSource(parent) : mapParentToSource(parent);
    }

    QList<QPersistentModelIndex> mapShownParentsFromSource(const QList<QPersistentModelIndex> &sourceParents) const
    {
        QList<QPersistentModelIndex> parents;
        for (const QPersistentModelIndex &sourceParent : sourceParents) {
            if (isShownParent(sourceParent)) {
                parents.append(mapParentFromSource(sourceParent));
            }
        }
        return parents;
    }

    // Returns the ranges of consecutive proxy columns showing source columns first..last
    QList<std::pair<int, int>> proxyColumnRanges(int first, int last) const
    {
        QList<std::pair<int, int>> ranges;
        for (int proxyColumn = 0; proxyColumn < m_sourceColumns.size(); ++proxyColumn) {
            const int sourceColumn = m_sourceColumns.at(proxyColumn);
            if (sourceColumn < first || sourceColumn > last) {
                continue;
            }
            if (!ranges.isEmpty() && ranges.last().second == proxyColumn - 1) {
                ranges.last().second = proxyColumn;
            } else {
                ranges.append({proxyColumn, proxyColumn});
            }
        }
        return ranges;
    }

    void beginReset()
    {
        Q_ASSERT(!m_resetting);
        m_resetting = true;
        q->beginResetModel();
    }

    void endReset()
    {
        Q_ASSERT(m_resetting);
        m_resetting = false;
        q->endResetModel();
    }

    void sourceRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
    {
        if (isShownParent(parent)) {
            q->beginInsertRows(mapParentFromSource(parent), first, last);
        }
    }

    void sourceRowsInserted(const QModelIndex &parent)
    {
        if (isShownParent(parent)) {
            q->endInsertRows();
        }
    }

    void sourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
    {
        if (isShownParent(parent)) {
            q->beginRemoveRows(mapParentFromSource(parent), first, last);
        }
    }

    void sourceRowsRemoved(const QModelIndex &parent)
    {
        if (isShownParent(parent)) {
            q->endRemoveRows();
        }
    }

    // A move from or to children that aren't shown is a plain insertion or removal in the proxy
    void sourceRowsAboutToBeMoved(const QModelIndex &sourceParent, int first, int last, const QModelIndex &destParent, int destRow)
    {
        const bool fromShown = isShownParent(sourceParent);
        const bool toShown = isShownParent(destParent);
        if (fromShown && toShown) {
            q->beginMoveRows(mapParentFromSource(sourceParent), first, last, mapParentFromSource(destParent), destRow);
        } else if (fromShown) {
            q->beginRemoveRows(mapParentFromSource(sourceParent), first, last);
        } else if (toShown) {
            q->beginInsertRows(mapParentFromSource(destParent), destRow, destRow + last - first);
        }
    }

    void sourceRowsMoved(const QModelIndex &sourceParent, int, int, const QModelIndex &destParent)
    {
        const bool fromShown = isShownParent(sourceParent);
        const bool toShown = isShownParent(destParent);
        if (fromShown && toShown) {
            q->endMoveRows();
        } else if (fromShown) {
            q->endRemoveRows();
        } else if (toShown) {
            q->endInsertRows();
        }
    }

    // m_sourceColumns holds source column numbers, so the proxy shows other data after
    // a change of the source columns at or before the last shown one
    bool changesShownColumns(int first) const
    {
        return !m_sourceColumns.isEmpty() && first <= *std::max_element(m_sourceColumns.cbegin(), m_sourceColumns.cend());
    }

    // The children come from source column 0, and the cells from the source column numbers
    // in m_sourceColumns, so a column change in a shown table resets the proxy
    void sourceColumnsAboutToBeInsertedOrRemoved(const QModelIndex &parent, int first)
    {
        if (isShownParent(parent) && changesShownColumns(first)) {
            beginReset();
        }
    }

    void sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent, int first, int, const QModelIndex &destParent, int destColumn)
    {
        if ((isShownParent(sourceParent) && changesShownColumns(first)) || (isShownParent(destParent) && changesShownColumns(destColumn))) {
            beginReset();
        }
    }

    // Column changes don't happen during a source reset, so m_resetting tells
    // whether the "about to" handler started a reset
    void sourceColumnsChanged()
    {
        if (m_resetting) {
            endReset();
        }
    }

    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
    {
        if (!isShownParent(topLeft.parent())) {
            return;
        }
        const auto ranges = proxyColumnRanges(topLeft.column(), bottomRight.column());
        for (const auto &[first, last] : ranges) {
            Q_EMIT q->dataChanged(q->mapFromSource(topLeft.siblingAtColumn(m_sourceColumns.at(first))),
                                  q->mapFromSource(bottomRight.siblingAtColumn(m_sourceColumns.at(last))),
                                  roles);
        }
    }

    void sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
    {
        if (orientation == Qt::Vertical) {
            Q_EMIT q->headerDataChanged(orientation, first, last);
            return;
        }
        const auto ranges = proxyColumnRanges(first, last);
        for (const auto &[proxyFirst, proxyLast] : ranges) {
            Q_EMIT q->headerDataChanged(orientation, proxyFirst, proxyLast);
        }
    }

    // A layout change of children that aren't shown is ignored
    void sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
    {
        const QList<QPersistentModelIndex> parents = mapShownParentsFromSource(sourceParents);
        m_ignoringLayoutChange = !sourceParents.isEmpty() && parents.isEmpty();
        if (m_ignoringLayoutChange) {
            return;
        }
        Q_EMIT q->layoutAboutToBeChanged(parents, hint);
        const QModelIndexList proxyIndexes = q->persistentIndexList();
        for (const QModelIndex &proxyIndex : proxyIndexes) {
            m_layoutChangeProxyIndexes.append(proxyIndex);
            m_layoutChangeSourceIndexes.append(q->mapToSource(proxyIndex));
        }
    }

    void sourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
    {
        if (m_ignoringLayoutChange) {
            m_ignoringLayoutChange = false;
            return;
        }
        const QList<QPersistentModelIndex> parents = mapShownParentsFromSource(sourceParents);
        for (int i = 0; i < m_layoutChangeProxyIndexes.size(); ++i) {
            q->changePersistentIndex(m_layoutChangeProxyIndexes.at(i), q->mapFromSource(m_layoutChangeSourceIndexes.at(i)));
        }
        m_layoutChangeProxyIndexes.clear();
        m_layoutChangeSourceIndexes.clear();
        Q_EMIT q->layoutChanged(parents, hint);
    }

    KRearrangeColumnsProxyModel *const q;
    QList<int> m_sourceColumns;
    bool m_resetting = false;
    bool m_ignoringLayoutChange = false;
    QList<QMetaObject::Connection> m_sourceConnections;
    QModelIndexList m_layoutChangeProxyIndexes;
    QList<QPersistentModelIndex> m_layoutChangeSourceIndexes;
};

KRearrangeColumnsProxyModel::KRearrangeColumnsProxyModel(QObject *parent)
    : BaseClass(parent)
    , d_ptr(new KRearrangeColumnsProxyModelPrivate(this))
{
}

KRearrangeColumnsProxyModel::~KRearrangeColumnsProxyModel()
{
}

void KRearrangeColumnsProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (sourceModel == this->sourceModel()) {
        return;
    }
    d_ptr->beginReset();
    for (const QMetaObject::Connection &connection : std::as_const(d_ptr->m_sourceConnections)) {
        disconnect(connection);
    }
    d_ptr->m_sourceConnections.clear();

#if QT_VERSION < QT_VERSION_CHECK(7, 0, 0)
    // Skip QIdentityProxyModel::setSourceModel: it forwards the source signals as if
    // proxy and source columns were the same, so we forward them ourselves below.
#endif
    QAbstractProxyModel::setSourceModel(sourceModel);

    if (sourceModel) {
        using D = KRearrangeColumnsProxyModelPrivate;
        D *d = d_ptr.get();
        d->m_sourceConnections = {
            connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, std::bind_front(&D::beginReset, d)),
            connect(sourceModel, &QAbstractItemModel::modelReset, this, std::bind_front(&D::endReset, d)),
            connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, std::bind_front(&D::sourceRowsAboutToBeInserted, d)),
            connect(sourceModel, &QAbstractItemModel::rowsInserted, this, std::bind_front(&D::sourceRowsInserted, d)),
            connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, std::bind_front(&D::sourceRowsAboutToBeRemoved, d)),
            connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, std::bind_front(&D::sourceRowsRemoved, d)),
            connect(sourceModel, &QAbstractItemModel::rowsAboutToBeMoved, this, std::bind_front(&D::sourceRowsAboutToBeMoved, d)),
            connect(sourceModel, &QAbstractItemModel::rowsMoved, this, std::bind_front(&D::sourceRowsMoved, d)),
            connect(sourceModel, &QAbstractItemModel::columnsAboutToBeInserted, this, std::bind_front(&D::sourceColumnsAboutToBeInsertedOrRemoved, d)),
            connect(sourceModel, &QAbstractItemModel::columnsInserted, this, std::bind_front(&D::sourceColumnsChanged, d)),
            connect(sourceModel, &QAbstractItemModel::columnsAboutToBeRemoved, this, std::bind_front(&D::sourceColumnsAboutToBeInsertedOrRemoved, d)),
            connect(sourceModel, &QAbstractItemModel::columnsRemoved, this, std::bind_front(&D::sourceColumnsChanged, d)),
            connect(sourceModel, &QAbstractItemModel::columnsAboutToBeMoved, this, std::bind_front(&D::sourceColumnsAboutToBeMoved, d)),
            connect(sourceModel, &QAbstractItemModel::columnsMoved, this, std::bind_front(&D::sourceColumnsChanged, d)),
            connect(sourceModel, &QAbstractItemModel::headerDataChanged, this, std::bind_front(&D::sourceHeaderDataChanged, d)),
        };
#if QT_VERSION < QT_VERSION_CHECK(7, 0, 0)
        const bool handleDataChanges = handleSourceDataChanges();
        const bool handleLayoutChanges = handleSourceLayoutChanges();
#else
        const bool handleDataChanges = true;
        const bool handleLayoutChanges = true;
#endif
        if (handleDataChanges) {
            d->m_sourceConnections << connect(sourceModel, &QAbstractItemModel::dataChanged, this, std::bind_front(&D::sourceDataChanged, d));
        }
        if (handleLayoutChanges) {
            d->m_sourceConnections << connect(sourceModel,
                                              &QAbstractItemModel::layoutAboutToBeChanged,
                                              this,
                                              std::bind_front(&D::sourceLayoutAboutToBeChanged, d))
                                   << connect(sourceModel, &QAbstractItemModel::layoutChanged, this, std::bind_front(&D::sourceLayoutChanged, d));
        }
    }

    d_ptr->endReset();
}

void KRearrangeColumnsProxyModel::setSourceColumns(const QList<int> &columns)
{
    // We could use layoutChanged() here, but we would have to map persistent
    // indexes from the old to the new location...
    if (!d_ptr->m_resetting)
        beginResetModel();
    d_ptr->m_sourceColumns = columns;
    if (!d_ptr->m_resetting)
        endResetModel();
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
    if (parent.column() > 0) {
        return 0;
    }
    const QModelIndex sourceParent = d_ptr->mapParentToSource(parent);
    return sourceModel()->rowCount(sourceParent);
}

bool KRearrangeColumnsProxyModel::hasChildren(const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    if (!sourceModel()) {
        return false;
    }
    if (d_ptr->m_sourceColumns.isEmpty()) { // no columns configured yet
        return false;
    }
    if (parent.column() > 0) {
        return false;
    }
    const QModelIndex sourceParent = d_ptr->mapParentToSource(parent);
    return sourceModel()->rowCount(sourceParent) > 0;
}

QModelIndex KRearrangeColumnsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_ASSERT(parent.isValid() ? parent.model() == this : true);
    Q_ASSERT(row >= 0);
    Q_ASSERT(column >= 0);

    // Only first column has children
    if (parent.column() > 0) {
        return {};
    }

    if (!sourceModel()) {
        return {};
    }
    if (d_ptr->m_sourceColumns.isEmpty()) {
        return {};
    }

    const QModelIndex sourceParent = d_ptr->mapParentToSource(parent);

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
    return d_ptr->mapParentFromSource(mapToSource(child).parent());
}

QVariant KRearrangeColumnsProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (!sourceModel() || section >= d_ptr->m_sourceColumns.count()) {
            return QVariant();
        }
        const int sourceCol = sourceColumnForProxyColumn(section);
        return sourceModel()->headerData(sourceCol, orientation, role);
    } else {
        return BaseClass::headerData(section, orientation, role);
    }
}

bool KRearrangeColumnsProxyModel::insertRows(int row, int count, const QModelIndex &parent)
{
    // Like rowCount(), only proxy column 0 has children
    return sourceModel() && parent.column() <= 0 && sourceModel()->insertRows(row, count, d_ptr->mapParentToSource(parent));
}

bool KRearrangeColumnsProxyModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return sourceModel() && parent.column() <= 0 && sourceModel()->removeRows(row, count, d_ptr->mapParentToSource(parent));
}

bool KRearrangeColumnsProxyModel::moveRows(const QModelIndex &sourceParent,
                                           int sourceRow,
                                           int count,
                                           const QModelIndex &destinationParent,
                                           int destinationChild)
{
    return sourceModel() && sourceParent.column() <= 0 && destinationParent.column() <= 0
        && sourceModel()->moveRows(d_ptr->mapParentToSource(sourceParent), sourceRow, count, d_ptr->mapParentToSource(destinationParent), destinationChild);
}

bool KRearrangeColumnsProxyModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    return sourceModel()
        && sourceModel()->canDropMimeData(data, action, row, d_ptr->mapDropColumnToSource(column), d_ptr->mapDropParentToSource(row, column, parent));
}

bool KRearrangeColumnsProxyModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    return sourceModel()
        && sourceModel()->dropMimeData(data, action, row, d_ptr->mapDropColumnToSource(column), d_ptr->mapDropParentToSource(row, column, parent));
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
    return createSourceIndex(proxyIndex.row(), sourceColumnForProxyColumn(proxyIndex.column()), proxyIndex.internalPointer());
}

int KRearrangeColumnsProxyModel::proxyColumnForSourceColumn(int sourceColumn) const
{
    // If this is too slow, we could add a second QList with index=logical_source_column value=desired_pos_in_proxy.
    return d_ptr->m_sourceColumns.indexOf(sourceColumn);
}

int KRearrangeColumnsProxyModel::sourceColumnForProxyColumn(int proxyColumn) const
{
    Q_ASSERT(proxyColumn >= 0);
    Q_ASSERT(proxyColumn < d_ptr->m_sourceColumns.size());
    return d_ptr->m_sourceColumns.at(proxyColumn);
}

#include "moc_krearrangecolumnsproxymodel.cpp"
