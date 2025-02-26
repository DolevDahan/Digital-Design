#ifndef COMPUTE_STREAM_H
#define COMPUTE_STREAM_H

#include <hls_stream.h>
#include <ap_axi_sdata.h>

// Define AXI Stream data type (32-bit float with metadata)
typedef hls::axis<float, 0, 0, 0> AXI_STREAM_DATA;

// Function prototype for the compute_stream function
void vector_mult(hls::stream<AXI_STREAM_DATA> &input_stream,
                    hls::stream<AXI_STREAM_DATA> &output_stream,
                    const float multiplier);

#endif // COMPUTE_STREAM_H
