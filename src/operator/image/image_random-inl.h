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
* \file image_random-inl.h
* \brief
* \author
*/
#ifndef MXNET_OPERATOR_IMAGE_IMAGE_RANDOM_INL_H_
#define MXNET_OPERATOR_IMAGE_IMAGE_RANDOM_INL_H_

#include <vector>
#include <cmath>
#include <mxnet/base.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include "../mxnet_op.h"
#include "image_common.h"
#include "../../operator/operator_common.h"
#include "../../operator/linalg.h"

namespace mxnet {
namespace op {


enum ImageRandomResource { kRandom };

template<typename xpu>
static void RandomFlip(const nnvm::NodeAttrs &attrs,
                       const OpContext &ctx,
                       const std::vector<TBlob> &inputs,
                       const std::vector<OpReqType> &req,
                       const std::vector<TBlob> &outputs) {
}
template<typename xpu>
static void ToTensor(const nnvm::NodeAttrs &attrs,
                     const OpContext &ctx,
                     const std::vector<TBlob> &inputs,
                     const std::vector<OpReqType> &req,
                     const std::vector<TBlob> &outputs) {
}
template<typename xpu>
static void Normalize(const nnvm::NodeAttrs &attrs,
                      const OpContext &ctx,
                      const std::vector<TBlob> &inputs,
                      const std::vector<OpReqType> &req,
                      const std::vector<TBlob> &outputs) {
}

struct RandomBrightnessParam : public dmlc::Parameter<RandomBrightnessParam> {
  float max_brightness;
  DMLC_DECLARE_PARAMETER(RandomBrightnessParam) {
    DMLC_DECLARE_FIELD(max_brightness)
    .set_default(0.0)
    .describe("Max Brightness.");
  }
};

template<typename xpu>
static void RandomBrightness(const nnvm::NodeAttrs &attrs,
                             const OpContext &ctx,
                             const std::vector<TBlob> &inputs,
                             const std::vector<OpReqType> &req,
                             const std::vector<TBlob> &outputs) {
  using namespace mshadow;
  auto input = inputs[0];
  auto output = outputs[0];
  int channel = input.shape_[0];
  int hight = input.shape_[1];
  int weight = input.shape_[2];
  Stream<xpu> *s = ctx.get_stream<xpu>();
  Random<xpu> *prnd = ctx.requested[kRandom].get_random<xpu, real_t>(s);

  const RandomBrightnessParam &param = nnvm::get<RandomBrightnessParam>(attrs.parsed);
  float alpha_b = 1.0 + std::uniform_real_distribution<float>(-param.max_brightness, param.max_brightness)(prnd->GetRndEngine());
  MSHADOW_TYPE_SWITCH(input.type_flag_, DType, {
    MXNET_ASSIGN_REQ_SWITCH(req[0], Req, {
      mxnet_op::Kernel<mxnet_op::op_with_req<mshadow::op::mul, Req>, xpu>::Launch(
        s, inputs[0].Size(), outputs[0].dptr<DType>(), inputs[0].dptr<DType>(), DType(alpha_b));
    });
  });

}

struct RandomContrastParam : public dmlc::Parameter<RandomContrastParam> {
  float max_contrast;
  DMLC_DECLARE_PARAMETER(RandomContrastParam) {
    DMLC_DECLARE_FIELD(max_contrast)
    .set_default(0.0)
    .describe("Max Contrast.");
  }
};

/*! \brief mul_add operator */
struct mul_add {
  /*! \brief map a, b, c to result using defined operation */
  template<typename DType>
  MSHADOW_XINLINE static DType Map(DType a, DType b, DType c) {
    return a * b + c;
  }
};

template<typename xpu>
static void RandomContrast(const nnvm::NodeAttrs &attrs,
                           const OpContext &ctx,
                           const std::vector<TBlob> &inputs,
                           const std::vector<OpReqType> &req,
                           const std::vector<TBlob> &outputs) {
  using namespace mshadow;
  auto input = inputs[0];
  auto output = outputs[0];
  int channel = input.shape_[0];
  int hight = input.shape_[1];
  int weight = input.shape_[2];
  Stream<xpu> *s = ctx.get_stream<xpu>();
  Random<xpu> *prnd = ctx.requested[kRandom].get_random<xpu, real_t>(s);


  const RandomContrastParam &param = nnvm::get<RandomContrastParam>(attrs.parsed);
  float alpha_c = 1.0 + std::uniform_real_distribution<float>(-param.max_contrast, param.max_contrast)(prnd->GetRndEngine());

  const float R2YF = 0.299f;
  const float G2YF = 0.587f;
  const float B2YF = 0.114f;
  static const float coeffs0[] = { R2YF, G2YF, B2YF };

  MSHADOW_TYPE_SWITCH(input.type_flag_, DType, {
    auto input_3d = input.get<xpu, 3, DType>(s);
    DType sum = (DType)0.0;
    for (int c = 0; c < channel; ++c) {
      for (int h = 0; h < hight; ++h) {
        for (int w = 0; w < weight; ++w) {
          sum += input_3d[c][h][w] * coeffs0[c];
        }
      }
    }
    float gray_mean = sum / (float)(hight * weight);
    float beta = (1 - alpha_c) * gray_mean;

    MXNET_ASSIGN_REQ_SWITCH(req[0], Req, {
      mxnet_op::Kernel<mxnet_op::op_with_req<mul_add, Req>, xpu>::Launch(
        s, inputs[0].Size(), outputs[0].dptr<DType>(), inputs[0].dptr<DType>(), DType(alpha_c), DType(beta));
    });

  });

}

struct RandomSaturationParam : public dmlc::Parameter<RandomSaturationParam> {
  float max_saturation;
  DMLC_DECLARE_PARAMETER(RandomSaturationParam) {
    DMLC_DECLARE_FIELD(max_saturation)
    .set_default(0.0)
    .describe("Max Saturation.");
  }
};

template<typename xpu>
static void RandomSaturation(const nnvm::NodeAttrs &attrs,
                             const OpContext &ctx,
                             const std::vector<TBlob> &inputs,
                             const std::vector<OpReqType> &req,
                             const std::vector<TBlob> &outputs) {
  using namespace mshadow;
  auto input = inputs[0];
  auto output = outputs[0];
  int channel = input.shape_[0];
  int hight = input.shape_[1];
  int weight = input.shape_[2];
  Stream<xpu> *s = ctx.get_stream<xpu>();
  Random<xpu> *prnd = ctx.requested[kRandom].get_random<xpu, real_t>(s);
  const RandomSaturationParam &param = nnvm::get<RandomSaturationParam>(attrs.parsed);
  float alpha_s = 1.0 + std::uniform_real_distribution<float>(-param.max_saturation, param.max_saturation)(prnd->GetRndEngine());
  float alpha_o = 1 - alpha_s;
  const float R2YF = 0.299f;
  const float G2YF = 0.587f;
  const float B2YF = 0.114f;
  static const float coeffs0[] = { R2YF, G2YF, B2YF };


  MSHADOW_TYPE_SWITCH(input.type_flag_, DType, {
    MXNET_ASSIGN_REQ_SWITCH(req[0], Req, {
      auto input_3d =  input.get<xpu, 3, DType>(s);
      auto output_3d = output.get<xpu, 3, DType>(s);
      switch (channel) {
        case 1:
          Assign(output_3d, Req, input_3d)
          break;
        case 3:
          for (int h = 0; h < hight; ++h) {
            for (int w = 0; w < weight; ++w) {
              float gray = input_3d[0][h][w] * R2YF + input_3d[1][h][w] * G2YF + input_3d[2][h][w] * B2YF;
              Assign(output_3d[0][h][w], Req, DType(gray * alpha_s + input_3d[0][h][w] * alpha_o))
            }
          }
          break;
        default:
          LOG(FATAL) << "not support channel" << channel;

      }
    });
  });

}

struct RandomHueParam : public dmlc::Parameter<RandomHueParam> {
  float max_hue;
  DMLC_DECLARE_PARAMETER(RandomHueParam) {
    DMLC_DECLARE_FIELD(max_hue)
    .set_default(0.0)
    .describe("Max Hue.");
  }
};

template <typename DType> static
void RGB2HLSConvert(const DType src_r,
                    const DType src_g,
                    const DType src_b,
                    DType &dst_h,
                    DType &dst_l,
                    DType &dst_s
                   ) {
  DType b = src_b, g = src_g, r = src_r;
  DType h = 0.f, s = 0.f, l;
  DType vmin;
  DType vmax;
  DType diff;

  vmax = vmin = r;
  vmax = fmax(vmax, g);
  vmax = fmax(vmax, b);
  vmin = fmin(vmin, g);
  vmin = fmin(vmin, b);

  diff = vmax - vmin;
  l = (vmax + vmin) * 0.5f;

  if (diff > std::numeric_limits<DType>::epsilon()) {
    s = (l < 0.5f) * diff / (vmax + vmin);
    s += (l >= 0.5f) * diff / (2.0f - vmax - vmin);

    diff = 60.f / diff;

    h = (vmax == r) * (g - b) * diff;
    h += (vmax != r && vmax == g) * ((b - r) * diff + 120.f);
    h += (vmax != r && vmax != g) * ((r - g) * diff + 240.f);
    h += (h < 0.f) * 360.f;
  }

  dst_h = h;
  dst_l = l;
  dst_s = s;
}


static  int c_HlsSectorData[6][3] = { { 1, 3, 0 }, { 1, 0, 2 }, { 3, 0, 1 }, { 0, 2, 1 }, { 0, 1, 3 }, { 2, 1, 0 } };

template <typename DType>  static  void HLS2RGBConvert(const DType src_h,
    const DType src_l,
    const DType src_s,
    DType &dst_r,
    DType &dst_g,
    DType &dst_b) {


  float h = src_h, l = src_l, s = src_s;
  float b = l, g = l, r = l;

  if (s != 0) {
    float p2 = (l <= 0.5f) * l * (1 + s);
    p2 += (l > 0.5f) * (l + s - l * s);
    float p1 = 2 * l - p2;

    if (h < 0)
        do h += 6; while (h < 0);
    else if (h >= 6)
        do h -= 6; while (h >= 6);

    int sector = (int)(h);

    h -= sector;

    float tab[4];
    tab[0] = p2;
    tab[1] = p1;
    tab[2] = p1 + (p2 - p1) * (1 - h);
    tab[3] = p1 + (p2 - p1) * h;

    b = tab[c_HlsSectorData[sector][0]];
    g = tab[c_HlsSectorData[sector][1]];
    r = tab[c_HlsSectorData[sector][2]];
  }

  dst_b = b;
  dst_g = g;
  dst_r = r;
}

template<typename xpu, typename DType>
static  void RandomHueKernal(TBlob &input, TBlob &output, Stream<xpu> *s, int hight, int weight, DType alpha) {
  auto input_3d = input.get<xpu, 3, DType>(s);
  auto output_3d = output.get<xpu, 3, DType>(s);
  for (int h_index = 0; h_index < hight; ++h_index) {
    for (int w_index = 0; w_index < weight; ++w_index) {
      DType h;
      DType l;
      DType s;
      RGB2HLSConvert(input_3d[0][h_index][w_index],
                     input_3d[1][h_index][w_index],
                     input_3d[2][h_index][w_index],
                     h, l, s
                    );
      h += alpha;
      h = std::max(DType(0), std::min(DType(180), h));

      HLS2RGBConvert(
        h, l, s,
        output_3d[0][h_index][w_index],
        output_3d[1][h_index][w_index],
        output_3d[2][h_index][w_index]
      );
    }
  }
};
template<typename xpu>
static void RandomHue(const nnvm::NodeAttrs &attrs,
                      const OpContext &ctx,
                      const std::vector<TBlob> &inputs,
                      const std::vector<OpReqType> &req,
                      const std::vector<TBlob> &outputs) {
  using namespace mshadow;
  auto input = inputs[0];
  auto output = outputs[0];
  int channel = input.shape_[0];
  int hight = input.shape_[1];
  int weight = input.shape_[2];
  Stream<xpu> *s = ctx.get_stream<xpu>();
  Random<xpu> *prnd = ctx.requested[kRandom].get_random<xpu, real_t>(s);

  const RandomHueParam &param = nnvm::get<RandomHueParam>(attrs.parsed);
  float alpha =  std::uniform_real_distribution<float>(-param.max_hue, param.max_hue)(prnd->GetRndEngine());
  auto output_float = output.get<xpu, 3, float>(s);

  MSHADOW_TYPE_SWITCH(input.type_flag_, DType, {
    RandomHueKernal<xpu, DType>(input, output, s, hight, weight, alpha);
  });
}

template<typename xpu>
static void RandomColorJitter(const nnvm::NodeAttrs &attrs,
                              const OpContext &ctx,
                              const std::vector<TBlob> &inputs,
                              const std::vector<OpReqType> &req,
                              const std::vector<TBlob> &outputs) {
}

template<typename xpu>
static void RandomLighting(const nnvm::NodeAttrs &attrs,
                           const OpContext &ctx,
                           const std::vector<TBlob> &inputs,
                           const std::vector<OpReqType> &req,
                           const std::vector<TBlob> &outputs) {
}




} // namespace op
} // namespace mxnet

#endif  // MXNET_OPERATOR_IMAGE_IMAGE_RANDOM_INL_H_
