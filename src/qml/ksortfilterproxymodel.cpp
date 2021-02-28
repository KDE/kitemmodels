/*
 *   Copyright 2010 by Marco Martin <mart@kde.org>
 *   Copyright 2019 by David Edmundson <davidedmundson@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "ksortfilterproxymodel.h"

#include <QQmlContext>
#include <QQmlEngine>

#include "kitemmodels_debug.h"

KSortFilterProxyModel::KSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    connect(this, &KSortFilterProxyModel::modelReset, this, &KSortFilterProxyModel::rowCountChanged);
    connect(this, &KSortFilterProxyModel::rowsInserted, this, &KSortFilterProxyModel::rowCountChanged);
    connect(this, &KSortFilterProxyModel::rowsRemoved, this, &KSortFilterProxyModel::rowCountChanged);
}

KSortFilterProxyModel::~KSortFilterProxyModel()
{
}

void KSortFilterProxyModel::syncRoleNames()
{
    if (!sourceModel()) {
        return;
    }

    m_roleIds.clear();
    const QHash<int, QByteArray> rNames = roleNames();
    m_roleIds.reserve(rNames.count());
    for (auto i = rNames.constBegin(); i != rNames.constEnd(); ++i) {
        m_roleIds[QString::fromUtf8(i.value())] = i.key();
    }
}

int KSortFilterProxyModel::roleNameToId(const QString &name) const
{
    return m_roleIds.value(name, Qt::DisplayRole);
}

void KSortFilterProxyModel::setModel(QAbstractItemModel *model)
{
    if (model == sourceModel()) {
        return;
    }

    QSortFilterProxyModel::setSourceModel(model);
    if (m_componentCompleted) {
        syncRoleNames();
        setFilterRole(m_filterRole);
        setSortRole(m_sortRole);
    }
}

bool KSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_filterRowCallback.isCallable()) {
        QJSEngine *engine = qjsEngine(this);
        QJSValueList args = {QJSValue(source_row), engine->toScriptValue(source_parent)};

        QJSValue result = const_cast<KSortFilterProxyModel *>(this)->m_filterRowCallback.call(args);
        if (result.isError()) {
            qCWarning(KITEMMODELS_LOG) << "Row filter callback produced an error:";
            qCWarning(KITEMMODELS_LOG) << result.toString();
            return true;
        } else {
            return result.toBool();
        }
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool KSortFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    if (m_filterColumnCallback.isCallable()) {
        QJSEngine *engine = qjsEngine(this);
        QJSValueList args = {QJSValue(source_column), engine->toScriptValue(source_parent)};

        QJSValue result = const_cast<KSortFilterProxyModel *>(this)->m_filterColumnCallback.call(args);
        if (result.isError()) {
            qCWarning(KITEMMODELS_LOG) << "Row filter callback produced an error:";
            qCWarning(KITEMMODELS_LOG) << result.toString();
            return true;
        } else {
            return result.toBool();
        }
    }

    return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
}

void KSortFilterProxyModel::setFilterString(const QString &filterString)
{
    if (filterString == m_filterString) {
        return;
    }
    m_filterString = filterString;
    QSortFilterProxyModel::setFilterFixedString(filterString);
    Q_EMIT filterStringChanged();
}

QString KSortFilterProxyModel::filterString() const
{
    return m_filterString;
}

QJSValue KSortFilterProxyModel::filterRowCallback() const
{
    return m_filterRowCallback;
}

void KSortFilterProxyModel::setFilterRowCallback(const QJSValue &callback)
{
    if (m_filterRowCallback.strictlyEquals(callback)) {
        return;
    }

    if (!callback.isNull() && !callback.isCallable()) {
        return;
    }

    m_filterRowCallback = callback;
    invalidateFilter();

    Q_EMIT filterRowCallbackChanged(callback);
}

void KSortFilterProxyModel::setFilterColumnCallback(const QJSValue &callback)
{
    if (m_filterColumnCallback.strictlyEquals(callback)) {
        return;
    }

    if (!callback.isNull() && !callback.isCallable()) {
        return;
    }

    m_filterColumnCallback = callback;
    invalidateFilter();

    Q_EMIT filterColumnCallbackChanged(callback);
}

QJSValue KSortFilterProxyModel::filterColumnCallback() const
{
    return m_filterColumnCallback;
}

void KSortFilterProxyModel::setFilterRole(const QString &role)
{
    QSortFilterProxyModel::setFilterRole(roleNameToId(role));
    m_filterRole = role;
    Q_EMIT filterRoleChanged();
}

QString KSortFilterProxyModel::filterRole() const
{
    return m_filterRole;
}

void KSortFilterProxyModel::setSortRole(const QString &role)
{
    m_sortRole = role;
    if (role.isEmpty()) {
        sort(-1, Qt::AscendingOrder);
    } else if (sourceModel()) {
        QSortFilterProxyModel::setSortRole(roleNameToId(role));
        sort(std::max(sortColumn(), 0), sortOrder());
    }
    Q_EMIT sortRoleChanged();
}

QString KSortFilterProxyModel::sortRole() const
{
    return m_sortRole;
}

void KSortFilterProxyModel::setSortOrder(const Qt::SortOrder order)
{
    sort(std::max(sortColumn(), 0), order);
    Q_EMIT sortOrderChanged();
}

void KSortFilterProxyModel::setSortColumn(int column)
{
    if (column == sortColumn()) {
        return;
    }
    sort(column, sortOrder());
    Q_EMIT sortColumnChanged();
}

void KSortFilterProxyModel::classBegin()
{
}

void KSortFilterProxyModel::componentComplete()
{
    m_componentCompleted = true;
    if (sourceModel()) {
        syncRoleNames();
        setFilterRole(m_filterRole);
        setSortRole(m_sortRole);
    }
}

void KSortFilterProxyModel::invalidateFilter()
{
    QSortFilterProxyModel::invalidateFilter();
}
