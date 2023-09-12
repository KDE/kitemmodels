/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "kmodelindexobserver.h"

#include <QMap>
#include <QQmlPropertyMap>

#include <algorithm>
#include <iterator>
#include <qassert.h>
#include <utility>

#include "kitemmodels_debug.h"

// Internal support class to resolve and maintain both lists of roles and role
// names. It remembers the last type assigned (SourceOfTruth: Roles or
// RoleNames), and uses that information to decide on mapping direction.
class ModelRoleNamesResolver
{
public:
    bool resolve(const QMap<int, QByteArray> &mapping)
    {
        if (m_source == Roles) {
            QList<QByteArray> roleNames;
            for (const auto role : std::as_const(m_roles)) {
                const QByteArray roleName = mapping.value(role, QByteArray());
                roleNames.append(roleName);
            }
            const bool changed = (m_roleNames != roleNames);
            if (changed) {
                m_roleNames = roleNames;
            }
            return changed;
        } else {
            QList<int> roles;
            for (const auto &roleName : std::as_const(m_roleNames)) {
                const int role = mapping.key(roleName, -1);
                roles.append(role);
            }
            const bool changed = (m_roles != roles);
            if (changed) {
                m_roles = roles;
            }
            return changed;
        }
    }

    inline const QList<int> &roles() const
    {
        return m_roles;
    }

    bool setRoles(const QList<int> &roles)
    {
        const bool changed = (m_roles != roles);
        if (changed) {
            m_roles = roles;
        }
        m_source = Roles;
        return changed;
    }

    inline const QList<QByteArray> &roleNames() const
    {
        return m_roleNames;
    }

    QStringList roleNamesAsStringList() const
    {
        QStringList names;
        names.reserve(m_roleNames.count());
        for (const auto &roleName : m_roleNames) {
            names.append(QString::fromUtf8(roleName));
        }
        return names;
    }

    bool setRoleNames(const QList<QByteArray> &roleNames)
    {
        const bool changed = (m_roleNames != roleNames);
        if (changed) {
            m_roleNames = roleNames;
        }
        m_source = RoleNames;
        return changed;
    }

    bool setRoleNames(const QStringList &roleNames)
    {
        QList<QByteArray> names;
        names.reserve(roleNames.count());
        for (const auto &roleName : roleNames) {
            names.append(roleName.toUtf8());
        }
        return setRoleNames(names);
    }

private:
    enum SourceOfTruth {
        Roles,
        RoleNames,
    };

    QList<int> m_roles;
    QList<QByteArray> m_roleNames;
    SourceOfTruth m_source = Roles;
};

class KModelIndexObserverPrivate
{
    KModelIndexObserver *const q;

public:
    explicit KModelIndexObserverPrivate(KModelIndexObserver *qq)
        : q(qq)
    {
    }

    ~KModelIndexObserverPrivate()
    {
        if (modelData) {
            modelData->deleteLater();
            modelData = nullptr;
        }
    }

    QAbstractItemModel *model = nullptr;
    // Sorted, has stable iterators
    QMap<int, QByteArray> cachedRoleNames;

    // Set of observed roles. If empty, observe everything.
    ModelRoleNamesResolver resolver;

    // Have to store row and column separately, because we can't trust index
    // to store it for us: model might return an invalid index for partial
    // data (only row or only column) as properties are assigned one by one.
    int row = -1;
    int column = 0;
    QModelIndex parentIndex;
    QModelIndex index;

    QQmlPropertyMap *modelData = nullptr;

    void recreateIndex()
    {
        if (model) {
            index = model->index(row, column, parentIndex);
        } else {
            index = QModelIndex();
        }
        qCDebug(KITEMMODELS_LOG) << q << "recreated index" << index;
    }

    void updateRoles()
    {
    }
};

KModelIndexObserver::KModelIndexObserver(QObject *parent)
    : QObject(parent)
    , d(new KModelIndexObserverPrivate(this))
{
}

KModelIndexObserver::~KModelIndexObserver() = default;

QAbstractItemModel *KModelIndexObserver::sourceModel() const
{
    return d->model;
}

