// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <ostream>
#include <string>

#include "paddle/fluid/framework/executor.h"
#include "paddle/fluid/framework/program_desc.h"
#include "paddle/phi/core/dense_tensor.h"
#include "paddle/phi/core/enforce.h"

namespace paddle {
namespace jit {

using Variable = paddle::framework::Variable;
using VariableNameMap = std::map<std::string, Variable>;
using DenseTensor = phi::DenseTensor;

class Argument {
 public:
  explicit Argument(const std::string &name, bool is_out = false);

  const std::string &Name() const;

 private:
  std::string name_;
  // paddle::optional<Variable> default_val_;
  bool is_output_;
};

class FunctionSchema {
 public:
  FunctionSchema() = default;

  std::vector<std::string> GetInputArgNames();

  std::vector<std::string> GetOutputArgNames();

  void AddInputArg(std::string name);

  void AddOutputArg(std::string name);

 private:
  // input_args and output_args are ordered
  std::vector<Argument> input_args;
  std::vector<Argument> output_args;
};

// TODO(dev): make it as abstract class
class BaseFunction {
 public:
  BaseFunction(const framework::ProgramDesc &program_desc,
               const std::vector<std::string> &param_names,
               const VariableNameMap &params_dict,
               const phi::Place &place);

  virtual ~BaseFunction() {}

  virtual std::vector<Variable> operator()(
      const std::vector<Variable> &inputs) = 0;

 protected:
  void FetchOutput(std::vector<Variable> *outs);

  void ShareInputsIntoScope(const std::vector<Variable> &vars);

  void ShareParamsIntoScope(const std::vector<std::string> &param_names,
                            const VariableNameMap &params_dict);

  void RemoveFeedFetch();

 protected:
  framework::ProgramDesc program_desc_;
  FunctionSchema schema_;
  // global_scope place params
  framework::Scope scope_;
  phi::Place place_;
};

}  // namespace jit
}  // namespace paddle
