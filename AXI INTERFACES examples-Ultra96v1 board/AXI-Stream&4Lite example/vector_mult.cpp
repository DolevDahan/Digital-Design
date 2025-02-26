#include <hls_stream.h>
#include <ap_axi_sdata.h>

typedef hls::axis<float, 0, 0, 0> AXI_STREAM_DATA;

void vector_mult(hls::stream<AXI_STREAM_DATA> &input_stream,
                 hls::stream<AXI_STREAM_DATA> &output_stream,
                 const float multiplier) {
#pragma HLS INTERFACE axis port=input_stream register_mode=off
#pragma HLS INTERFACE axis port=output_stream
#pragma HLS INTERFACE s_axilite port=multiplier bundle=CTRL_BUS
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS

    AXI_STREAM_DATA in_data, out_data;

    while (!input_stream.empty()) {
#pragma HLS PIPELINE II=1
        // Read input data
        in_data = input_stream.read();

        // Perform computation
        out_data.data = multiplier * in_data.data;
        out_data.keep = in_data.keep;
        out_data.strb = in_data.strb;
        out_data.last = in_data.last;

        // Write output data
        output_stream.write(out_data);

        if (in_data.last) {
            break; // Stop processing on TLAST
        }
    }
}
