// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <miller/VAST.h>
#include <miller/Clang.h>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Missing path to VAST IR module\n";
    return EXIT_FAILURE;
  }

  std::ifstream is(argv[1]);
  std::string data;
  for (std::string line; std::getline(is, line); line.clear()) {
    data.insert(data.end(), line.begin(), line.end());
  }

  auto maybe_module = miller::VASTModule::Deserialize(data);
  if (!maybe_module) {
    std::cerr << "Invalid VAST IR module\n";
    return EXIT_FAILURE;
  }

  miller::VASTModule module = std::move(maybe_module.value());

  auto maybe_ast = miller::ClangModule::Lift(module);
  if (!maybe_ast) {
    std::cerr << "Could not lift VAST IR module into an AST\n";
    return EXIT_FAILURE;
  }

  miller::ClangModule ast = std::move(maybe_ast.value());

  (void) ast;

  return EXIT_SUCCESS;
}
