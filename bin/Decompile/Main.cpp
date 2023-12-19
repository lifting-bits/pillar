// Copyright (c) 2023-present, Trail of Bits, Inc.
// All rights reserved.
//
// This source code is licensed in accordance with the terms specified in
// the LICENSE file found in the root directory of this source tree.

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
// #include <pillar/VAST.h>
// #include <pillar/Clang.h>
#include "../../include/pillar/VAST.h"
#include "../../include/pillar/Clang.h"
#include <string>
#include <optional>

using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cerr << "Missing path to VAST IR module\n";
    return EXIT_FAILURE;
  }

  string ir_file_name = argv[1];
  ifstream is(ir_file_name);
  string ir_data;
  for (string line; getline(is, line); line.clear())
  {
    ir_data.insert(ir_data.end(), line.begin(), line.end());
  }
  auto maybe_module = pillar::VASTModule::Deserialize(ir_data);
  if (!maybe_module)
  {
    cerr << "Invalid VAST IR module\n";
    return EXIT_FAILURE;
  }

  pillar::VASTModule module = move(maybe_module.value());

  auto maybe_ast = pillar::ClangModule::Lift(module);
  if (!maybe_ast)
  {
    cerr << "Could not lift VAST IR module into an AST\n";
    return EXIT_FAILURE;
  }

  pillar::ClangModule ast = move(maybe_ast.value());

  (void)ast;

  return EXIT_SUCCESS;
}
