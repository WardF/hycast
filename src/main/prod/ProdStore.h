/**
 * This file declares a store of data-products that can persist between
 * sessions.
 *
 * Copyright 2017 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: ProdStore.h
 * @author: Steven R. Emmerson
 */

#ifndef MAIN_PROD_PRODSTORE_H_
#define MAIN_PROD_PRODSTORE_H_

#include "Chunk.h"
#include "ProdInfo.h"
#include "ProdRcvr.h"

#include <memory>

namespace hycast {

class ProdStore final
{
    class Impl;

    std::shared_ptr<Impl> pImpl;

public:
    /**
     * Constructs. If the given file isn't the empty string, then the
     * product-store will be written to it upon destruction in order to persist
     * the store between sessions.
     * @param[in] path      Pathname of file for persisting the product-store
     *                      between sessions or the empty string to indicate no
     *                      persistence
     */
    explicit ProdStore(const std::string& pathname = "");

    /**
     * Destroys. Writes the product-store to the persistence-file if one was
     * specified during construction.
     */
    ~ProdStore() =default;

    /**
     * Adds product-information to an entry. Creates the entry if it doesn't
     * exist.
     * @param[in] prodInfo  Product information
     * @param[out] prod     Associated product
     * @retval true         Product is complete
     * @retval false        Product is not complete
     * @exceptionsafety     Basic guarantee
     * @threadsafety        Safe
     */
    bool add(const ProdInfo& prodInfo, Product& prod);

    /**
     * Adds a latent chunk of data to a product. Creates the product if it
     * doesn't already exist. Will not overwrite an existing chunk of data in
     * the product.
     * @param[in]  chunk  Latent chunk of data to be added
     * @param[out] prod   Associated product
     * @retval true       Product is complete
     * @retval false      Product is not complete
     * @exceptionsafety   Strong guarantee
     * @threadsafety      Safe
     */
    bool add(LatentChunk& chunk, Product& prod);
};

} // namespace

#endif /* MAIN_PROD_PRODSTORE_H_ */
