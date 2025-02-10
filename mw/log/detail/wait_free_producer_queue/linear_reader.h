// *******************************************************************************>
// Copyright (c) 2024 Contributors to the Eclipse Foundation
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
// SPDX-License-Identifier: Apache-2.0 #
// *******************************************************************************




#ifndef PLATFORM_AAS_MW_LOG_DETAIL_WAIT_FREE_PRODUCER_QUEUE_LINEAR_READER_H
#define PLATFORM_AAS_MW_LOG_DETAIL_WAIT_FREE_PRODUCER_QUEUE_LINEAR_READER_H

#include "platform/aas/mw/log/detail/wait_free_producer_queue/linear_control_block.h"

#include "amp_span.hpp"

#include <optional>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

/// \brief Reader for a linear buffer.
/// The Reader instance itself is not thread-safe and should only be used after
/// the last writer has finished.
class LinearReader
{
  public:
    explicit LinearReader(const amp::span<Byte>& data) noexcept;

    /// \brief Try to read the next available data.
    /// Returns empty if the data is not available.
    std::optional<amp::span<Byte>> Read() noexcept;
    /// \brief Get size of whole data span which means sum of length encoding headers and payload of each chunk
    Length GetSizeOfWholeDataBuffer() const noexcept;

    
  private:
    
    
    /* Members are private () */
    amp::span<Byte> data_;
    Length read_index_;
    
};

LinearReader CreateLinearReaderFromControlBlock(const LinearControlBlock&) noexcept;
LinearReader CreateLinearReaderFromDataAndLength(const amp::span<Byte>& data,
                                                 const Length number_of_bytes_written) noexcept;

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
