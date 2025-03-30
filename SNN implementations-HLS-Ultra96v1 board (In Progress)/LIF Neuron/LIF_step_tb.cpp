#include <hls_stream.h>
#include <iostream>
#include <cmath> // For sin()
#include <ap_axi_sdata.h>

// Include the header file containing the LIF_step function
void LIF_step(
    hls::stream<hls::axis<float, 0, 0, 0>> &input_stream,
    hls::stream<hls::axis<float, 0, 0, 0>> &output_stream,
    hls::stream<hls::axis<float, 0, 0, 0>> &continuous_voltage,
    hls::stream<hls::axis<float, 0, 0, 0>> &spikes_out,
    float dt_in,
    float tau_rc_in,
    float tau_ref_in,
    float min_voltage_in
);

int main() {
    // Define input and output streams
    hls::stream<hls::axis<float, 0, 0, 0>> input_stream;
    hls::stream<hls::axis<float, 0, 0, 0>> output_stream;
    hls::stream<hls::axis<float, 0, 0, 0>> continuous_voltage;
    hls::stream<hls::axis<float, 0, 0, 0>> spikes_out;

    // Simulation parameters
    float dt = 0.01f;          // Time step in seconds
    int time_steps = 40;       // Number of simulation steps
    float tau_rc = 0.02f;      // Membrane time constant
    float tau_ref = 0.003f;    // Refractory period
    float min_voltage = 0.0f; // Minimum voltage

    // Generate time array and sinusoidal input current
    float input_current[time_steps];
    for (int i = 0; i < time_steps; i++) {
        float time = i * dt; // Compute time for this step
        input_current[i] = 5.0f * std::sin(5.0f * 2.0f * M_PI * time); // Sinusoidal input current
    }

    // Feed the input samples into the input stream
    std::cout << "Input Current:" << std::endl;
    for (int i = 0; i < time_steps; i++) {
        hls::axis<float, 0, 0, 0> input_signal;
        input_signal.data = input_current[i];
        input_signal.keep = -1;   // Keep all bytes (default value)
        input_signal.strb = -1;   // Strobe all bytes (default value)
        input_signal.user = {};   // Initialize `user` to default
        input_signal.last = (i == time_steps - 1) ? 1 : 0; // Mark the last signal
        input_signal.id = {};     // Initialize `id` to default
        input_signal.dest = {};   // Initialize `dest` to default

        input_stream.write(input_signal);
        std::cout << "Time " << i * dt << "s: " << input_current[i] << std::endl;
    }

    // Call the DUT (Device Under Test)
    LIF_step(input_stream, output_stream, continuous_voltage, spikes_out, dt, tau_rc, tau_ref, min_voltage);

    // Read and print the output voltage
    std::cout << "\nOutput Voltage:" << std::endl;
    while (!output_stream.empty()) {
        hls::axis<float, 0, 0, 0> output_signal = output_stream.read();
        std::cout << output_signal.data << std::endl;
    }

    // Read and print the continuous voltage
    std::cout << "\nContinuous Voltage:" << std::endl;
    while (!continuous_voltage.empty()) {
        hls::axis<float, 0, 0, 0> continuous_signal = continuous_voltage.read();
        std::cout << continuous_signal.data << std::endl;
    }

    // Read and print the spike outputs
    std::cout << "\nSpike Outputs:" << std::endl;
    while (!spikes_out.empty()) {
        hls::axis<float, 0, 0, 0> spike_signal = spikes_out.read();
        std::cout << spike_signal.data << std::endl;
    }

    return 0;
}
