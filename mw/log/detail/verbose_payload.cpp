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


#include "platform/aas/mw/log/detail/verbose_payload.h"

#include "amp_assert.hpp"
#include "amp_utility.hpp"

#include <cstdint>
#include <cstring>
#include <limits>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

namespace
{
class ReservedData
{
  public:
    ReservedData(ByteVector& buffer, const std::size_t reserve_size) noexcept
        : buffer_(buffer), old_size_(buffer.size()), total_used_{0}, reserved_{0}
    {
        const std::size_t max_possible_length_to_copy = buffer_.capacity() - old_size_;

        if ((reserve_size == 0) || (reserve_size > max_possible_length_to_copy))
        {
            reserved_ = max_possible_length_to_copy;
        }
        else
        {
            reserved_ = reserve_size;
        }
        //  casting constant value to more capable type:
        constexpr auto max_signed_type_size = static_cast<std::size_t>(std::numeric_limits<const std::int32_t>::max());
        //  limit value to maximum positive value of signed integer because data types with both signed and unsigned
        //  size types are mixed
        reserved_ = std::min(max_signed_type_size, reserved_);
        buffer_.resize(old_size_ + reserved_);
    }
    ReservedData(ReservedData&&) noexcept = delete;
    ReservedData(const ReservedData&) noexcept = delete;
    ReservedData& operator=(ReservedData&&) noexcept = delete;
    ReservedData& operator=(const ReservedData&) noexcept = delete;

    ~ReservedData()
    {
        buffer_.resize(old_size_ + total_used_);  //  resize to fit only used data
    }

    void IncreaseUsed(std::size_t size) noexcept
    {
        const auto space_left = buffer_.capacity() - total_used_ - old_size_;
        const auto use_size = std::min(size, space_left);
        total_used_ += use_size;
    }

    amp::span<Byte> GetData() const noexcept
    {
        const auto buffer_index = old_size_ + total_used_;
        if (buffer_index < buffer_.size())
        {
            //  reserved_ is limited at construction time and total_used_ is derived from it and shall be always
            //  smaller:
            return {&buffer_[buffer_index],
                    static_cast<std::int32_t>(reserved_) - static_cast<std::int32_t>(total_used_)};
        }
        else
        {
            return {nullptr, 0UL};
        }
    }

  private:
    ByteVector& buffer_;
    const std::size_t old_size_;
    std::size_t total_used_;
    std::size_t reserved_;
};
}  //  anonymous namespace

VerbosePayload::VerbosePayload(const std::size_t max_size, ByteVector& buffer) noexcept : buffer_{buffer}
{
    buffer_.get().reserve(max_size);
}

void VerbosePayload::Put(const Byte* const data, const std::size_t length) noexcept
{
    if (length == 0)
    {
        return;
    }

    // data == nullptr is only problematic if length != 0
    const auto isPointerValid = data != nullptr;
    
    
    if (isPointerValid == false)
    {
        return;
    }
    
    

    amp::ignore = this->Put(
        
        [data](amp::span<Byte> dst_data) {
            
            if (dst_data.size() > 0)
            {
                const amp::span<const Byte> data_span{data, dst_data.size()};
                amp::ignore = std::copy(data_span.begin(), data_span.end(), dst_data.begin());
            }
            return dst_data.size();
        },
        length);
}


/* False positive:Function passes non-const reference to member variable to non-const object. */
std::size_t VerbosePayload::Put(const ReserveCallback callback, const std::size_t reserve_size) noexcept

{
    ReservedData reserved_data(buffer_.get(), reserve_size);
    const auto data = reserved_data.GetData();

    //  Execute user provided callback which should fill 'data' buffer and returned used space
    const auto new_written_data_size = callback(data);
    reserved_data.IncreaseUsed(new_written_data_size);
    return new_written_data_size;
}

amp::span<const std::uint8_t> VerbosePayload::GetSpan() const noexcept
{
    //  Checking for sign overflow. Limit data to maximum possible span size:
    using size_type = amp::span<const std::uint8_t>::size_type;
    constexpr auto max_size = std::numeric_limits<const size_type>::max();

    //  Suppress false positive issue: 
    
    const auto span_size = static_cast<size_type>(std::min(static_cast<std::size_t>(max_size), buffer_.get().size()));
    

    
    //  reinterpret_cast due to handling raw data:
    // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast) justification provided
    // reinterpret_cast due to handling raw data
    // 
    return {reinterpret_cast<const std::uint8_t*>(buffer_.get().data()), span_size};
    // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast) justification provided
    
}

void VerbosePayload::Reset() const noexcept
{
    buffer_.get().resize(0);
}

bool VerbosePayload::WillOverflow(const std::size_t length) const noexcept
{
    return length > RemainingCapacity();
}

std::size_t VerbosePayload::RemainingCapacity() const noexcept
{
    return buffer_.get().capacity() - buffer_.get().size();
}

void VerbosePayload::SetBuffer(ByteVector& buffer) noexcept
{
    buffer_ = buffer;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw
