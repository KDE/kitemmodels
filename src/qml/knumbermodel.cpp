/*
 * Copyright (C) 2018 David Edmundson
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
#include <QDebug>

#include <cmath>

class KNumberModelPrivate
{
public:
    qreal min = 0.0;
    qreal max = 0.0;
    qreal step = 1.0;
    bool localeAware = false;
};

KNumberModel::KNumberModel(QObject *parent):
    QAbstractListModel(parent),
    d(new KNumberModelPrivate)
{}

KNumberModel::~KNumberModel()
{}

void KNumberModel::setMin(qreal min)
{
    if (min == d->min) {
        return;
    }
    beginResetModel();
    d->min = min;
    endResetModel();
    emit minChanged();
}

qreal KNumberModel::min() const
{
    return d->min;
}

void KNumberModel::setMax(qreal max)
{
    if (max == d->max) {
        return;
    }
    beginResetModel();
    d->max = max;
    endResetModel();
    emit maxChanged();
}

qreal KNumberModel::max() const
{
    return d->max;
}

void KNumberModel::setStep(qreal step)
{
    if (step == d->step) {
        return;
    }
    beginResetModel();
    d->step = step;
    endResetModel();
    emit stepChanged();
}

qreal KNumberModel::step() const
{
    return d->step;
}

void KNumberModel::setLocaleAware(bool localeAware)
{
    if (d->localeAware == localeAware) {
        return;
    }
    d->localeAware = localeAware;

    if (rowCount() == 0) {
        return;
    }
    dataChanged(index(0, 0, QModelIndex()), index(rowCount(), 0, QModelIndex()));
    emit localeAwareChanged();
}

bool KNumberModel::localeAware() const
{
    return d->localeAware;
}

qreal KNumberModel::value(const QModelIndex &index)  const
{
    return d->min + d->step * index.row();
}

int KNumberModel::rowCount(const QModelIndex &index) const {
    if (index.parent().isValid()) {
        return 0;
    }
    return std::max(0.0, std::floor((d->max - d->min) / d->step));
};

QVariant KNumberModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
        case Qt::DisplayRole:
            if (d->localeAware) {
                return QVariant(QLocale::system().toString(value(index)));
            } else {
                return QVariant(QString::number(value(index)));
            }
            break;
        case Qt::UserRole:
            return QVariant(value(index));
    }
    return QVariant();
}
