/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2026 Jakob Petsovits <jpetso@petsovits.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// This class exposes KModelIndexProxyMapper in a more QML friendly way

#pragma once

#include <KModelIndexProxyMapper>
#include <QObject>

#include <qqmlregistration.h>

/*!
 * \qmltype KModelIndexProxyMapper
 * \inqmlmodule org.kde.kitemmodels
 * \nativetype KModelIndexProxyMapper
 * \brief Facilitates easy mapping of indexes and selections through proxy models.
 */
class KModelIndexProxyMapperQml
{
    Q_GADGET
    QML_FOREIGN(KModelIndexProxyMapper)
    QML_NAMED_ELEMENT(KModelIndexProxyMapper)
};
