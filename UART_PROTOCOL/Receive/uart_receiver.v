`timescale 1ns / 1ps

module uart_receiver(
    input wire system_clk,
    input wire rst,
    input wire uart_tick,
    input wire uart_half_tick,
    input wire rx_data,
    output reg [7:0] data_out,
    output reg rx_done,
    output reg [1:0] flag_state
);
    
    reg [1:0] state;
    parameter IDLE  = 2'b00;
    parameter START = 2'b01;
    parameter READ  = 2'b10;
    parameter STOP  = 2'b11;
    
    reg [2:0] data_counter;
    reg [7:0] shift_register;
    
always @(posedge system_clk or posedge rst) begin
    if (rst) begin
        state <= IDLE;
        data_out <= 0;
        rx_done <= 0;
        data_counter <= 0;
        shift_register <= 0;
        flag_state <= 2'b00;
    end
    else begin
        case (state)
            IDLE: begin
                flag_state <= 2'b00;
                rx_done <= 0;
                shift_register <= 0;
                data_counter <= 0;
                if (rx_data == 0)
                    state <= START;
            end
            
            START: begin
                flag_state <= 2'b01;
                if (uart_half_tick) begin
                    if (rx_data == 0) begin
                        state <= READ;
                    end else begin
                        state <= IDLE;
                    end
                end       
            end
            
            READ: begin
                flag_state <= 2'b10;
                if (uart_tick) begin
                    shift_register <= {rx_data, shift_register[7:1]}; // push from left (LSB received first)
                    if (data_counter == 7)
                        state <= STOP;
                    else
                        data_counter <= data_counter + 1'b1; 
                end 
            end
            
            STOP: begin
                flag_state <= 2'b11;
                if (uart_tick) begin
                    rx_done <= 1;
                    data_out <= shift_register;
                    state <= IDLE;
                end
            end
            
            default: begin
                state <= IDLE;
            end
        endcase 
    end
end

endmodule
