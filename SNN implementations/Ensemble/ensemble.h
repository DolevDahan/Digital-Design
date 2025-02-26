#ifndef ENSEMBLE_H
#define ENSEMBLE_H

#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <hls_math.h>

#define NUM_NEURONS 2

// Struct to store LIF results
struct LIFResult {
    float voltage_out;
    float spike;
};

// Function prototypes
LIFResult LIF_step(
    float J_input,
    float dt,
    float tau_rc,
    float tau_ref,
    float min_voltage,
    float& previous_voltage_temp,
    float& refractory_time_temp
);

float compute_gain(float max_rate, float intercept, float inv_tau_rc, float tau_ref);
float compute_bias(float intercept, float gain);

void ensemble(
    hls::stream<hls::axis<float, 0, 0, 0>>& input_stream,
    hls::stream<hls::axis<float, 0, 0, 0>>& voltage_out_stream,
    hls::stream<hls::axis<float, 0, 0, 0>>& spikes_out_stream, // Restored this
    volatile float *max_rates_addr,
    volatile float *intercepts_addr,
    volatile float *encoders_addr,
    float dt,
    float tau_rc,
    float tau_ref,
    float min_voltage
);

#endif // ENS_GENERIC_H
