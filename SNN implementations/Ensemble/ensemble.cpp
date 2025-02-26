

#include <hls_stream.h>
#include <cmath>
#include <ap_axi_sdata.h>
#include <hls_math.h>
#include <stdio.h>
#include <string.h>
#include <tuple>

#define NUM_NEURONS 2 // Set the number of neurons as a compile-time constant

float compute_gain(float max_rate, float intercept, float inv_tau_rc, float tau_ref) {
    float x1 = 1.0f / max_rate;
    float x2 = tau_ref - x1;
    float x4 = x2 * inv_tau_rc;
    float exp_result = hls::exp(x4);
    float x = 1.0f / (1.0f - exp_result);
    float gain = (1.0f - x) / (intercept - 1.0f);
    return gain;
}

float compute_bias(float intercept, float gain) {
    float bias = 1.0f - gain * intercept;
    return bias;
}

struct LIFResult {
    float voltage_out;
    float spike;
};


LIFResult LIF_step(
    float J_input,
    float dt,
    float tau_rc,
    float tau_ref,
    float min_voltage,
    float& previous_voltage_temp,
    float& refractory_time_temp
) {
    #pragma HLS inline
    
    refractory_time_temp -= dt;

    float delta_tt;
	if ((dt - refractory_time_temp) < 0) {
		delta_tt = 0;
	} else if ((dt - refractory_time_temp) > dt) {
		delta_tt = dt;
	} else {
		delta_tt = dt - refractory_time_temp;
	};

    float lif1 = -delta_tt / tau_rc;
    float lif2=hls::exp(lif1);
    float lif3 = J_input - previous_voltage_temp;
    float lif4 = lif2 - 1.0f;
    float lif5 = lif3 * lif4;
    float voltage_out = previous_voltage_temp - lif5;
    float spike;
    
    if (voltage_out > 1.0f) {
        spike = 1000.0f;
        float temp1 = voltage_out - 1.0f;
        float temp2 = J_input - 1.0f;
        float temp3 = -(temp1 / temp2);
        float temp4 = hls::log(1.0f + temp3);
        float t_spike = dt + tau_rc * temp4;
        // float t_spike = dt + tau_rc * hls::log(1.0f + (-(voltage_out - 1.0f) / (J_input - 1.0f)));
        voltage_out = 0.0f;
        refractory_time_temp = tau_ref + t_spike;
    } else {
        spike = 0.0f;
    }
    voltage_out = hls::fmax(voltage_out, min_voltage);
    previous_voltage_temp = voltage_out;
	return {voltage_out, spike};
}

void ensemble(
    hls::stream<hls::axis<float, 0, 0, 0>>& input_stream,
    hls::stream<hls::axis<float, 0, 0, 0>>& voltage_out_stream,
    hls::stream<hls::axis<float, 0, 0, 0>>& spikes_out_stream,
	volatile float *max_rates_addr,
	volatile float *intercepts_addr,
	volatile float *encoders_addr,
    float dt,
    float tau_rc,
    float tau_ref,
    float min_voltage
) {
	#pragma HLS INTERFACE m_axi port=max_rates_addr depth=NUM_NEURONS offset=slave bundle=gmem
	#pragma HLS INTERFACE m_axi port=intercepts_addr depth=NUM_NEURONS offset=slave bundle=gmem
	#pragma HLS INTERFACE m_axi port=encoders_addr depth=NUM_NEURONS offset=slave bundle=gmem
	#pragma HLS INTERFACE s_axilite port=return
	#pragma HLS INTERFACE axis port=input_stream register_mode=off
	#pragma HLS INTERFACE axis port=voltage_out_stream
	#pragma HLS INTERFACE axis port=spikes_out_stream
	#pragma HLS INTERFACE s_axilite port=dt
	#pragma HLS INTERFACE s_axilite port=tau_rc
	#pragma HLS INTERFACE s_axilite port=tau_ref
	#pragma HLS INTERFACE s_axilite port=min_voltage 
	#pragma HLS INTERFACE s_axilite port=return
    
	float max_rates[NUM_NEURONS];
	memcpy(max_rates, (const float*)max_rates_addr, NUM_NEURONS*sizeof(float));
        
	float intercepts[NUM_NEURONS];
	memcpy(intercepts, (const float*)intercepts_addr, NUM_NEURONS*sizeof(float));

	float encoders[NUM_NEURONS];
	memcpy(encoders, (const float*)encoders_addr, NUM_NEURONS*sizeof(float));
        
    float inv_tau_rc = 1.0f / tau_rc;
    float gain[NUM_NEURONS], bias[NUM_NEURONS];
    float previous_voltage[NUM_NEURONS] = {0.0f};
    float refractory_time[NUM_NEURONS] = {0.0f};
    float J_input[NUM_NEURONS];
    float voltage_out[NUM_NEURONS], spike[NUM_NEURONS];

    // #pragma HLS PIPELINE
    for (int neuron_id = 0; neuron_id < NUM_NEURONS; neuron_id++) {
        // #pragma HLS PIPELINE
        gain[neuron_id] = compute_gain(max_rates[neuron_id], intercepts[neuron_id], inv_tau_rc, tau_ref);
        bias[neuron_id] = compute_bias(intercepts[neuron_id], gain[neuron_id]);
    }

	hls::axis<float, 0, 0, 0> input_data,voltage_stream,spike_stream;
    while (!input_stream.empty()) {
        #pragma HLS PIPELINE II=89
        input_data = input_stream.read();
        
        for (int neuron_id = 0; neuron_id < NUM_NEURONS; neuron_id++) {            
            J_input[neuron_id] = gain[neuron_id] * input_data.data * encoders[neuron_id] + bias[neuron_id];

            voltage_out[neuron_id] = max_rates[0];
            spike[neuron_id] = intercepts[0];
            LIFResult result = LIF_step(J_input[neuron_id], dt, tau_rc, tau_ref, min_voltage, previous_voltage[neuron_id], refractory_time[neuron_id]);

			voltage_out[neuron_id] = result.voltage_out;
			spike[neuron_id] = result.spike;
        }
		    voltage_stream.keep = input_data.keep;
            voltage_stream.strb = input_data.strb;
            voltage_stream.user = input_data.user;
            voltage_stream.last = input_data.last;
            voltage_stream.id   = input_data.id;

            spike_stream.keep = input_data.keep;
            spike_stream.strb = input_data.strb;
            spike_stream.user = input_data.user;
            spike_stream.last = input_data.last;
            spike_stream.id   = input_data.id;
            
		for (int neuron_id = 0 ; neuron_id < NUM_NEURONS ; neuron_id++){
			voltage_stream.data = voltage_out[neuron_id];         
			voltage_out_stream.write(voltage_stream);
			spike_stream.data = spike[neuron_id];
            spikes_out_stream.write(spike_stream);
		}
    }
}