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

    Q_INVOKABLE void expandChild(int row);
    Q_INVOKABLE void collapseChild(int row);
    Q_INVOKABLE void toggleChild(int row);
    
private:
    QPointer <QAbstractItemModel> m_sourceModel;
};
