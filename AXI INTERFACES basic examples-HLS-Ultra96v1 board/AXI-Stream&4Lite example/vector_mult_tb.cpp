#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <iostream>
#include <iomanip>

typedef hls::axis<float, 0, 0, 0> AXI_STREAM_DATA;

void vector_mult(hls::stream<AXI_STREAM_DATA> &input_stream,
                 hls::stream<AXI_STREAM_DATA> &output_stream,
                 const float multiplier);

int main() {
    hls::stream<AXI_STREAM_DATA> input_stream;
    hls::stream<AXI_STREAM_DATA> output_stream;

    const float multiplier = 3.0; // Example constant multiplier

    // Manually write 5 inputs to the stream
    AXI_STREAM_DATA input_data;

    input_data.data = 1.5f;
    input_data.keep = 0xF;  // Keep all bytes valid
    input_data.strb = 0xF;  // Strobe all bytes valid
    input_data.last = 0;    // Not the last element
    input_stream.write(input_data);

    input_data.data = 2.5f;
    input_data.last = 0;    // Not the last element
    input_stream.write(input_data);

    input_data.data = 3.5f;
    input_data.last = 0;    // Not the last element
    input_stream.write(input_data);

    input_data.data = 4.5f;
    input_data.last = 0;    // Not the last element
    input_stream.write(input_data);

    input_data.data = 5.5f;
    input_data.last = 1;    // Set TLAST for the last element
    input_stream.write(input_data);

    // Call the function
    vector_mult(input_stream, output_stream, multiplier);

    // Print output
    std::cout << "Output Data:\n";
    for (int i = 0; i < 5; ++i) {
        AXI_STREAM_DATA output_data = output_stream.read();
        std::cout << "Output: " << output_data.data
                  << ", TLAST: " << output_data.last << "\n";
    }

    return 0;
}
