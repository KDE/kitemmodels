/*
 *   SPDX-FileCopyrightText: 2024 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

.pragma library

.import QtQuick as QtQuick
.import QtQml.Models as QtQmlModels
.import org.kde.kitemmodels as KItemModels

class RepeaterIterator {
    constructor(repeater) {
        this.repeater = repeater as QtQuick.Repeater;
        this.index = 0;
    }

    next() {
        if (this.repeater === null || this.index >= this.repeater.count) {
            return { done: true };
        }
        const value = this.repeater.itemAt(this.index);
        this.index += 1;
        return { value };
    }
}

class RepeaterIterable {
    constructor(repeater) {
        this.repeater = repeater as QtQuick.Repeater;
    }

    [Symbol.iterator]() {
        return new RepeaterIterator(this.repeater);
    }
}

function repeater(repeater) {
    if (repeater === null || !(repeater instanceof QtQuick.Repeater)) {
        return [];
    }

    return new RepeaterIterable(repeater);
}

class InstantiatorIterator {
    constructor(instantiator) {
        this.instantiator = instantiator as QtQmlModels.Instantiator;
        this.index = 0;
    }

    next() {
        if (this.instantiator === null || this.index >= this.instantiator.count) {
            return { done: true };
        }
        const value = this.instantiator.objectAt(this.index);
        this.index += 1;
        return { value };
    }
}

class InstantiatorIterable {
    constructor(instantiator) {
        this.instantiator = instantiator as QtQmlModels.Instantiator;
    }

    [Symbol.iterator]() {
        return new InstantiatorIterator(this.instantiator);
    }
}

function instantiator(instantiator) {
    if (instantiator === null || !(instantiator instanceof QtQmlModels.Instantiator)) {
        return [];
    }

    return new InstantiatorIterable(instantiator);
}

function makeInvalidIndex(model) {
    if (model !== null) {
        return model.index(-1, -1);
    } else {
        return null;
    }
}

class ModelIndicesIterator {
    constructor(model, column, parentIndex) {
        this.model = model;
        this.column = column;
        this.parentIndex = parentIndex ?? makeInvalidIndex(model);
        this.row = 0;
    }

    next() {
        if (this.model !== null) {
            const index = this.model.index(this.row, this.column, this.parentIndex);
            if (index.valid) {
                this.row += 1;
                return { value: index };
            }
        }
        return { done: true };
    }
}

class ModelIndicesIterable {
    constructor(model, column, parentIndex) {
        this.model = model;
        this.column = column;
        this.parentIndex = parentIndex;
    }

    [Symbol.iterator]() {
        return new ModelIndicesIterator(this.model, this.column, this.parentIndex);
    }
}

function modelIndices(
    model /*QAbstractItemModel*/,
    column /*int*/ = 0,
    parentIndex /*QModelIndex | undefined*/ = undefined,
) {
    return new ModelIndicesIterable(model, column, parentIndex);
}

class ModelDataIterator {
    constructor(model, role, column, parentIndex) {
        this.model = model;
        this.role = role;
        this.column = column;
        this.parentIndex = parentIndex ?? makeInvalidIndex(model);
        this.row = 0;
    }

    next() {
        if (this.model !== null) {
            const index = this.model.index(this.row, this.column, this.parentIndex);
            if (index.valid) {
                this.row += 1;
                const data = index.data(this.role);
                return { value: data };
            }
        }
        return { done: true };
    }
}

class ModelDataIterable {
    constructor(model, role, column, parentIndex) {
        this.model = model;
        this.role = role;
        this.column = column;
        this.parentIndex = parentIndex;
    }

    [Symbol.iterator]() {
        return new ModelDataIterator(this.model, this.role, this.column, this.parentIndex);
    }
}

function modelData(
    model /*QAbstractItemModel*/,
    role /*int*/ = Qt.DisplayRole,
    column /*int*/ = 0,
    parentIndex /*QModelIndex | undefined*/ = undefined,
) {
    return new ModelDataIterable(model, role, column, parentIndex);
}
