/*
* Licensed to the Apache Software Foundation (ASF) under one
* or more contributor license agreements.  See the NOTICE file
* distributed with this work for additional information
* regarding copyright ownership.  The ASF licenses this file
* to you under the Apache License, Version 2.0 (the
* "License"); you may not use this file except in compliance
* with the License.  You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied.  See the License for the
* specific language governing permissions and limitations
* under the License.
*/

/*!
* \file image_random.cc
* \brief
* \author
*/

#include <mxnet/base.h>
#include "./image_random-inl.h"
#include "../operator_common.h"
#include "../elemwise_op_common.h"



namespace mxnet {
namespace op {

DMLC_REGISTER_PARAMETER(RandomBrightnessParam);
NNVM_REGISTER_OP(_image_random_brightness)
.describe(R"code()code" ADD_FILELINE)
.set_num_inputs(1)
.set_num_outputs(1)
.set_attr_parser(ParamParser<RandomBrightnessParam>)
.set_attr<FResourceRequest>("FResourceRequest", [](const NodeAttrs& attrs) { 
  return std::vector<ResourceRequest>{ResourceRequest::kRandom}; 
})
.set_attr<nnvm::FInferShape>("FInferShape", ElemwiseShape<1, 1>)
.set_attr<nnvm::FInferType>("FInferType", ElemwiseType<1, 1>)
.set_attr<FCompute>("FCompute<cpu>", RandomBrightness<cpu>)
.set_attr<nnvm::FGradient>("FGradient", ElemwiseGradUseNone{ "_copy" })
.add_argument("data", "NDArray-or-Symbol", "The input.")
.add_arguments(RandomBrightnessParam::__FIELDS__());

DMLC_REGISTER_PARAMETER(RandomContrastParam);
NNVM_REGISTER_OP(_image_random_contrast)
.describe(R"code()code" ADD_FILELINE)
.set_num_inputs(1)
.set_num_outputs(1)
.set_attr_parser(ParamParser<RandomContrastParam>)
.set_attr<FResourceRequest>("FResourceRequest", [](const NodeAttrs& attrs) {
  return std::vector<ResourceRequest>{ResourceRequest::kRandom};
})
.set_attr<nnvm::FInferShape>("FInferShape", ElemwiseShape<1, 1>)
.set_attr<nnvm::FInferType>("FInferType", ElemwiseType<1, 1>)
.set_attr<FCompute>("FCompute<cpu>", RandomContrast<cpu>)
.set_attr<nnvm::FGradient>("FGradient", ElemwiseGradUseNone{ "_copy" })
.add_argument("data", "NDArray-or-Symbol", "The input.")
.add_arguments(RandomContrastParam::__FIELDS__());

DMLC_REGISTER_PARAMETER(RandomSaturationParam);
NNVM_REGISTER_OP(_image_random_saturation)
.describe(R"code()code" ADD_FILELINE)
.set_num_inputs(1)
.set_num_outputs(1)
.set_attr_parser(ParamParser<RandomSaturationParam>)
.set_attr<FResourceRequest>("FResourceRequest", [](const NodeAttrs& attrs) {
  return std::vector<ResourceRequest>{ResourceRequest::kRandom};
})
.set_attr<nnvm::FInferShape>("FInferShape", ElemwiseShape<1, 1>)
.set_attr<nnvm::FInferType>("FInferType", ElemwiseType<1, 1>)
.set_attr<FCompute>("FCompute<cpu>", RandomSaturation<cpu>)
.set_attr<nnvm::FGradient>("FGradient", ElemwiseGradUseNone{ "_copy" })
.add_argument("data", "NDArray-or-Symbol", "The input.")
.add_arguments(RandomSaturationParam::__FIELDS__());

DMLC_REGISTER_PARAMETER(RandomHueParam);
NNVM_REGISTER_OP(_image_random_hue)
.describe(R"code()code" ADD_FILELINE)
.set_num_inputs(1)
.set_num_outputs(1)
.set_attr_parser(ParamParser<RandomHueParam>)
.set_attr<FResourceRequest>("FResourceRequest", [](const NodeAttrs& attrs) {
  return std::vector<ResourceRequest>{ResourceRequest::kRandom};
})
.set_attr<nnvm::FInferShape>("FInferShape", ElemwiseShape<1, 1>)
.set_attr<nnvm::FInferType>("FInferType", ElemwiseType<1, 1>)
.set_attr<FCompute>("FCompute<cpu>", RandomHue<cpu>)
.set_attr<nnvm::FGradient>("FGradient", ElemwiseGradUseNone{ "_copy" })
.add_argument("data", "NDArray-or-Symbol", "The input.")
.add_arguments(RandomHueParam::__FIELDS__());

}
}
