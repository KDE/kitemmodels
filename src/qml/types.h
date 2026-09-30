/*
    SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QQmlEngine>

#include <KColumnHeadersModel>
#include <KNumberModel>

/*!
 * \qmltype KColumnHeadersModel
 * \inqmlmodule org.kde.kitemmodels
 * \nativetype KColumnHeadersModel
 * \brief Exposes a model's horizontal column headers as a list.
 *
 * Each row represents one column of sourceModel. Its roles return the
 * corresponding horizontal header data from the source model. The additional
 * \c sort role reports sortOrder for the row selected by sortColumn; it is
 * empty for other rows. Setting these properties does not sort the source model.
 *
 * \since 5.66
 */
struct KColumnHeadersModelForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(KColumnHeadersModel)
    QML_FOREIGN(KColumnHeadersModel)
};

/*!
 * \qmlproperty var KColumnHeadersModel::sourceModel
 * The model whose horizontal column headers are exposed as rows.
 */

/*!
 * \qmlproperty int KColumnHeadersModel::sortColumn
 * \default -1
 * The column whose \c sort role reports sortOrder. A value of -1 leaves the
 * \c sort role empty for every row.
 */

/*!
 * \qmlproperty Qt::SortOrder KColumnHeadersModel::sortOrder
 * \default Qt.AscendingOrder
 * The order reported by the \c sort role for sortColumn.
 */

/*!
 * \qmltype KNumberModel
 * \inqmlmodule org.kde.kitemmodels
 * \nativetype KNumberModel
 * \brief Provides a list of numbers at a fixed interval.
 *
 * The \c display role contains a locale-formatted string, and the \c value
 * role contains the number. The list starts at minimumValue and advances by
 * stepSize toward maximumValue. The end value is included only when a step
 * reaches it exactly.
 *
 * \since 5.65
 */
struct KNumberModelForeign {
    Q_GADGET
    QML_NAMED_ELEMENT(KNumberModel)
    QML_FOREIGN(KNumberModel)
};

/*!
 * \qmlproperty real KNumberModel::minimumValue
 * \default 0.0
 * The first value in the model.
 */

/*!
 * \qmlproperty real KNumberModel::maximumValue
 * \default 0.0
 * The upper bound for values in the model. The model includes this value only
 * when it falls on a step from minimumValue.
 */

/*!
 * \qmlproperty real KNumberModel::stepSize
 * \default 1.0
 * The interval between values.
 */

/*!
 * \qmlproperty QLocale::NumberOptions KNumberModel::formattingOptions
 * \default QLocale::DefaultNumberOptions
 * The \l QLocale number formatting options used by the \c display role.
 */
