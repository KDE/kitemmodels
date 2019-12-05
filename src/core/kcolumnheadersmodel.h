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

#ifndef KCOLUMNHEADERSMODEL_H
#define KCOLUMNHEADERSMODEL_H

#include "kitemmodels_export.h"

#include <memory>
#include <QAbstractListModel>

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
 * @since 5.65
 */
class KITEMMODELS_EXPORT KColumnHeadersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

public:
    explicit KColumnHeadersModel(QObject *parent = nullptr);
    ~KColumnHeadersModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex{}) const override;
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
