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

#include "knumbermodel.h"

#include <QLocale>
#include <QtMath>

#include <cmath>

class KNumberModelPrivate
{
public:
    qreal minimumValue = 0.0;
    qreal maximumValue = 0.0;
    qreal stepSize = 1.0;
    QLocale::NumberOptions formattingOptions = QLocale::DefaultNumberOptions;
};

KNumberModel::KNumberModel(QObject *parent):
    QAbstractListModel(parent),
    d(new KNumberModelPrivate)
{}

KNumberModel::~KNumberModel()
{}

void KNumberModel::setMinimumValue(qreal minimumValue)
{
    if (minimumValue == d->minimumValue) {
        return;
    }
    beginResetModel();
    d->minimumValue = minimumValue;
    endResetModel();
    emit minimumValueChanged();
}

qreal KNumberModel::minimumValue() const
{
    return d->minimumValue;
}

void KNumberModel::setMaximumValue(qreal maximumValue)
{
    if (maximumValue == d->maximumValue) {
        return;
    }
    beginResetModel();
    d->maximumValue = maximumValue;
    endResetModel();
    emit maximumValueChanged();
}

qreal KNumberModel::maximumValue() const
{
    return d->maximumValue;
}

void KNumberModel::setStepSize(qreal stepSize)
{
    if (stepSize == d->stepSize) {
        return;
    }
    beginResetModel();
    d->stepSize = stepSize;
    endResetModel();
    emit stepSizeChanged();
}

qreal KNumberModel::stepSize() const
{
    return d->stepSize;
}

void KNumberModel::setFormattingOptions(QLocale::NumberOptions formattingOptions)
{
    if (d->formattingOptions == formattingOptions) {
        return;
    }
    d->formattingOptions = formattingOptions;

    if (rowCount() == 0) {
        return;
    }
    dataChanged(index(0, 0, QModelIndex()), index(rowCount(), 0, QModelIndex()), QVector<int>{DisplayRole});
    emit formattingOptionsChanged();
}

QLocale::NumberOptions KNumberModel::formattingOptions() const
{
    return d->formattingOptions;
}

qreal KNumberModel::value(const QModelIndex &index)  const
{
    if (!index.isValid()) {
        return 0.0;
    }
    return d->minimumValue + d->stepSize * index.row();
}

int KNumberModel::rowCount(const QModelIndex &index) const
{
    if (index.parent().isValid()) {
        return 0;
    }
    if (d->stepSize == 0) {
        return 1;
    }
    //1 initial entry (the minimumValue) + the number of valid steps afterwards
    return 1 + std::max(0, qFloor((d->maximumValue - d->minimumValue) / d->stepSize));
}

QVariant KNumberModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
        case KNumberModel::DisplayRole: {
            auto locale = QLocale::system();
            locale.setNumberOptions(d->formattingOptions);
            return QVariant(locale.toString(value(index)));
        }
        case KNumberModel::ValueRole:
            return QVariant(value(index));
    }
    return QVariant();
}

QHash<int, QByteArray> KNumberModel::roleNames() const
{
    return {{KNumberModel::DisplayRole, QByteArrayLiteral("display")},
                  {KNumberModel::ValueRole, QByteArrayLiteral("value")}};
}
