// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#pragma once

#include <memory>
#include <optional>
#include <string_view>

namespace vast {

}  // namespace vast
namespace miller {

class ClangModule;
class VASTModuleImpl;

class VASTModule {
  friend class ClangModule;

  std::shared_ptr<VASTModuleImpl> impl;

  VASTModule(void) = delete;

  inline VASTModule(std::shared_ptr<VASTModuleImpl> impl_)
      : impl(std::move(impl_)) {}

 public:
  ~VASTModule(void);

  VASTModule(VASTModule &&) noexcept = default;
  VASTModule &operator=(VASTModule &&) noexcept = default;

  static std::optional<VASTModule> Deserialize(std::string_view data);
};

}  // namespace miller
