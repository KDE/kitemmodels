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

#include "kconcatenaterowsproxymodel.h"

class KConcatenateRowsProxyModelPrivate
{
public:
    KConcatenateRowsProxyModelPrivate(KConcatenateRowsProxyModel* model)
        : q(model)
    {}

    int computeRowsPrior(const QAbstractItemModel *sourceModel) const;
    QAbstractItemModel *sourceModelForRow(int row, int *sourceRow) const;

    void slotRowsAboutToBeInserted(const QModelIndex &, int start, int end);
    void slotRowsInserted(const QModelIndex &, int start, int end);
    void slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end);
    void slotRowsRemoved(const QModelIndex &, int start, int end);
    void slotColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end);
    void slotColumnsInserted(const QModelIndex &parent, int, int);
    void slotColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    void slotColumnsRemoved(const QModelIndex &parent, int, int);
    void slotDataChanged(const QModelIndex &from, const QModelIndex &to, const QVector<int> &roles);
    void slotSourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
    void slotSourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
    void slotModelAboutToBeReset();
    void slotModelReset();

    KConcatenateRowsProxyModel *q;
    QList<QAbstractItemModel *> m_models;
    int m_rowCount = 0; // have to maintain it here since we can't compute during model destruction

    // for layoutAboutToBeChanged/layoutChanged
    QVector<QPersistentModelIndex> layoutChangePersistentIndexes;
    QModelIndexList proxyIndexes;
};

KConcatenateRowsProxyModel::KConcatenateRowsProxyModel(QObject *parent)
    : QAbstractItemModel(parent),
      d(new KConcatenateRowsProxyModelPrivate(this))
{
}

KConcatenateRowsProxyModel::~KConcatenateRowsProxyModel()
{
}

QModelIndex KConcatenateRowsProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    const QAbstractItemModel *sourceModel = sourceIndex.model();
    if (!sourceModel) {
        return {};
    }
    int rowsPrior = d->computeRowsPrior(sourceModel);
    return createIndex(rowsPrior + sourceIndex.row(), sourceIndex.column());
}

QModelIndex KConcatenateRowsProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }
    const int row = proxyIndex.row();
    int sourceRow;
    QAbstractItemModel *sourceModel = d->sourceModelForRow(row, &sourceRow);
    if (!sourceModel) {
        return QModelIndex();
    }
    return sourceModel->index(sourceRow, proxyIndex.column());
}

QVariant KConcatenateRowsProxyModel::data(const QModelIndex &index, int role) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (!sourceIndex.isValid()) {
        return QVariant();
    }
    return sourceIndex.model()->data(sourceIndex, role);
}

bool KConcatenateRowsProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const QModelIndex sourceIndex = mapToSource(index);
    if (!sourceIndex.isValid()) {
        return false;
    }
    QAbstractItemModel *sourceModel = const_cast<QAbstractItemModel *>(sourceIndex.model());
    return sourceModel->setData(sourceIndex, value, role);
}

QMap<int, QVariant> KConcatenateRowsProxyModel::itemData(const QModelIndex &proxyIndex) const
{
    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    if (!sourceIndex.isValid()) {
        return {};
    }
    return sourceIndex.model()->itemData(sourceIndex);
}

Qt::ItemFlags KConcatenateRowsProxyModel::flags(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    return sourceIndex.isValid() ? sourceIndex.model()->flags(sourceIndex) : Qt::ItemFlags();
}

QVariant KConcatenateRowsProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (d->m_models.isEmpty()) {
        return QVariant();
    }
    if (orientation == Qt::Horizontal) {
        return d->m_models.at(0)->headerData(section, orientation, role);
    } else {
        int sourceRow;
        QAbstractItemModel *sourceModel = d->sourceModelForRow(section, &sourceRow);
        if (!sourceModel) {
            return QVariant();
        }
        return sourceModel->headerData(sourceRow, orientation, role);
    }
}

int KConcatenateRowsProxyModel::columnCount(const QModelIndex &parent) const
{
    if (d->m_models.isEmpty()) {
        return 0;
    }
    if (parent.isValid()) {
        return 0; // flat model;
    }
    return d->m_models.at(0)->columnCount(QModelIndex());
}

QHash<int, QByteArray> KConcatenateRowsProxyModel::roleNames() const
{
    if (d->m_models.isEmpty()) {
        return {};
    }
    return d->m_models.at(0)->roleNames();
}

QModelIndex KConcatenateRowsProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0) {
        return {};
    }
    if(column < 0) {
        return {};
    }
    int sourceRow;
    QAbstractItemModel *sourceModel = d->sourceModelForRow(row, &sourceRow);
    if (!sourceModel) {
        return QModelIndex();
    }
    return mapFromSource(sourceModel->index(sourceRow, column, parent));
}

