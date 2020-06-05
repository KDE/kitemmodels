/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kdescendantsproxymodel_qml.h"
#include <QDebug>

KDescendantsProxyModelQml::KDescendantsProxyModelQml(QObject *parent):
    KDescendantsProxyModel(parent)
{
    connect(this, &KDescendantsProxyModel::sourceIndexExpanded,
            this, [this] (const QModelIndex &sourceIndex) {
        if (m_expandableRole < 0) {
            return;
        }
        const QModelIndex index = mapFromSource(sourceIndex);
        emit dataChanged(index, index, {m_expandedRole});
    });

    connect(this, &KDescendantsProxyModel::sourceIndexCollapsed,
            this, [this] (const QModelIndex &sourceIndex) {
        if (m_expandableRole < 0) {
            return;
        }
        const QModelIndex index = mapFromSource(sourceIndex);
        emit dataChanged(index, index, {m_expandedRole});
    });

    connect(this, &KDescendantsProxyModel::sourceModelChanged,
            this, [this] {
        if (m_sourceModel) {
            disconnect(m_sourceModel, nullptr, this, nullptr);
        }
        m_sourceModel = sourceModel();

        if (!m_sourceModel) {
            return;
        }

        connect(m_sourceModel, &QAbstractItemModel::rowsInserted,
                this, [this] (const QModelIndex &parent, int first, int last) {
            Q_UNUSED(first)
            Q_UNUSED(last)
            const QModelIndex index = mapFromSource(parent);
            emit dataChanged(index, index, {m_expandableRole});
        });

        connect(m_sourceModel, &QAbstractItemModel::rowsRemoved,
                this, [this] (const QModelIndex &parent, int first, int last) {
            Q_UNUSED(first)
            Q_UNUSED(last)
            const QModelIndex index = mapFromSource(parent);
            emit dataChanged(index, index, {m_expandableRole});
            if (!sourceModel()->hasChildren(parent)) {
                emit dataChanged(index, index, {m_expandedRole});
            }
        });

        connect(m_sourceModel, &QAbstractItemModel::rowsMoved,
                this, [this] (const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) {
            Q_UNUSED(start)
            Q_UNUSED(end)
            Q_UNUSED(row)
            const QModelIndex index1 = mapFromSource(parent);
            const QModelIndex index2 = mapFromSource(destination);
            emit dataChanged(index1, index1, {m_expandableRole});
            if (index1 != index2) {
                emit dataChanged(index2, index2, {m_expandableRole});
                if (!sourceModel()->hasChildren(destination)) {
                    emit dataChanged(index2, index2, {m_expandedRole});
                }
            }
        });
    });
}

KDescendantsProxyModelQml::~KDescendantsProxyModelQml()
{}

QHash<int, QByteArray> KDescendantsProxyModelQml::roleNames() const
{
    QHash<int, QByteArray> roleNames = KDescendantsProxyModel::roleNames();
    const auto &keys = roleNames.keys();
    const int max = *std::max_element(keys.constBegin(), keys.constEnd());

    const_cast<KDescendantsProxyModelQml *>(this)->m_levelRole = max + 1;
    const_cast<KDescendantsProxyModelQml *>(this)->m_expandableRole = max + 2;
    const_cast<KDescendantsProxyModelQml *>(this)->m_expandedRole = max + 3;

    roleNames[m_levelRole] = "kDescendantLevel";
    roleNames[m_expandableRole] = "kDescendantExpandable";
    roleNames[m_expandedRole] = "kDescendantExpanded";
    return roleNames;
}

QVariant KDescendantsProxyModelQml::data(const QModelIndex &index, int role) const
{
    if (role == m_levelRole) {
        QModelIndex sourceIndex = mapToSource(index);
        int level = 0;
        while (sourceIndex.isValid()) {
            sourceIndex = sourceIndex.parent();
            ++level;
        }
        return level;
    } else if (role == m_expandableRole) {
        QModelIndex sourceIndex = mapToSource(index);
        return sourceModel()->hasChildren(sourceIndex);
    } else if (role == m_expandedRole) {
        return isSourceIndexExpanded(mapToSource(index));
    } else {
        return KDescendantsProxyModel::data(index, role);
    }
}

void KDescendantsProxyModelQml::toggleChild(int row)
{
    QModelIndex sourceIndex = mapToSource(index(row, 0));

    if (!sourceModel()->hasChildren(sourceIndex)) {
        return;
    }

    if (isSourceIndexExpanded(sourceIndex)) {
        collapseChild(row);
    } else {
        expandChild(row);
    }
}

#include "moc_kdescendantsproxymodel_qml.cpp"
