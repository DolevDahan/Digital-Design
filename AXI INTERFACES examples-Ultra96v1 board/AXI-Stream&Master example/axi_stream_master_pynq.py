from pynq import Overlay
from pynq import allocate
import numpy as np

# Load the FPGA bitstream
overlay = Overlay("/home/xilinx/jupyter_notebooks/Bitstream_Files/axi_stream_master.bit")
help(overlay)

my_ip = overlay.axi_stream_master_1

print(my_ip.register_map)

N=5
py_buffer = allocate(shape=(N,), dtype=np.float32)
py_buffer[:] = [183.40, 2.0, 3.0, 4.0, 5.0]
print('array:',py_buffer, ',addr:',hex(py_buffer.physical_address))

input_buff = allocate(shape=(N,),dtype=np.float32)
input_buff[:] = [1.0, 2.0, 3.0, 4.0, 5.0]
print('array:',input_buff, ',addr:',hex(input_buff.physical_address))

output_buff = allocate(shape=(N,),dtype=np.float32)
print('array:',output_buff, ',addr:',hex(output_buff.physical_address))

my_ip.register_map.a_1 = py_buffer.physical_address
dma = overlay.axi_dma_0
dma.sendchannel.transfer(input_buff)
dma.recvchannel.transfer(output_buff)

my_ip.register_map.CTRL.AP_START=1

print('array:',input_buff, ',addr:',hex(input_buff.physical_address))
print('array:',output_buff, ',addr:',hex(output_buff.physical_address))