/********************************************************************************
* Copyright (c) 2025 Contributors to the Eclipse Foundation
*
* See the NOTICE file(s) distributed with this work for additional
* information regarding copyright ownership.
*
* This program and the accompanying materials are made available under the
* terms of the Apache License Version 2.0 which is available at
* https://www.apache.org/licenses/LICENSE-2.0
*
* SPDX-License-Identifier: Apache-2.0
********************************************************************************/


#include "platform/aas/mw/log/detail/file_logging/non_blocking_writer.h"

#if defined(__QNXNTO__)
#include <sys/iomsg.h>
#endif  //  __QNXNTO__

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

std::size_t NonBlockingWriter::GetMaxChunkSize() noexcept
{
    
/// \brief Maximum number of bytes to be flushed in one call.
/// For QNX The Max size of bytes to be written shall be less than SSIZE_MAX - sizeof(io_write_t)
#if defined __QNX__
    constexpr std::size_t kMaxChunkSizeSupportedByOs = static_cast<std::size_t>(SSIZE_MAX) - sizeof(io_write_t);
#else
    constexpr std::size_t kMaxChunkSizeSupportedByOs = static_cast<std::size_t>(SSIZE_MAX);
#endif
    
    return kMaxChunkSizeSupportedByOs;
}

NonBlockingWriter::NonBlockingWriter(const int32_t fileHandle,
                                     std::size_t max_chunk_size,
                                     amp::pmr::unique_ptr<bmw::os::Unistd> unistd) noexcept
    : unistd_{std::move(unistd)},
      file_handle_(fileHandle),
      number_of_flushed_bytes_{0},
      buffer_{nullptr, 0},
      buffer_flushed_{Result::kWouldBlock},
      max_chunk_size_(std::min(max_chunk_size, GetMaxChunkSize()))

{
}

void NonBlockingWriter::SetSpan(const amp::span<const std::uint8_t>& buffer) noexcept
{
    buffer_flushed_ = Result::kWouldBlock;
    number_of_flushed_bytes_ = 0;
    buffer_ = buffer;
}

amp::expected<NonBlockingWriter::Result, bmw::mw::log::detail::Error> NonBlockingWriter::FlushIntoFile() noexcept
{
    const auto buffer_size = static_cast<uint64_t>(buffer_.size());

    const uint64_t left_over = buffer_size - number_of_flushed_bytes_;

    const uint64_t bytes_to_write = max_chunk_size_ <= left_over ? max_chunk_size_ : left_over;

    const auto flush_output = InternalFlush(bytes_to_write);

    if (!(flush_output.has_value()))
    {
        return amp::make_unexpected(bmw::mw::log::detail::Error::kUnknownError);
    }

    if (number_of_flushed_bytes_ == buffer_size)
    {
        buffer_flushed_ = Result::kDone;
    }

    return buffer_flushed_;
}

amp::expected<ssize_t, bmw::os::Error> NonBlockingWriter::InternalFlush(const uint64_t size_to_flush) noexcept
{
    const auto buffer_size = static_cast<uint64_t>(buffer_.size());
    if (number_of_flushed_bytes_ < buffer_size)
    {
        amp::expected<ssize_t, bmw::os::Error> num_of_bytes_written{};
        
         * construction above. */
        
        
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
        // NOLINTBEGIN(bmw-banned-function) it is among safety headers.
        // Needs ptr to access amp::span elements
        // 
        num_of_bytes_written = unistd_->write(file_handle_, &(buffer_.data()[number_of_flushed_bytes_]), size_to_flush);
        // NOLINTEND(bmw-banned-function) it is among safety headers.
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) used on span which is an array
        
        
        
        if (!num_of_bytes_written.has_value())
        {
            return amp::make_unexpected(num_of_bytes_written.error());
        }
        else
        {
            
            number_of_flushed_bytes_ += static_cast<uint64_t>(num_of_bytes_written.value());
            
        }
    }
    return number_of_flushed_bytes_;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
