/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/compiler/tf2xla/resource_operation_table.h"
#include "absl/algorithm/container.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/gtl/flatmap.h"

namespace tensorflow {
/*static*/ StringPiece XlaResourceOpInfo::XlaResourceOpKindToString(
    XlaResourceOpKind op_kind) {
  switch (op_kind) {
    case XlaResourceOpKind::kRead:
      return "Read";
    case XlaResourceOpKind::kWrite:
      return "Write";
    case XlaResourceOpKind::kReadWrite:
      return "Modify";
  }
}

static gtl::FlatMap<StringPiece, XlaResourceOpInfo>* CreateResourceOpInfoMap() {
  auto* result = new gtl::FlatMap<StringPiece, XlaResourceOpInfo>;

  auto add = [&](StringPiece op, XlaResourceOpKind op_kind,
                 XlaResourceKind resource_kind) {
    auto insert_result =
        result->insert({op, XlaResourceOpInfo(op_kind, resource_kind)});
    CHECK(insert_result.second);
  };

  auto kRead = XlaResourceOpKind::kRead;
  auto kWrite = XlaResourceOpKind::kWrite;
  auto kReadWrite = XlaResourceOpKind::kReadWrite;

  auto kVariable = XlaResourceKind::kVariable;
  auto kStack = XlaResourceKind::kStack;
  auto kTensorArray = XlaResourceKind::kTensorArray;

  // clang-format off
  add("AssignAddVariableOp"                  , kReadWrite, kVariable);
  add("AssignSubVariableOp"                  , kReadWrite, kVariable);
  add("AssignVariableOp"                     , kWrite,     kVariable);
  add("ReadVariableOp"                       , kRead,      kVariable);
  add("ResourceApplyAdaMax"                  , kReadWrite, kVariable);
  add("ResourceApplyAdadelta"                , kReadWrite, kVariable);
  add("ResourceApplyAdagrad"                 , kReadWrite, kVariable);
  add("ResourceApplyAdagradDA"               , kReadWrite, kVariable);
  add("ResourceApplyAdam"                    , kReadWrite, kVariable);
  add("ResourceApplyAddSign"                 , kReadWrite, kVariable);
  add("ResourceApplyCenteredRMSProp"         , kReadWrite, kVariable);
  add("ResourceApplyFtrl"                    , kReadWrite, kVariable);
  add("ResourceApplyFtrlV2"                  , kReadWrite, kVariable);
  add("ResourceApplyGradientDescent"         , kReadWrite, kVariable);
  add("ResourceApplyMomentum"                , kReadWrite, kVariable);
  add("ResourceApplyPowerSign"               , kReadWrite, kVariable);
  add("ResourceApplyProximalAdagrad"         , kReadWrite, kVariable);
  add("ResourceApplyProximalGradientDescent" , kReadWrite, kVariable);
  add("ResourceApplyRMSProp"                 , kReadWrite, kVariable);
  add("ResourceGather"                       , kRead,      kVariable);
  add("ResourceScatterAdd"                   , kReadWrite, kVariable);
  add("ResourceScatterDiv"                   , kReadWrite, kVariable);
  add("ResourceScatterMax"                   , kReadWrite, kVariable);
  add("ResourceScatterMin"                   , kReadWrite, kVariable);
  add("ResourceScatterMul"                   , kReadWrite, kVariable);
  add("ResourceScatterNdAdd"                 , kReadWrite, kVariable);
  add("ResourceScatterNdUpdate"              , kReadWrite, kVariable);
  add("ResourceScatterSub"                   , kReadWrite, kVariable);
  add("ResourceScatterUpdate"                , kReadWrite, kVariable);
  add("ResourceStridedSliceAssign"           , kReadWrite, kVariable);
  add("VarIsInitializedOp"                   , kRead,      kVariable);
  add("VariableShape"                        , kRead,      kVariable);

  add("StackV2"                              , kWrite,     kStack);
  add("StackCloseV2"                         , kRead,      kStack);
  add("StackPopV2"                           , kReadWrite, kStack);
  add("StackPushV2"                          , kReadWrite, kStack);

  add("TensorArrayV3"                        , kWrite,     kTensorArray);
  add("TensorArrayConcatV3"                  , kRead,      kTensorArray);
  add("TensorArrayGatherV3"                  , kRead,      kTensorArray);
  add("TensorArrayScatterV3"                 , kWrite,     kTensorArray);
  add("TensorArrayGradV3"                    , kRead,      kTensorArray);
  add("TensorArrayCloseV3"                   , kRead,      kTensorArray);
  add("TensorArrayReadV3"                    , kRead,      kTensorArray);
  add("TensorArraySizeV3"                    , kRead,      kTensorArray);
  add("TensorArraySplitV3"                   , kWrite,     kTensorArray);
  add("TensorArrayWriteV3"                   , kWrite,     kTensorArray);
  // clang-format on

  return result;
}

static const gtl::FlatMap<StringPiece, XlaResourceOpInfo>&
GetStaticResourceOpInfoMap() {
  static gtl::FlatMap<StringPiece, XlaResourceOpInfo>* op_info_map =
      CreateResourceOpInfoMap();
  return *op_info_map;
}

const XlaResourceOpInfo* GetResourceOpInfoForOp(absl::string_view op) {
  const gtl::FlatMap<StringPiece, XlaResourceOpInfo>& op_infos =
      GetStaticResourceOpInfoMap();
  auto it = op_infos.find(StringPiece(op.data(), op.length()));
  return it == op_infos.end() ? nullptr : &it->second;
}

namespace resource_op_table_internal {
std::vector<StringPiece> GetKnownResourceOps() {
  std::vector<StringPiece> result;
  for (const auto& p : GetStaticResourceOpInfoMap()) {
    result.push_back(absl::string_view(p.first));
  }
  absl::c_sort(result);
  return result;
}
}  // namespace resource_op_table_internal
}  // namespace tensorflow
