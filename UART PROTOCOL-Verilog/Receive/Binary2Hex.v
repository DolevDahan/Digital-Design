`timescale 1ns / 1ps
module BinaryTo7Segment (
    input  wire        clk,
    input  wire [7:0]  binary_in,
    output wire [6:0]  seg,
    output wire [3:0]  an
);
    wire [3:0] hex_high = binary_in[7:4];
    wire [3:0] hex_low  = binary_in[3:0];
    wire [3:0] digit;

    reg select;  // Toggle between lower and higher digit
    reg [19:0] counter;  // Slow refresh counter

    // Refresh logic (~1kHz)
    always @(posedge clk) begin
        counter <= counter + 1;
        if (counter == 20'd99999) begin
            counter <= 0;
            select <= ~select;
        end
    end

    assign digit = (select) ? hex_high : hex_low;
    assign an    = (select) ? 4'b1101 : 4'b1110;  // AN1 and AN0 active low

    HexTo7Segment hex_to_7seg (
        .hex(digit),
        .seg(seg)
    );
endmodule
