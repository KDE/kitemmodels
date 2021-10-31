/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// This class exposes KDescendantsProxyModel in a more QML friendly way

#pragma once

#include <KDescendantsProxyModel>
#include <QObject>
#include <QPointer>

class KDescendantsProxyModelQml : public KDescendantsProxyModel
{
    Q_OBJECT

public:
    explicit KDescendantsProxyModelQml(QObject *parent = nullptr);
    ~KDescendantsProxyModelQml() override;

    Q_INVOKABLE void expandChildren(int row);
    Q_INVOKABLE void collapseChildren(int row);
    Q_INVOKABLE void toggleChildren(int row);
};
