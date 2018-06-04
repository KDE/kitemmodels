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

#include <QAbstractListModel>
#include <QScopedPointer>

class KNumberModelPrivate;

class KNumberModel: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY (qreal min READ min WRITE setMin NOTIFY minChanged);
    Q_PROPERTY (qreal max READ max WRITE setMax NOTIFY maxChanged);
    Q_PROPERTY (qreal step READ step WRITE setStep NOTIFY stepChanged);
    Q_PROPERTY (bool localeAware READ localeAware WRITE setLocaleAware NOTIFY localeAwareChanged);

public:
    KNumberModel(QObject *parent = nullptr);
    ~KNumberModel();

    void setMin(qreal);
    qreal min() const;

    void setMax(qreal);
    qreal max() const;

    /*
     * The default value is 1.0
     */
    void setStep(qreal);
    qreal step() const;

    void setLocaleAware(bool);
    bool localeAware() const;

    qreal value(const QModelIndex &index)  const;

    int rowCount(const QModelIndex &index=QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const  override;

Q_SIGNALS:
    void minChanged();
    void maxChanged();
    void stepChanged();
    void localeAwareChanged();

private:
    QScopedPointer<KNumberModelPrivate> d;
};
