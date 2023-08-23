/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "krolenamesmodel.h"

#include <QMap>

#include <iterator>

class KRoleNamesModelPrivate
{
public:
    QAbstractItemModel *model = nullptr;
    // Sorted, has stable iterators
    QMap<int, QByteArray> cachedRoleNames;
};

KRoleNamesModel::KRoleNamesModel(QObject *parent)
    : QAbstractListModel(parent)
    , d_ptr(new KRoleNamesModelPrivate)
{
}

KRoleNamesModel::~KRoleNamesModel() = default;

QAbstractItemModel *KRoleNamesModel::sourceModel() const
{
    Q_D(const KRoleNamesModel);

    return d->model;
}

void KRoleNamesModel::setSourceModel(QAbstractItemModel *model)
{
    Q_D(KRoleNamesModel);

    if (d->model == model) {
        return;
    }

    if (d->model) {
        disconnect(d->model, nullptr, this, nullptr);
    }

    d->model = model;

    // Binding to self will cause infinite recursion
    if (d->model && d->model != this) {
        connect(d->model, &QAbstractItemModel::modelReset, this, &KRoleNamesModel::update);
        connect(d->model, &QObject::destroyed, this, &KRoleNamesModel::sourceModelDestroyed);
    }

    update();
    Q_EMIT sourceModelChanged();
}

int KRoleNamesModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const KRoleNamesModel);

    if (!d->model || parent.isValid()) {
        return 0;
    }

    return d->cachedRoleNames.count();
}

QVariant KRoleNamesModel::data(const QModelIndex &index, int role) const
{
    Q_D(const KRoleNamesModel);

    if (!d->model) {
        return QVariant();
    }

    if (!index.isValid() || index.row() >= d->cachedRoleNames.count()) {
        return QVariant();
    }

    auto it = d->cachedRoleNames.constBegin();
    std::advance(it, index.row());

    switch (role) {
    case RoleRole:
        return it.key();
    case RoleNameRole:
        return it.value();
    }

    return QVariant();
}

QHash<int, QByteArray> KRoleNamesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[RoleRole] = "role";
    roles[RoleNameRole] = "roleName";
    return roles;
}

QByteArray KRoleNamesModel::roleName(int role) const
{
    Q_D(const KRoleNamesModel);

    return d->cachedRoleNames.value(role);
}

int KRoleNamesModel::role(const QByteArray &roleName) const
{
    Q_D(const KRoleNamesModel);

    return d->cachedRoleNames.key(roleName, -1);
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
}

void KRoleNamesModel::update()
{
    Q_D(KRoleNamesModel);

    beginResetModel();
    if (d->model) {
        d->cachedRoleNames = hashToMap(d->model->roleNames());
    } else {
        d->cachedRoleNames.clear();
    }
    endResetModel();
}

void KRoleNamesModel::sourceModelDestroyed(QObject *source)
{
    Q_D(KRoleNamesModel);

    Q_ASSERT(source == d->model);

    setSourceModel(nullptr);
}

#include "moc_krolenamesmodel.cpp"
