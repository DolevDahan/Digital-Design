#include <hls_math.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <algorithm>
// Exponential decay function
float exp_decay(float x) {
    #pragma HLS INLINE
    return hls::expf(x) - 1.0f;
}

// Leaky Integrate-and-Fire (LIF) model step function
void LIF_step(
    hls::stream<hls::axis<float, 0, 0, 0>> &input_stream, // Input stream
    hls::stream<hls::axis<float, 0, 0, 0>> &output_stream, // Output stream
    hls::stream<hls::axis<float, 0, 0, 0>> &continuous_voltage, // Continuous Output stream
    hls::stream<hls::axis<float, 0, 0, 0>> &spikes_out, // Spikes output stream

    float dt_in,                  // Time step for the simulation
    float tau_rc_in,              // Membrane time constant
    float tau_ref_in,             // Refractory period
    float min_voltage_in          // Minimum allowed voltage
) {
    #pragma HLS INTERFACE axis port=input_stream register_mode=off
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE axis port=continuous_voltage
    #pragma HLS INTERFACE axis port=spikes_out
    #pragma HLS INTERFACE s_axilite port=dt_in bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=tau_rc_in bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=tau_ref_in bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=min_voltage_in bundle=CTRL_BUS
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL_BUS

    hls::axis<float, 0, 0, 0> J, voltage, spikes, con_voltage;

    // Initialize variables
    float refractory_time = 0.0f;
    float previous_voltage = 0.0f;
    float dt = dt_in;
    float tau_rc = tau_rc_in;
    float min_voltage = min_voltage_in;
    float tau_ref = tau_ref_in;
    

    // Pipelining the loop
    #pragma HLS PIPELINE II=1

    while (true) {
        if (!input_stream.empty()) {

            J = input_stream.read();
            
            refractory_time = refractory_time - dt;
            
            float delta_tt;
            // Precompute time step for refractory condition
            if ((dt - refractory_time) < 0) {
                delta_tt = 0;
            }
            else if (((dt - refractory_time) > dt)){
                delta_tt = dt;             
            }            
            else {
                delta_tt=(dt - refractory_time);
            }


            // Compute voltage update
            voltage.data = previous_voltage - (J.data - previous_voltage) * exp_decay(-delta_tt / tau_rc);
            con_voltage.data = voltage.data;
            // Spike handling
            if (voltage.data > 1.0f) {                                   
                spikes.data = 1.0f;
                voltage.data = 0.0f;
                refractory_time = tau_ref; // Reset refractory period
            } 
            else {
                spikes.data = 0.0f;
                voltage.data = voltage.data;
            }
            voltage.data = std::max(voltage.data,min_voltage);
            // Update previous voltage
            previous_voltage = voltage.data;

            // Copy metadata to output
            voltage.keep = J.keep;
            voltage.strb = J.strb;
            voltage.user = J.user;
            voltage.last = J.last;
            voltage.id   = J.id;
            voltage.dest = J.dest;

            con_voltage.keep = J.keep;
            con_voltage.strb = J.strb;
            con_voltage.user = J.user;
            con_voltage.last = J.last;
            con_voltage.id   = J.id;
            con_voltage.dest = J.dest;

            spikes.keep = J.keep;
            spikes.strb = J.strb;
            spikes.user = J.user;
            spikes.last = J.last;
            spikes.id   = J.id;
            spikes.dest = J.dest;

            // Write to output stream
            output_stream.write(voltage);
            continuous_voltage.write(con_voltage);
            spikes_out.write(spikes);
            // Exit on the last signal
            if (J.last) {
                break;
            }
        }
    }
}
