#ifndef NAC_UTILS_H
#define NAC_UTILS_H

#include "common.h"

namespace nac{
namespace utils{

template<typename dtype>
dtype im2col_get_pixel(dtype *im, int height, int width, int channels,
                        int row, int col, int channel, int pad)
{
    row -= pad;
    col -= pad;

    if (row < 0 || col < 0 || row >= height || col >= width)
        return 0;
    return im[col + width*(row + height*channel)];
}

template<typename dtype>
void im2col_cpu(dtype* data_im,
    int channels,  int height,  int width,
    int ksize,  int stride, int pad, dtype* data_col)
{
    int c,h,w;
    int height_col = (height + 2*pad - ksize) / stride + 1;
    int width_col = (width + 2*pad - ksize) / stride + 1;

    int channels_col = channels * ksize * ksize;
    for (c = 0; c < channels_col; ++c) {
        int w_offset = c % ksize;
        int h_offset = (c / ksize) % ksize;
        int c_im = c / ksize / ksize;
        for (h = 0; h < height_col; ++h) {
            for (w = 0; w < width_col; ++w) {
                int im_row = h_offset + h * stride;
                int im_col = w_offset + w * stride;
                int col_index = (c * height_col + h) * width_col + w;
                data_col[col_index] = im2col_get_pixel(data_im, height, width, channels,
                        im_row, im_col, c_im, pad);
            }
        }
    }
}

inline int im2col_cpu_outsize(int channels,  int height,  int width, int ksize,  int stride, int pad)
{
    int height_col = (height + 2*pad - ksize) / stride + 1;
    int width_col = (width + 2*pad - ksize) / stride + 1;
    int channels_col = channels * ksize * ksize;

    return height_col*width_col*channels_col;
}

void gemm_cpu(int ta, int tb,
        int m, int n, int k, float alpha, 
        float *a, int lda, 
        float *b, int ldb,
        float *c, int ldc);

void activate_cpu(float * d, int num, activation_type act_type);
void activate_cpu(float * din, float * dout, int num, activation_type act_type);
void softmax_cpu(float *input, int n, int batch, int batch_offset, int groups, int group_offset, int stride, float temp, float *output);

void bin_to_file(const char * file_name, const void * ptr, int bytes);


}

}


#endif