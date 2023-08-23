/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KROLENAMESMODEL_H
#define KROLENAMESMODEL_H

#include <QAbstractItemModel>
#include <QAbstractListModel>

#include "kitemmodels_export.h"

#include <memory>

class KRoleNamesModelPrivate;

/**
 * @class KRoleNamesModel krolenamesmodel.h KRoleNamesModel
 *
 * @brief List model of and a mapper between roles and role names of a source model.
 *
 * KRoleNamesModel provides access to source model's roles in two ways.
 *
 * First, it is a list model itself, with a row per each source model's role,
 * and "role" and "roleName" roles per each row. This way could be useful for
 * introspection, like rolling out a view.
 *
 * Second, it exposes runtime-invokable methods to map from roles to role names and
 * vice-versa in an imperative fashion. This way might be more suitable for signal
 * handlers.
 *
 * @since 6.0
 */
class KITEMMODELS_EXPORT KRoleNamesModel : public QAbstractListModel
{
    Q_OBJECT

    /**
     * The source model whose role names are to be accessed.
     */
    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

public:
    enum Roles {
        RoleRole = Qt::UserRole,
        RoleNameRole,
    };
    Q_ENUM(Roles)

    explicit KRoleNamesModel(QObject *parent = nullptr);
    ~KRoleNamesModel() override;

    QAbstractItemModel *sourceModel() const;
    void setSourceModel(QAbstractItemModel *model);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QByteArray roleName(int role) const;
    Q_INVOKABLE int role(const QByteArray &roleName) const;

Q_SIGNALS:
    void sourceModelChanged();

private Q_SLOTS:
    void update();
    void sourceModelDestroyed(QObject *source);

private:
    Q_DECLARE_PRIVATE(KRoleNamesModel)
    //@cond PRIVATE
    std::unique_ptr<KRoleNamesModelPrivate> const d_ptr;
};

#endif
