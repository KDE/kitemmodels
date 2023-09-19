/*
    SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QQmlEngine>

#include <KColumnHeadersModel>
#include <KNumberModel>

struct KColumnHeadersModelForeign {
    Q_GADGET
    QML_ELEMENT
    QML_FOREIGN(KColumnHeadersModel)
    QML_NAMED_ELEMENT(KColumnHeadersModel)
};

struct KNumberModelForeign {
    Q_GADGET
    QML_ELEMENT
    QML_FOREIGN(KNumberModel)
    QML_NAMED_ELEMENT(KNumberModel)
};