void KModelIndexObserver::setSourceModel(QAbstractItemModel *model)
{
    if (d->model == model) {
        return;
    }

    if (d->model) {
        disconnect(d->model, nullptr, this, nullptr);
    }

    d->model = model;

    if (d->model) {
        connect(d->model, &QAbstractItemModel::rowsInserted, this, &KModelIndexObserver::rowsInsertedOrRemoved);
        connect(d->model, &QAbstractItemModel::rowsRemoved, this, &KModelIndexObserver::rowsInsertedOrRemoved);
        connect(d->model, &QAbstractItemModel::rowsMoved, this, &KModelIndexObserver::rowsMoved);
        connect(d->model, &QAbstractItemModel::columnsInserted, this, &KModelIndexObserver::columnsInsertedOrRemoved);
        connect(d->model, &QAbstractItemModel::columnsRemoved, this, &KModelIndexObserver::columnsInsertedOrRemoved);
        connect(d->model, &QAbstractItemModel::columnsMoved, this, &KModelIndexObserver::columnsMoved);
        connect(d->model, &QAbstractItemModel::dataChanged, this, &KModelIndexObserver::dataChanged);
        connect(d->model, &QAbstractItemModel::modelReset, this, &KModelIndexObserver::update);
        connect(d->model, &QObject::destroyed, this, &KModelIndexObserver::sourceModelDestroyed);
    }

    d->recreateIndex();
    update();
    Q_EMIT sourceModelChanged();
}

int KModelIndexObserver::row() const
{
    return d->row;
}

void KModelIndexObserver::setRow(int row)
{
    if (d->row == row) {
        return;
    }

    d->row = row;
    d->recreateIndex();

    updateModelData();
    Q_EMIT indexChanged();
}

int KModelIndexObserver::column() const
{
    return d->column;
}

void KModelIndexObserver::setColumn(int column)
{
    if (d->column == column) {
        return;
    }

    d->column = column;
    d->recreateIndex();

    updateModelData();
    Q_EMIT indexChanged();
}

QModelIndex KModelIndexObserver::parentIndex() const
{
    return d->parentIndex;
}

void KModelIndexObserver::setParentIndex(const QModelIndex &parentIndex)
{
    if (d->parentIndex == parentIndex) {
        return;
    }

    d->parentIndex = parentIndex;
    d->recreateIndex();

    updateModelData();
    Q_EMIT indexChanged();
}

QModelIndex KModelIndexObserver::index() const
{
    return d->index;
}

void KModelIndexObserver::setIndex(const QModelIndex &index)
{
    if (d->index == index) {
        return;
    }

    d->index = index;
    d->row = index.row();
    d->column = index.column();
    d->parentIndex = index.parent();

    updateModelData();
    Q_EMIT indexChanged();
}

QList<int> KModelIndexObserver::roles() const
{
    return d->resolver.roles();
}

void KModelIndexObserver::setRoles(const QList<int> &roles)
{
    bool changed = false;

    changed |= d->resolver.setRoles(roles);
    changed |= d->resolver.resolve(d->cachedRoleNames);

    if (changed) {
        updateModelData();
        Q_EMIT rolesChanged();
    }
}

QStringList KModelIndexObserver::roleNames() const
{
    return d->resolver.roleNamesAsStringList();
}

void KModelIndexObserver::setRoleNames(const QStringList &roleNames)
{
    bool changed = false;

    changed |= d->resolver.setRoleNames(roleNames);
    changed |= d->resolver.resolve(d->cachedRoleNames);

    if (changed) {
        updateModelData();
        Q_EMIT rolesChanged();
    }
}

void KModelIndexObserver::resetRoles()
{
    bool changed = false;

    changed |= d->resolver.setRoles({});
    changed |= d->resolver.setRoleNames(QList<QByteArray>());

    if (changed) {
        updateModelData();
        Q_EMIT rolesChanged();
    }
}

QQmlPropertyMap *KModelIndexObserver::modelData() const
{
    return d->modelData;
}

namespace
{
template<typename K, typename T>
QMap<K, T> hashToMap(const QHash<K, T> &hash)
{
    auto map = QMap<K, T>();
    for (auto it = hash.constBegin(); it != hash.constEnd(); it++) {
        map[it.key()] = it.value();
    }
    return map;
}

template<typename T>
QList<T> listsIntersection(const QList<T> &lhs, const QList<T> &rhs)
{
    QList<T> result;
    for (const auto left : lhs) {
        for (const auto right : rhs) {
            if (left == right) {
                result.append(right);
            }
        }
    }
    return result;
}
}

void KModelIndexObserver::update()
{
    if (d->model) {
        d->cachedRoleNames = hashToMap(d->model->roleNames());
    } else {
        d->cachedRoleNames.clear();
    }
    d->resolver.resolve(d->cachedRoleNames);
    qCDebug(KITEMMODELS_LOG) << this << "resolved roles" << d->cachedRoleNames << d->resolver.roleNamesAsStringList() << d->resolver.roles();
    updateModelData();
}

