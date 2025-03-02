from pynq import Overlay
from pynq import allocate
import numpy as np

# Load the FPGA bitstream
overlay = Overlay("/home/xilinx/jupyter_notebooks/Bitstream_Files/elaborate_e24.bit")
help(overlay)

# print('done')

my_ip = overlay.ensemble_0
my_ip.register_map

dt = float(0.001)  # Time step (1 ms)
duration = 1.0  # Total simulation time (1 second)
time = np.arange(dt, duration+dt, dt)  # Time array


tau_rc = 0.02
tau_ref = 0.002
min_voltage = 0.0

my_ip.write(0x34, int(np.float32(dt).view(np.uint32)))
my_ip.write(0x3c, int(np.float32(tau_rc).view(np.uint32)))
my_ip.write(0x44, int(np.float32(tau_ref).view(np.uint32)))
my_ip.write(0x4c, int(np.float32(min_voltage).view(np.uint32)))


NUM_NEURONS = 2
max_rates_buff = allocate(shape=(NUM_NEURONS,), dtype=np.float32)
# max_rates_buff[:] = [int(np.float32(100.0).view(np.uint32)) , int(np.float32(100.0).view(np.uint32))]
max_rates_buff[:] = [100.0 , 100.0]
# max_rates_buff[:] = [np.float32(100.0), np.float32(100.0)]
print('max_rates_buff array:',max_rates_buff, ',addr:',hex(max_rates_buff.physical_address))

intercepts_buff = allocate(shape=(NUM_NEURONS,), dtype=np.float32)
# intercepts_buff[:] = [int(np.float32(-0.5).view(np.uint32)),int(np.float32(-0.5).view(np.uint32))]
intercepts_buff[:] = [-0.5,-0.5]
# intercepts_buff[:] = [np.float32(-0.5), np.float32(-0.5)]
print('intercepts_buff array:',intercepts_buff, ',addr:',hex(intercepts_buff.physical_address))

encoders_buff = allocate(shape=(NUM_NEURONS,), dtype=np.float32)
encoders_buff[:] = [1.0,-1.0]
# encoders_buff[:] = [np.float32(1.0), np.float32(-1.0)]
# encoders_buff[:] = [int(np.float32(1.0).view(np.uint32)),int(np.float32(-1.0).view(np.uint32))]
print('encoders_buff array:',encoders_buff, ',addr:',hex(encoders_buff.physical_address))


import matplotlib.pyplot as plt
input_signal = np.array(np.sin(8 * time), dtype=np.float32)  # Sinusoidal input current
plt.title('Input Signal')
plt.plot(time, input_signal)
plt.grid()


input_buffer = allocate(shape=(len(input_signal),), dtype=np.float32)
np.copyto(input_buffer, input_signal)
print('input_buffer array:',input_buffer[:10], ',addr:',hex(input_buffer.physical_address))
print(len(input_buffer))
output_buffer = allocate(shape=(NUM_NEURONS*len(input_signal),),dtype=np.float32)
print('*before* output_buffer array :',output_buffer[:10], ',addr:',hex(output_buffer.physical_address))

output_buffer2 = allocate(shape=(NUM_NEURONS*len(input_signal),),dtype=np.float32)
print('*before* output_buffer2 array :',output_buffer2[:10], ',addr:',hex(output_buffer2.physical_address))


my_ip.register_map.max_rates_addr_1 = max_rates_buff.physical_address
my_ip.register_map.intercepts_addr_1 = intercepts_buff.physical_address
my_ip.register_map.encoders_addr_1 = encoders_buff.physical_address


dma = overlay.axi_dma_0
dma1 = overlay.axi_dma_1
dma.sendchannel.transfer(input_buffer)
dma.recvchannel.transfer(output_buffer)
dma1.recvchannel.transfer(output_buffer2)


my_ip.register_map.CTRL.AP_START=1

print('input_buffer array:',input_buffer[:15], ',addr:',hex(input_buffer.physical_address))
print('*after* output_buffer array:',output_buffer[:50], ',addr:',hex(output_buffer.physical_address))
print('*after* output_buffer array:',output_buffer2[:50], ',addr:',hex(output_buffer2.physical_address))


vol_1 = output_buffer[1::2]
vol_2 = output_buffer[::2]

plt.figure(figsize=(10, 5))
plt.title('vol_1')
plt.plot(time, vol_1)
plt.grid()

plt.figure(figsize=(10, 5))
plt.title('vol_2')
plt.plot(time, vol_2)
plt.grid()


spike_1 = output_buffer2[1::2]
spike_2 = output_buffer2[::2]
plt.figure(figsize=(10, 5))
plt.title('spike_1')
plt.plot(time, spike_1)
plt.grid()

plt.figure(figsize=(10, 5))
plt.title('spike_2')
plt.plot(time, spike_2)
plt.grid()