`timescale 1ns / 1ps

module baud_rate_generator(
    input wire system_clk,
    input wire rst,
    output wire tick_out  // הפכנו ל-wire כי הוא יוגדר עם assign
    );

    parameter system_clk_freq = 100_000_000; // 100MHz
    parameter baud_rate = 115200; // 115200 bps
    parameter oversampaling_factor = 16;
    parameter counter_cycles = (system_clk_freq / baud_rate) / oversampaling_factor; 
    reg [9:0] counter = 0;
    
    always @(posedge system_clk or posedge rst) begin
        if (rst) begin
            counter <= 0;
        end
        else begin
            if (counter == counter_cycles - 1) begin
                counter <= 0;
            end
            else begin
                counter <= counter + 1'b1;
            end
        end
    end

    assign tick_out = (counter == counter_cycles - 1) ? 1'b1 : 1'b0;

endmodule
