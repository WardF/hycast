/**
 * Map whose entries are also linked together into a list.
 *
 * Copyright 2020 University Corporation for Atmospheric Research. All Rights
 * reserved. See file "COPYING" in the top-level source-directory for usage
 * restrictions.
 *
 *        File: LinkedMap.h
 *  Created on: May 13, 2020
 *      Author: Steven R. Emmerson
 */

#ifndef MAIN_MISC_LINKEDMAP_H_
#define MAIN_MISC_LINKEDMAP_H_

#include <memory>

namespace hycast {

/**
 * @tparam KEY    Key type. Must have:
 *                  - Default constructor;
 *                  - `operator bool()`, which must return false if and only if
 *                    default constructed; and
 *                  - `operator ==(const Key& rhs)`
 * @tparam VALUE  Value type
 */
template<class KEY, class VALUE>
class LinkedMap
{
    class Impl;

    std::shared_ptr<Impl> pImpl;

public:
    /**
     * Default constructs.
     */
    LinkedMap();

    /**
     * Constructs with an initial size.
     *
     * @param[in] initSize  Initial size
     */
    LinkedMap(const size_t initSize);

    /**
     * Returns the number of entries.
     *
     * @return Number of entries
     */
    size_t size() const noexcept;

    /**
     * Adds an entry. If a new entry is created, then it is added to the tail of
     * the list.
     *
     * @param[in] key    Key
     * @param[in] value  Value mapped-to by key
     * @return           Pointer to value in map
     */
    VALUE* add(const KEY& key, VALUE& value);

    /**
     * Returns the value that corresponds to a key.
     *
     * @param[in] key        Key
     * @retval    `nullptr`  No such value
     * @return               Pointer to value. Valid only while no changes are
     *                       made.
     */
    VALUE* find(const KEY& key);

    /**
     * Removes an entry.
     *
     * @param[in] key           Key of entry to be removed
     * @return                  Value associated with key
     * @throws InvalidArgument  No such entry
     */
    VALUE remove(const KEY& key);

    /**
     * Returns the key of the head of the list.
     *
     * @return                  Key of head of list. Will test false if the list
     *                          is empty.
     */
    KEY getHead();

    /**
     * Returns the key of the tail of the list.
     *
     * @return                  Key of tail of list. Will test false if the list
     *                          is empty.
     */
    KEY getTail();
};

} // namespace

#endif /* MAIN_MISC_LINKEDMAP_H_ */
