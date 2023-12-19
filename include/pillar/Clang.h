// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#pragma once

#include <memory>
#include <optional>
#include <string_view>

namespace vast
{

} // namespace vast
namespace pillar
{

  class ClangModuleImpl;
  class VASTModule;

  class ClangModule
  {
    friend class VASTModule;

    std::shared_ptr<ClangModuleImpl> impl;

    ClangModule(void) = delete;

    inline ClangModule(std::shared_ptr<ClangModuleImpl> impl_)
        : impl(std::move(impl_)) {}

  public:
    ~ClangModule(void);

    ClangModule(ClangModule &&) noexcept = default;
    ClangModule &operator=(ClangModule &&) noexcept = default;

    static std::optional<ClangModule> Lift(const VASTModule &module);
  };

} // namespace pillar
