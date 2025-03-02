// #include <hls_stream.h>
// #include <ap_axi_sdata.h>
// #include <iostream>
// #include <fstream>

// #define N 5  // Number of neurons

// // Function prototype from your HLS code
// void ensemble(
//     volatile float *max_rates_addr,
//     volatile float *intercepts_addr,
//     volatile float *encoders_addr,
//     hls::stream<hls::axis<float, 0, 0, 0>> &input_stream,
//     hls::stream<hls::axis<float, 0, 0, 0>> &output_stream,
//     float dt,
//     float tau_rc,
//     float tau_ref,
//     float min_voltage
// );

// int main() {
//     // Define neuron parameters
//     float max_rates[N] = {100.0f, 120.0f, 110.0f, 130.0f, 125.0f};
//     float intercepts[N] = {0.5f, 0.6f, 0.4f, 0.7f, 0.55f};
//     float encoders[N] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

//     // AXI-streams for input and output
//     hls::stream<hls::axis<float, 0, 0, 0>> input_stream;
//     hls::stream<hls::axis<float, 0, 0, 0>> output_stream;

//     // Simulation parameters
//     float dt = 0.001f;
//     float tau_rc = 0.02f;
//     float tau_ref = 0.002f;
//     float min_voltage = 0.0f;

//     // Inject test values from 1 to 10
//     std::ofstream result_file("output_results.txt");
//     for (float input_value = 1.0f; input_value <= 10.0f; input_value += 1.0f) {
//         hls::axis<float, 0, 0, 0> in_data;
//         in_data.data = input_value;
//         input_stream.write(in_data);
//     }

//     // Call the HLS function
//     ensemble(max_rates, intercepts, encoders, input_stream, output_stream, dt, tau_rc, tau_ref, min_voltage);

//     // Retrieve output values and store in file
//     while (!output_stream.empty()) {
//         hls::axis<float, 0, 0, 0> out_data = output_stream.read();
//         std::cout << "Output: " << out_data.data << std::endl;
//         result_file << out_data.data << "\n";
//     }

//     result_file.close();
//     return 0;
// }


#include "ensemble.h"
#include <hls_stream.h>
#include <cmath>
#include <ap_axi_sdata.h>
#include <cstdio>

#define NUM_NEURONS 2

int main() {
    const float dt = 0.001f;
    const float duration = 1.0f;
    const int steps = static_cast<int>(duration / dt) + 1;

    const float tau_rc = 0.02f;
    const float tau_ref = 0.002f;
    const float min_voltage = 0.0f;
    
    float max_rates[NUM_NEURONS] = {100.0f, 100.0f};
    float intercepts[NUM_NEURONS] = {-0.5f, -0.5f};
    float encoders[NUM_NEURONS] = {1.0f, -1.0f};

    hls::stream<hls::axis<float, 0, 0, 0>> input_stream;
    hls::stream<hls::axis<float, 0, 0, 0>> voltage_out_stream;
    hls::stream<hls::axis<float, 0, 0, 0>> spikes_out_stream;

    // Feed input data
    for (int i = 0; i < steps; i++) {
        float time = (i + 1) * dt;
        float input_value = std::sin(8 * time);

        hls::axis<float, 0, 0, 0> input_data;
        input_data.data = input_value;
        input_data.last = (i == steps - 1);
        input_stream.write(input_data);
    }

    // Call ensemble function
    ensemble(input_stream, voltage_out_stream, spikes_out_stream, 
             max_rates, intercepts, encoders, dt, tau_rc, tau_ref, min_voltage);

    // Open results file
    FILE *results_file = fopen("Ensemble_generic_results.csv", "w");
    if (!results_file) {
        printf("Error: Could not open results file for writing\n");
        return -1;
    }

    fprintf(results_file, "time,neuron_id,voltage,spike\n");

    // Read outputs from voltage and spike streams
    for (int i = 0; i < steps; i++) {
        float time = (i+1) * dt;
        for (int neuron_id = 0; neuron_id < NUM_NEURONS; neuron_id++) {
            hls::axis<float, 0, 0, 0> voltage_data = voltage_out_stream.read();
            hls::axis<float, 0, 0, 0> spike_data = spikes_out_stream.read();

            printf("time: %f, neuron: %d, voltage: %f, spike: %f\n",
                   time, neuron_id, voltage_data.data, spike_data.data);
            fprintf(results_file, "%f,%d,%f,%f\n",
                    time, neuron_id, voltage_data.data, spike_data.data);
        }
    }

    fclose(results_file);

    printf("Address of max_rates: %p\n", static_cast<void*>(max_rates));
    printf("Address of intercepts: %p\n", static_cast<void*>(intercepts));
    printf("Address of encoders: %p\n", static_cast<void*>(encoders));

    return 0;
}
