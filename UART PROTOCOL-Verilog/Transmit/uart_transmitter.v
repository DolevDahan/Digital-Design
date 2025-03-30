`timescale 1ns / 1ps

module uart_transmitter(
    input wire system_clk,
    input wire rst,
    input wire tick_in, // oversampled (factor 16) baud rate
    input wire tx_start,
    input wire [7:0] data_in,
    output reg tx_data
);

reg [1:0] state;
parameter IDLE  = 2'b00;
parameter START = 2'b01;
parameter WRITE = 2'b10;
parameter STOP  = 2'b11;

reg [2:0] data_counter;
reg [3:0] sample_counter;
reg [7:0] shift_register;


//always @(posedge system_clk or posedge rst)
always @(posedge system_clk or posedge rst)
    begin
    if (rst)
        begin
        tx_data <= 1;
        data_counter <= 0;
        sample_counter <= 0;
        shift_register <= 0;
        state <= IDLE;
        end
    else
        begin
            case(state)
                IDLE:
                    begin
                    if (tx_start)
                        begin
                        tx_data <= 0;
                        state <= START;
                        shift_register <= data_in;
                        end
                    end
                START:
                    begin
                        if (tick_in)
                            begin
                               if (sample_counter==15)
                                begin
                                data_counter <= 0;
//                                tx_data <= shift_register[0];
//                                shift_register <= shift_register >> 1;
                                sample_counter <= 0;
                                state <= WRITE;
                                end
                               else
                                sample_counter <= sample_counter + 1'b1;
                            end
                    end
                WRITE:
                    begin
                        if (tick_in)
                            begin
                            tx_data <= shift_register[0];
                               if (sample_counter==15)
                                    begin
                                    if (data_counter==7)
                                        begin
                                        tx_data <= 1;
                                        state <= STOP;
                                        end
                                    shift_register <= shift_register >> 1;
                                    data_counter <= data_counter + 1'b1;
                                    sample_counter <= 0;
                                    end
                               else
                                sample_counter <= sample_counter + 1'b1;
                            end
                    end
                STOP:
                    begin
                        if (tick_in)
                            begin
                               if (sample_counter==15)
                                    begin                                    sample_counter <= 0;
                                    state <= IDLE;
                                    end
                               else
                                sample_counter <= sample_counter + 1'b1;       
                            end
                    end
            endcase
        end
    end
endmodule