QModelIndex KConcatenateRowsProxyModel::parent(const QModelIndex &) const
{
    return QModelIndex(); // we are flat, no hierarchy
}

int KConcatenateRowsProxyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;    // flat model
    }

    return d->m_rowCount;
}

void KConcatenateRowsProxyModel::addSourceModel(QAbstractItemModel *sourceModel)
{
    Q_ASSERT(sourceModel);
    Q_ASSERT(!d->m_models.contains(sourceModel));
    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(slotDataChanged(QModelIndex,QModelIndex,QVector<int>)));
    connect(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)), this, SLOT(slotRowsAboutToBeInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(slotRowsAboutToBeRemoved(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(columnsInserted(QModelIndex,int,int)), this, SLOT(slotColumnsInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(columnsRemoved(QModelIndex,int,int)), this, SLOT(slotColumnsRemoved(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)), this, SLOT(slotColumnsAboutToBeInserted(QModelIndex,int,int)));
    connect(sourceModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(slotColumnsAboutToBeRemoved(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(layoutAboutToBeChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),
            this, SLOT(slotSourceLayoutAboutToBeChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)));
    connect(sourceModel, SIGNAL(layoutChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)),
            this, SLOT(slotSourceLayoutChanged(QList<QPersistentModelIndex>,QAbstractItemModel::LayoutChangeHint)));
    connect(sourceModel, SIGNAL(modelAboutToBeReset()), this, SLOT(slotModelAboutToBeReset()));
    connect(sourceModel, SIGNAL(modelReset()), this, SLOT(slotModelReset()));

    const int newRows = sourceModel->rowCount();
    if (newRows > 0) {
        beginInsertRows(QModelIndex(), d->m_rowCount, d->m_rowCount + newRows - 1);
    }
    d->m_rowCount += newRows;
    d->m_models.append(sourceModel);
    if (newRows > 0) {
        endInsertRows();
    }
}

QList<QAbstractItemModel*> KConcatenateRowsProxyModel::sources() const
{
    return d->m_models;
}



void KConcatenateRowsProxyModel::removeSourceModel(QAbstractItemModel *sourceModel)
{
    Q_ASSERT(d->m_models.contains(sourceModel));
    disconnect(sourceModel, nullptr, this, nullptr);

    const int rowsRemoved = sourceModel->rowCount();
    const int rowsPrior = d->computeRowsPrior(sourceModel);   // location of removed section

    if (rowsRemoved > 0) {
        beginRemoveRows(QModelIndex(), rowsPrior, rowsPrior + rowsRemoved - 1);
    }
    d->m_models.removeOne(sourceModel);
    d->m_rowCount -= rowsRemoved;
    if (rowsRemoved > 0) {
        endRemoveRows();
    }
}

void KConcatenateRowsProxyModelPrivate::slotRowsAboutToBeInserted(const QModelIndex &, int start, int end)
{
    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    const int rowsPrior = computeRowsPrior(model);
    q->beginInsertRows(QModelIndex(), rowsPrior + start, rowsPrior + end);
}

void KConcatenateRowsProxyModelPrivate::slotRowsInserted(const QModelIndex &, int start, int end)
{
    m_rowCount += end - start + 1;
    q->endInsertRows();
}

void KConcatenateRowsProxyModelPrivate::slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end)
{
    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    const int rowsPrior = computeRowsPrior(model);
    q->beginRemoveRows(QModelIndex(), rowsPrior + start, rowsPrior + end);
}

void KConcatenateRowsProxyModelPrivate::slotRowsRemoved(const QModelIndex &, int start, int end)
{
    m_rowCount -= end - start + 1;
    q->endRemoveRows();
}

void KConcatenateRowsProxyModelPrivate::slotColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    if (parent.isValid()) { // we are flat
        return;
    }
    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    if (m_models.at(0) == model) {
        q->beginInsertColumns(QModelIndex(), start, end);
    }
}

void KConcatenateRowsProxyModelPrivate::slotColumnsInserted(const QModelIndex &parent, int, int)
{
    if (parent.isValid()) { // we are flat
        return;
    }
    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    if (m_models.at(0) == model) {
        q->endInsertColumns();
    }
}

void KConcatenateRowsProxyModelPrivate::slotColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    if (parent.isValid()) { // we are flat
        return;
    }
    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    if (m_models.at(0) == model) {
        q->beginRemoveColumns(QModelIndex(), start, end);
    }
}

void KConcatenateRowsProxyModelPrivate::slotColumnsRemoved(const QModelIndex &parent, int, int)
{
    if (parent.isValid()) { // we are flat
        return;
    }
    const QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(q->sender());
    if (m_models.at(0) == model) {
        q->endRemoveColumns();
    }
}

