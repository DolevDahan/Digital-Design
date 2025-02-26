#ifndef LIF_STEP_H
#define LIF_STEP_H

#include <hls_stream.h>
#include <ap_axi_sdata.h>

// Exponential decay function
float exp_decay(float x);

// Leaky Integrate-and-Fire (LIF) model step function
void LIF_step(
    hls::stream<hls::axis<float, 0, 0, 0>> &input_stream, // Input stream
    hls::stream<hls::axis<float, 0, 0, 0>> &output_stream, // Output stream
    hls::stream<hls::axis<float, 0, 0, 0>> &continuous_voltage, // Continuous voltage stream
    hls::stream<hls::axis<float, 0, 0, 0>> &spikes_out, // Spikes output stream
    float dt_in,                  // Time step for the simulation
    float tau_rc_in,              // Membrane time constant
    float tau_ref_in,             // Refractory period
    float min_voltage_in          // Minimum allowed voltage
);

#endif // LIF_STEP_H
