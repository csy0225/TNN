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

#import <CoreML/CoreML.h>
#import <Metal/Metal.h>
#import <Foundation/Foundation.h>
#import "../../../../include/tnn/core/macro.h"
#import "../../../../include/tnn/core/status.h"

#include <string>
#include <vector>
#include "Model.pb-c.h"

#ifndef TNN_COREML_TEST
#define TNN_COREML_TEST 0
#endif

@interface CoreMLModel : NSObject
@property(nonatomic, strong, readonly) NSString *cachePath;
@property(nonatomic, strong, readonly) NSString *ID;
@property(nonatomic, strong, readonly) MLModel* model API_AVAILABLE(ios(12.0), macosx(10.14));

- (instancetype)initWithCachePath:(std::string)path ID:(std::string) ID;

- (TNN_NS::Status)buildFromCache;
- (TNN_NS::Status)buildFromProtoBuf:(CoreML__Specification__Model*)model;
- (TNN_NS::Status)cleanup;
@end