void KConcatenateRowsProxyModelPrivate::slotDataChanged(const QModelIndex &from, const QModelIndex &to, const QVector<int> &roles)
{
    if (!from.isValid()) { // QSFPM bug, it emits dataChanged(invalid, invalid) if a cell in a hidden column changes
        return;
    }
    const QModelIndex myFrom = q->mapFromSource(from);
    const QModelIndex myTo = q->mapFromSource(to);
    emit q->dataChanged(myFrom, myTo, roles);
}

void KConcatenateRowsProxyModelPrivate::slotSourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    QList<QPersistentModelIndex> parents;
    parents.reserve(sourceParents.size());
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid()) {
            parents << QPersistentModelIndex();
            continue;
        }
        const QModelIndex mappedParent = q->mapFromSource(parent);
        Q_ASSERT(mappedParent.isValid());
        parents << mappedParent;
    }

    emit q->layoutAboutToBeChanged(parents, hint);

    const QModelIndexList persistentIndexList = q->persistentIndexList();
    layoutChangePersistentIndexes.reserve(persistentIndexList.size());

    for (const QPersistentModelIndex &proxyPersistentIndex : persistentIndexList) {
        proxyIndexes << proxyPersistentIndex;
        Q_ASSERT(proxyPersistentIndex.isValid());
        const QPersistentModelIndex srcPersistentIndex = q->mapToSource(proxyPersistentIndex);
        Q_ASSERT(srcPersistentIndex.isValid());
        layoutChangePersistentIndexes << srcPersistentIndex;
    }
}

void KConcatenateRowsProxyModelPrivate::slotSourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint)
{
    for (int i = 0; i < proxyIndexes.size(); ++i) {
        const QModelIndex proxyIdx = proxyIndexes.at(i);
        QModelIndex newProxyIdx = q->mapFromSource(layoutChangePersistentIndexes.at(i));
        q->changePersistentIndex(proxyIdx, newProxyIdx);
    }

    layoutChangePersistentIndexes.clear();
    proxyIndexes.clear();

    QList<QPersistentModelIndex> parents;
    parents.reserve(sourceParents.size());
    for (const QPersistentModelIndex &parent : sourceParents) {
        if (!parent.isValid()) {
            parents << QPersistentModelIndex();
            continue;
        }
        const QModelIndex mappedParent = q->mapFromSource(parent);
        Q_ASSERT(mappedParent.isValid());
        parents << mappedParent;
    }
    emit q->layoutChanged(parents, hint);
}

void KConcatenateRowsProxyModelPrivate::slotModelAboutToBeReset()
{
        const QAbstractItemModel *sourceModel = qobject_cast<const QAbstractItemModel *>(q->sender());
    Q_ASSERT(m_models.contains(const_cast<QAbstractItemModel *>(sourceModel)));
    const int oldRows = sourceModel->rowCount();
    if (oldRows > 0) {
        slotRowsAboutToBeRemoved(QModelIndex(), 0, oldRows - 1);
        slotRowsRemoved(QModelIndex(), 0, oldRows - 1);
    }
    if (m_models.at(0) == sourceModel) {
        q->beginResetModel();
    }
}

void KConcatenateRowsProxyModelPrivate::slotModelReset()
{
    const QAbstractItemModel *sourceModel = qobject_cast<const QAbstractItemModel *>(q->sender());
    Q_ASSERT(m_models.contains(const_cast<QAbstractItemModel *>(sourceModel)));
    if (m_models.at(0) == sourceModel) {
        q->endResetModel();
    }
    const int newRows = sourceModel->rowCount();
    if (newRows > 0) {
        slotRowsAboutToBeInserted(QModelIndex(), 0, newRows - 1);
        slotRowsInserted(QModelIndex(), 0, newRows - 1);
    }
}

int KConcatenateRowsProxyModelPrivate::computeRowsPrior(const QAbstractItemModel *sourceModel) const
{
    int rowsPrior = 0;
    for (const QAbstractItemModel *model : qAsConst(m_models)) {
        if (model == sourceModel) {
            break;
        }
        rowsPrior += model->rowCount();
    }
    return rowsPrior;
}

QAbstractItemModel *KConcatenateRowsProxyModelPrivate::sourceModelForRow(int row, int *sourceRow) const
{
    int rowCount = 0;
    QAbstractItemModel *selection = nullptr;
    for (QAbstractItemModel *model : qAsConst(m_models)) {
        const int subRowCount = model->rowCount();
        if (rowCount + subRowCount > row) {
            selection = model;
            break;
        }
        rowCount += subRowCount;
    }
    *sourceRow = row - rowCount;
    return selection;
}

#include "moc_kconcatenaterowsproxymodel.cpp"
