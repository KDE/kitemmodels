/*
 * Copyright (C) 2018 David Edmundson <davidedmundson@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#ifndef KNUMBERMODEL_H
#define KNUMBERMODEL_H

#include <QAbstractListModel>
#include <QScopedPointer>
#include <QLocale>

#include "kitemmodels_export.h"

class KNumberModelPrivate;

/**
 * Creates a model of entries from N to M with rows at a given interval
 *
 * The model contains two Roles
 * display, the number represented as a string
 * value, the actual value as a number
 *
 * @since 5.NUMBERMODEL_VERSION
 */
class KITEMMODELS_EXPORT KNumberModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY (qreal minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)
    Q_PROPERTY (qreal maximumValue READ maximumValue WRITE setMaximumValue NOTIFY maximumValueChanged)
    Q_PROPERTY (qreal stepSize READ stepSize WRITE setStepSize NOTIFY stepSizeChanged)
    Q_PROPERTY (QLocale::NumberOptions formattingOptions READ formattingOptions WRITE setFormattingOptions NOTIFY formattingOptionsChanged)

public:
    KNumberModel(QObject *parent = nullptr);
    ~KNumberModel() override;

    enum Roles {
        DisplayRole = Qt::DisplayRole,
        ValueRole = Qt::UserRole
    };

    /**
     * The minimum value for the model.
     * The default value is 1.
     */
    void setMinimumValue(qreal minimumValue);
    qreal minimumValue() const;

    /**
     * The maximum value in this model.
     * The default value is 1.
     *
     * @note  If max is a multiple of @arg step it will be included. Otherwise it will not be reached
     * i.e in a model with a min of 0 with a maximum of 1 with a step size of 0.3, the final row will be 0.9.
     *
     */
    void setMaximumValue(qreal maximumValue);
    qreal maximumValue() const;

    /*
     * Step between listed entries
     * The default value is 1.0
     */
    void setStepSize(qreal stepSize);
    qreal stepSize() const;

    /**
     * Define how the string representation of the number should be presented
     * i.e "1,000" or "1000"
     * Default is QLocale::Default
     */
    void setFormattingOptions(QLocale::NumberOptions options);
    QLocale::NumberOptions formattingOptions() const;

    /**
     * Returns the value represnted at the given index.
     */
    qreal value(const QModelIndex &index)  const;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const  override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void minimumValueChanged();
    void maximumValueChanged();
    void stepSizeChanged();
    void formattingOptionsChanged();

private:
    QScopedPointer<KNumberModelPrivate> d;
};

#endif
