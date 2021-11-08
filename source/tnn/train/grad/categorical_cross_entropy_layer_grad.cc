// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

// author: sanerzheng@tencent.com

#include "tnn/train/grad/layer_grad.h"
#include "tnn/train/operations/op_builder.h"

namespace TNN_NS {
namespace train {
DECLARE_LAYER_GRAD(CategoricalCrossEntropy, LAYER_CATEGORICAL_CROSSENTROPY);

Status CategoricalCrossEntropyLayerGrad::OnGrad(const BaseLayer *layer, TrainContext &context) {
    auto inputs  = layer->input_blobs_;
    auto outputs = layer->output_blobs_;
    if (inputs.size() != 2 || outputs.size() != 1) {
        return Status(TNN_TRAIN_ERROR, "input size or output size not match in CategoricalCrossEntropyLayerGrad");
    }
    auto input0_dims = inputs[0]->GetBlobDesc().dims;
    auto input1_dims = inputs[1]->GetBlobDesc().dims;
    auto output_dims = outputs[0]->GetBlobDesc().dims;
    if (!DimsVectorUtils::Equal(input1_dims, input0_dims) || !DimsVectorUtils::Equal(input1_dims, output_dims)) {
        return Status(TNN_TRAIN_ERROR, "input dims and output dims not match in CategoricalCrossEntropyLayerGrad");
    }
    auto input0_data_type = inputs[0]->GetBlobDesc().data_type;
    auto input1_data_type = inputs[1]->GetBlobDesc().data_type;
    auto output_data_type = outputs[0]->GetBlobDesc().data_type;
    if (input0_data_type != input1_data_type || input1_data_type != output_data_type) {
        return Status(TNN_TRAIN_ERROR,
                      "input datatype and output datatype not match in CategoricalCrossEntropyLayerGrad");
    }
    if (output_data_type != DATA_TYPE_BFP16 && output_data_type != DATA_TYPE_FLOAT) {
        return Status(TNN_TRAIN_ERROR, "output datatype not match in CategoricalCrossEntropyLayerGrad");
    }
    auto data_format = outputs[0]->GetBlobDesc().data_format;
    ParamWrapper pw0(inputs[0]);
    ParamWrapper pw1(inputs[1]);
    // x0 is logits, x1 is true labels
    // y = -x1*log(x0)
    // dy/dx0 = -x1/x0
    // dy/dx1 = -log(x0)
    ParamWrapper grad0 = _Neg(_Div(pw1, pw0, context), context);
    ParamWrapper grad1 = _Neg(_Log(pw0, context), context);
    auto iter_output   = context.backward_grads_blob.find(outputs[0]);
    if (iter_output == context.backward_grads_blob.end()) {
        return Status(TNN_TRAIN_ERROR, "CategoricalCrossEntropyLayerGrad output grad not find");
    }
    grad0 = _Mul(grad0, ParamWrapper(iter_output->second), context);
    grad1 = _Mul(grad1, ParamWrapper(iter_output->second), context);
    if (!grad0.IsRawbufferSharedPtr() || !grad1.IsRawbufferSharedPtr()) {
        return Status(TNN_TRAIN_ERROR, "Calcute CategoricalCrossEntropyLayerGrad error");
    }
    UpdateGradValue(inputs[0], grad0.GetRawbufferSharedPtr(), context);
    UpdateGradValue(inputs[1], grad1.GetRawbufferSharedPtr(), context);
    return Status(TNN_OK);
}
REGISTER_LAYER_GRAD(CategoricalCrossEntropy, LAYER_CATEGORICAL_CROSSENTROPY);

} // namespace train
} // namespace TNN_NS