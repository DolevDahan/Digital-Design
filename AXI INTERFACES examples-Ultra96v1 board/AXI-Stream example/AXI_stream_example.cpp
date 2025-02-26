
#include <hls_math.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>



void stream_example(
    hls::stream<hls::axis<float, 0, 0, 0>> &input_stream,
    hls::stream<hls::axis<float, 0, 0, 0>> &output_stream 
) {
    #pragma HLS INTERFACE axis port=input_stream register_mode=off
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=return
    hls::axis<float, 0, 0, 0> data;
    while (!input_stream.empty()) {
        data = input_stream.read();
        data.data = data.data * 10.0f + 0.5f;
        
        // Write to output stream
        output_stream.write(data);

        // Exit on the last signal
        if (data.last) {
            break;
        }
    }
}