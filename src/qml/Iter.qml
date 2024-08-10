/*
 *   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

pragma Singleton

import QtQuick
import QtQml.Models
import "iter_impl.js" as Impl

QtObject {
    /**
     * Iterable over items of a Repeater.
     *
     * The returned object implements the iterable protocol `Symbol.iterator`,
     * thus it can be looped over in a `for...of` construct.
     */
    function repeater(repeater: Repeater): /*Iterable*/ var {
        return Impl.repeater(repeater);
    }

    /**
     * Iterable over objects of an Instantiator.
     *
     * The returned object implements the iterable protocol `Symbol.iterator`,
     * thus it can be looped over in a `for...of` construct.
     */
    function instantiator(instantiator: Instantiator): /*Iterable*/ var {
        return Impl.instantiator(instantiator);
    }

    function modelIndices(
        model /*QAbstractItemModel*/,
        column /*int*/ = 0,
        parentIndex /*QModelIndex | undefined*/ = undefined,
    ): /*Iterable*/ var {
        return Impl.modelIndices(model, column, parentIndex);
    }

    function modelData(
        model /*QAbstractItemModel*/,
        role /*int*/ = Qt.DisplayRole,
        column /*int*/ = 0,
        parentIndex /*QModelIndex | undefined*/ = undefined,
    ): /*Iterable*/ var {
        return Impl.modelData(model, role, column, parentIndex);
    }
}
