/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

//This class exposes KDescendantsProxyModel in a more QML friendly way

#pragma once

#include <QObject>
#include <QPointer>
#include <KDescendantsProxyModel>

class KDescendantsProxyModelQml : public KDescendantsProxyModel
{
    Q_OBJECT

public:
    explicit KDescendantsProxyModelQml(QObject *parent = nullptr);
    ~KDescendantsProxyModelQml();

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void toggleChild(int row);
    
private:
    void notifyhasSiblings(const QModelIndex &parent);
    QPointer <QAbstractItemModel> m_sourceModel;
    int m_levelRole = -1;
    int m_expandableRole = -1;
    int m_expandedRole = -1;
    int m_hasSiblingsRole = -1;
};
