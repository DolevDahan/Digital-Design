import matplotlib.pyplot as plt
from pynq import Overlay
from pynq import allocate
import numpy as np
import time
# Load the overlay
overlay = Overlay("/home/xilinx/jupyter_notebooks/Bitstream_Files/LIF_step_design.bit")

dma1=overlay.axi_dma_1 # input, voltage output
dma2=overlay.axi_dma_0 # spikes output
dma3=overlay.axi_dma_2 # continues voltage output
lif_step=overlay.LIF_step_0

dt = 0.01
tau_rc = 0.01
tau_ref = 0.003
min_voltage = 0

lif_step.write(0x10, int(np.float32(dt).view(np.uint32)))
lif_step.write(0x18, int(np.float32(tau_rc).view(np.uint32)))
lif_step.write(0x20, int(np.float32(tau_ref).view(np.uint32)))
lif_step.write(0x28, int(np.float32(min_voltage).view(np.uint32)))

time_steps = 2500  # Number of simulation steps
t = np.arange(0, time_steps * dt, dt)  # Time array

# Input current parameters
input_data = np.array(5 * np.sin(1 * 2 * np.pi * t), dtype=np.float32)  # Sinusoidal input current

input_buffer = allocate(shape=(len(input_data),), dtype=np.float32)
np.copyto(input_buffer, input_data)
dma1.sendchannel.transfer(input_buffer)

voltage1 = allocate(shape=(len(input_data),), dtype=np.float32)
spikes1 = allocate(shape=(len(input_data),), dtype=np.float32)
continues_voltage1 = allocate(shape=(len(input_data),), dtype=np.float32)

dma1.recvchannel.transfer(voltage1)
dma2.recvchannel.transfer(spikes1)
dma3.recvchannel.transfer(continues_voltage1)

start_time = time.time()
lif_step.write(0x00, 1)  # Start the IP
end_time = time.time()
fpga_runtime = end_time - start_time
print("Execution time: ", fpga_runtime, " seconds")

# Plot input current over time
plt.figure(figsize=(12, 6))
plt.plot(t, input_data, label="Input Current")
plt.xlabel("Time (s)")
plt.ylabel("Input Current")
plt.title("Input Current Over Time (Sinusoidal Input)")
plt.legend()
plt.grid()
plt.show()

# Plot membrane voltage
plt.figure(figsize=(12, 6))
plt.plot(t, voltage1, label="Input Current")
plt.axhline(y=1, color='r', linestyle='--', label="Threshold")
plt.xlabel("Time (s)")
plt.ylabel("Membrane Voltage")
plt.title("Membrane Voltage Over Time")
plt.legend()
plt.grid()
plt.show()

# Plot membrane voltage
plt.figure(figsize=(12, 6))
plt.plot(t, spikes1, label="Input Current")
plt.axhline(y=1, color='r', linestyle='--', label="Threshold")
plt.xlabel("Time (s)")
plt.ylabel("Membrane Voltage")
plt.title("Membrane Voltage Over Time")
plt.legend()
plt.grid()
plt.show()

# Plot spike output
plt.figure(figsize=(12, 6))
plt.plot(t, continues_voltage1, label="Input Current")
plt.xlabel("Time (s)")
plt.ylabel("Spike Output")
plt.title("Neuron Spike Output Over Time")
plt.legend()
plt.grid()
plt.show()