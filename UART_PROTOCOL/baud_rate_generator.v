`timescale 1ns / 1ps

module baud_rate_generator(
    input wire system_clk,
    input wire rst,
    output reg uart_tick,
    output reg uart_half_tick
    );

    parameter system_clk_freq = 100_000_000; // 100MHz
    parameter baud_rate = 115200; // 115200 bps
    parameter counter_cycles = system_clk_freq / baud_rate; // 868 cycles
    parameter half_counter_cycles = counter_cycles / 2; // חצי זמן ביט
    
    reg [9:0] counter = 0; // גודל המונה מתאים ל- log2(868)

    always @(posedge system_clk or posedge rst) begin
        if (rst) begin
            counter <= 0;
            uart_tick <= 0;
            uart_half_tick <= 0;
        end
        else begin
            if (counter == (counter_cycles / 2) - 1) begin 
                uart_half_tick <= 1;
                counter <= counter + 1'b1;
            end
            else if (counter == counter_cycles - 1) begin
                uart_tick <= 1;
                counter <= 0;
            end
            else begin
                counter <= counter + 1'b1;
                uart_tick <= 0; // מאפס את uart_tick בכל מחזור רגיל
                uart_half_tick <= 0;
            end
        end
    end
endmodule
