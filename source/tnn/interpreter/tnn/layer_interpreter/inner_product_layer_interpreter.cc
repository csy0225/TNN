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

#include "tnn/interpreter/tnn/layer_interpreter/abstract_layer_interpreter.h"

#include <stdlib.h>

namespace TNN_NS {

DECLARE_LAYER_INTERPRETER(InnerProduct, LAYER_INNER_PRODUCT);

Status InnerProductLayerInterpreter::InterpretProto(str_arr layer_cfg_arr, int start_index, LayerParam** param) {
    InnerProductLayerParam* layer_param = new InnerProductLayerParam();
    *param                              = layer_param;
    int index                           = start_index;

    layer_param->num_output = atoi(layer_cfg_arr[index++].c_str());
    layer_param->has_bias   = atoi(layer_cfg_arr[index++].c_str());
    layer_param->transpose  = atoi(layer_cfg_arr[index++].c_str());
    layer_param->axis       = atoi(layer_cfg_arr[index++].c_str());

    return TNN_OK;
}

Status InnerProductLayerInterpreter::InterpretResource(Deserializer& deserializer, LayerResource** resource) {
    InnerProductLayerResource* layer_res = new InnerProductLayerResource();
    *resource                            = layer_res;

    std::string layer_name = deserializer.GetString();

    RawBuffer weights;
    deserializer.GetRaw(weights);
    layer_res->weight_handle = weights;

    RawBuffer bias;
    deserializer.GetRaw(bias);
    layer_res->bias_handle = bias;

    if (weights.GetDataType() == DATA_TYPE_INT8) {
        // quantized
        RawBuffer scale;
        deserializer.GetRaw(scale);
        layer_res->scale_handle = scale;
    }

    return TNN_OK;
}

Status InnerProductLayerInterpreter::SaveProto(std::ostream& output_stream, LayerParam* param) {
    InnerProductLayerParam* layer_param = dynamic_cast<InnerProductLayerParam*>(param);
    if (nullptr == layer_param) {
        LOGE("invalid layer param to save\n");
        return Status(TNNERR_NULL_PARAM, "invalid layer param to save");
    }

    output_stream << layer_param->num_output << " ";
    output_stream << layer_param->has_bias << " ";
    output_stream << layer_param->transpose << " ";
    output_stream << layer_param->axis << " ";

    return TNN_OK;
}

Status InnerProductLayerInterpreter::SaveResource(Serializer& serializer, LayerParam* param, LayerResource* resource) {
    InnerProductLayerParam* layer_param = dynamic_cast<InnerProductLayerParam*>(param);
    if (nullptr == layer_param) {
        LOGE("invalid layer param to save\n");
        return Status(TNNERR_NULL_PARAM, "invalid layer param to save");
    }
    InnerProductLayerResource* layer_res = dynamic_cast<InnerProductLayerResource*>(resource);
    if (nullptr == layer_res) {
        LOGE("invalid layer res to save\n");
        return Status(TNNERR_NULL_PARAM, "invalid layer res to save");
    }

    serializer.PutString(layer_param->name);
    serializer.PutRaw(layer_res->weight_handle);
    serializer.PutRaw(layer_res->bias_handle);

    if (layer_param->quantized) {
        serializer.PutRaw(layer_res->scale_handle);
    }

    return TNN_OK;
}

REGISTER_LAYER_INTERPRETER(InnerProduct, LAYER_INNER_PRODUCT);

}  // namespace TNN_NS