void KModelIndexObserver::updateModelData(const QList<int> &roles)
{
    QList<int> changedWatchedRoles;

    if (!roles.isEmpty() && !d->resolver.roles().isEmpty()) {
        changedWatchedRoles = listsIntersection(roles, d->resolver.roles());
        if (changedWatchedRoles.isEmpty()) {
            qCDebug(KITEMMODELS_LOG) << this << "updateModelData: empty intersection";
            return;
        }
    }

    if (!d->model || !d->index.isValid() || !d->model->checkIndex(d->index, QAbstractItemModel::CheckIndexOption::IndexIsValid)) {
        qCDebug(KITEMMODELS_LOG) << this << "updateModelData: Removing modelData" << (!d->model) << (!d->index.isValid())
                                 << (!d->model->checkIndex(d->index, QAbstractItemModel::CheckIndexOption::IndexIsValid));
        if (d->modelData) {
            d->modelData->deleteLater();
            d->modelData = nullptr;
            Q_EMIT modelDataChanged();
        }
    } else {
        Q_ASSERT(d->model);
        Q_ASSERT(d->index.isValid());
        Q_ASSERT(d->model->checkIndex(d->index, QAbstractItemModel::CheckIndexOption::IndexIsValid));

        if (!d->modelData) {
            d->modelData = new QQmlPropertyMap;

            // set up all roles
            for (auto it = d->cachedRoleNames.constKeyValueBegin(); it != d->cachedRoleNames.constKeyValueEnd(); it++) {
                const auto [role, roleName] = *it;
                if (!changedWatchedRoles.isEmpty() && !changedWatchedRoles.contains(role)) {
                    continue;
                }
                const auto roleNameString = QString::fromUtf8(roleName);
                const auto data = d->model->data(d->index, role);
                d->modelData->insert(roleNameString, data);
            }
            qCDebug(KITEMMODELS_LOG) << this << "updateModelData: Created map with keys" << d->modelData->keys();
            Q_EMIT modelDataChanged();
        } else {
            for (auto it = d->cachedRoleNames.constKeyValueBegin(); it != d->cachedRoleNames.constKeyValueEnd(); it++) {
                const auto [role, roleName] = *it;
                if (!changedWatchedRoles.isEmpty() && !changedWatchedRoles.contains(role)) {
                    continue;
                }
                const auto roleNameString = QString::fromUtf8(roleName);
                const auto data = d->model->data(d->index, role);
                d->modelData->insert(roleNameString, data);
            }
            qCDebug(KITEMMODELS_LOG) << this << "updateModelData: Changed map for roles" << changedWatchedRoles;
            Q_EMIT modelDataChanged();
        }
    }
}

namespace
{
bool insertionOrRemovalAffectsIndex(const QModelIndex &parent, int index, const QModelIndex &sourceParent, int first, int last)
{
    Q_UNUSED(last)

    if (parent == sourceParent) {
        if (index >= first) {
            return true;
        }
    }

    return false;
}

bool moveAffectsIndex(const QModelIndex &parent,
                      int index,
                      const QModelIndex &sourceParent,
                      int first,
                      int last,
                      const QModelIndex &destinationParent,
                      int destination)
{
    Q_UNUSED(last)

    if (parent == sourceParent) {
        if (index >= first) {
            return true;
        }
    }

    if (parent == destinationParent) {
        if (index >= destination) {
            return true;
        }
    }

    return false;
}
}

void KModelIndexObserver::rowsInsertedOrRemoved(const QModelIndex &parent, int first, int last)
{
    if (insertionOrRemovalAffectsIndex(d->parentIndex, d->row, parent, first, last)) {
        updateModelData();
    }
}

void KModelIndexObserver::columnsInsertedOrRemoved(const QModelIndex &parent, int first, int last)
{
    if (insertionOrRemovalAffectsIndex(d->parentIndex, d->column, parent, first, last)) {
        updateModelData();
    }
}

void KModelIndexObserver::rowsMoved(const QModelIndex &parent, int first, int last, const QModelIndex &destinationParent, int destinationRow)
{
    if (moveAffectsIndex(d->parentIndex, d->row, parent, first, last, destinationParent, destinationRow)) {
        updateModelData();
    }
}

void KModelIndexObserver::columnsMoved(const QModelIndex &parent, int first, int last, const QModelIndex &destinationParent, int destinationColumn)
{
    if (moveAffectsIndex(d->parentIndex, d->column, parent, first, last, destinationParent, destinationColumn)) {
        updateModelData();
    }
}

void KModelIndexObserver::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    if ((!d->parentIndex.isValid() && !topLeft.parent().isValid()) || d->parentIndex != topLeft.parent()) {
        return;
    }

    if (d->row < topLeft.row() || d->row > bottomRight.row() || //
        d->column < topLeft.column() || d->column > bottomRight.column()) {
        return;
    }

    updateModelData(roles);
}

void KModelIndexObserver::sourceModelDestroyed(QObject *source)
{
    Q_ASSERT(source == d->model);

    setSourceModel(nullptr);
}

#include "moc_kmodelindexobserver.cpp"
