
#include <hls_math.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <stdio.h>
#include <string.h>

#define N  5

void axi_stream_master(
    volatile float *a,
    hls::stream<hls::axis<float, 0, 0, 0>> &input_stream,
    hls::stream<hls::axis<float, 0, 0, 0>> &output_stream 
) {
    #pragma HLS INTERFACE m_axi port=a depth=N offset=slave
    #pragma HLS INTERFACE s_axilite port=return
    #pragma HLS INTERFACE axis port=input_stream register_mode=off
    #pragma HLS INTERFACE axis port=output_stream

    int i;
    float buff[N];
    memcpy(buff, (const float*)a, N*sizeof(float));
  
    hls::axis<float, 0, 0, 0> data;
    while (!input_stream.empty()) {
        data = input_stream.read();
        data.data = data.data * 10.0f + 0.5f + buff[0];

        // Write to output stream
        output_stream.write(data);

        // Exit on the last signal
        if (data.last) {
            break;
        }
    }
}

