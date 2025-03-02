
#include <hls_math.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <stdio.h>
#include <string.h>
#include <hls_stream.h>
#include <iostream>

#define N  5

void axi_stream_master(
    volatile float *a,
    hls::stream<hls::axis<float, 0, 0, 0>> &input_stream,
    hls::stream<hls::axis<float, 0, 0, 0>> &output_stream 
);

int main() {
    
    float A[N] = {183.40f, 2.0f, 3.0f, 4.0f, 5.0f};
    float input_current[N] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    hls::stream<hls::axis<float, 0, 0, 0>> input_stream;
    hls::stream<hls::axis<float, 0, 0, 0>> output_stream;

        std::cout << "Input Current:" << std::endl;
    for (int i = 0; i < N; i++) {
        hls::axis<float, 0, 0, 0> input_signal;
        input_signal.data = input_current[i];
        input_signal.keep = -1;   // Keep all bytes (default value)
        input_signal.strb = -1;   // Strobe all bytes (default value)
        input_signal.user = {};   // Initialize `user` to default
        input_signal.last = (i == N - 1) ? 1 : 0; // Mark the last signal
        input_signal.id = {};     // Initialize `id` to default
        input_signal.dest = {};   // Initialize `dest` to default

        input_stream.write(input_signal);
        std::cout << "Input Val = " << i << ", i=" << input_current[i] << std::endl;
    }

    //Call the hardware function
    axi_stream_master(A, input_stream, output_stream);

    // Read and print the output voltage
    std::cout << "\n\n\nOutput stream:" << std::endl;
    while (!output_stream.empty()) {
        hls::axis<float, 0, 0, 0> output_signal;
        output_signal = output_stream.read();
        std::cout << output_signal.data << std::endl;
    }

    return 0;
}


