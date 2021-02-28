/*
    SPDX-FileCopyrightText: 2019 Arjen Hiemstra <ahiemstra@heimr.nl>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCOLUMNHEADERSMODEL_H
#define KCOLUMNHEADERSMODEL_H

#include "kitemmodels_export.h"

#include <QAbstractListModel>

#include <memory>

class KColumnHeadersModelPrivate;

/**
 * A model that converts a model's headers into a list model.
 *
 * This model will expose the source model's headers as a simple list. This is
 * mostly useful as a helper for QML applications that want to display a model's
 * headers.
 *
 * Each columns's header will be presented as a row in this model. Roles are
 * forwarded directly to the source model's headerData() method.
 *
 * @since 5.66
 */
class KITEMMODELS_EXPORT KColumnHeadersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

public:
    explicit KColumnHeadersModel(QObject *parent = nullptr);
    ~KColumnHeadersModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QAbstractItemModel *sourceModel() const;
    void setSourceModel(QAbstractItemModel *newSourceModel);

Q_SIGNALS:
    void sourceModelChanged();

private:
    const std::unique_ptr<KColumnHeadersModelPrivate> d;
};

#endif
