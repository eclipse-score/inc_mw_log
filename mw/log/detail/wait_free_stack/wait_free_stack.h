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


#ifndef PLATFORM_AAS_MW_LOG_DETAIL_WAIT_FREE_STACK_H
#define PLATFORM_AAS_MW_LOG_DETAIL_WAIT_FREE_STACK_H

#include "amp_callback.hpp"
#include "amp_optional.hpp"
#include "amp_span.hpp"

#include <atomic>
#include <utility>
#include <vector>

namespace bmw
{
namespace mw
{
namespace log
{
namespace detail
{

template <typename Element>
using FindPredicate = amp::callback<bool(const Element&)>;

/// \brief Wait- and lock-free, push-only stack with fixed capacity.
template <typename Element>
class WaitFreeStack
{
  public:
    explicit WaitFreeStack(const size_t max_number_of_elements) noexcept;

    /// \brief Inserts an element if capacity is left.
    /// Returns a reference to the element in the stack if push was successful.
    amp::optional<std::reference_wrapper<Element>> TryPush(Element&& element) noexcept;

    /// \brief returns the first matching element
    /// Returns a reference to the element in the stack if element was found.
    amp::optional<std::reference_wrapper<Element>> Find(const FindPredicate<Element>&) noexcept;

    using AtomicIndex = std::atomic_size_t;
    using AtomicBool = std::atomic_bool;

    
  private:
    
    
    std::vector<amp::optional<Element>> elements_;
    std::vector<std::atomic_int> elements_written_;
    AtomicIndex write_index_;
    AtomicBool capacity_full_;
    
};

template <typename Element>
WaitFreeStack<Element>::WaitFreeStack(const size_t max_number_of_elements) noexcept
    : elements_(max_number_of_elements),
      elements_written_(max_number_of_elements),
      write_index_{},
      capacity_full_{false}
{
}

template <typename Element>
auto WaitFreeStack<Element>::TryPush(Element&& element) noexcept -> amp::optional<std::reference_wrapper<Element>>
{
    if (capacity_full_.load())
    {
        return amp::nullopt;
    }

    const auto current_write_index = write_index_.fetch_add(1);

    if (current_write_index >= elements_.size())
    {
        capacity_full_.store(true);
        return amp::nullopt;
    }

    elements_[current_write_index] = std::forward<Element>(element);

    std::atomic_thread_fence(std::memory_order_release);
    elements_written_[current_write_index].store(1);

    return elements_[current_write_index].value();
}

template <typename Element>
auto WaitFreeStack<Element>::Find(const FindPredicate<Element>& predicate) noexcept
    -> amp::optional<std::reference_wrapper<Element>>
{
    for (auto i = 0UL; (i < elements_.size()) && (i <= write_index_.load()); i++)
    {
        if (elements_written_[i].load() != 0)
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            if (predicate(elements_[i].value()) == true)
            {
                return elements_[i].value();
            }
        }
    }

    return amp::nullopt;
}

}  // namespace detail
}  // namespace log
}  // namespace mw
}  // namespace bmw

#endif
