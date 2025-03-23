`timescale 1ns / 1ps

module BinaryTo7Segment (
    input  wire       clk,
    input  wire [7:0] binary_in,
    output wire [6:0] seg,
    output reg  [3:0] an  // full 4-bit output, but only AN0 and AN1 are active
);
    wire [3:0] hex_high = binary_in[7:4];
    wire [3:0] hex_low  = binary_in[3:0];
    reg  [3:0] digit;
    reg        select = 0;
    reg [19:0] refresh_counter = 0;

    always @(posedge clk) begin
        refresh_counter <= refresh_counter + 1;
        if (refresh_counter == 20'd99999) begin
            refresh_counter <= 0;
            select <= ~select;
        end
    end

    always @(*) begin
        if (select) begin
            digit = hex_high;
            an    = 4'b1101;  // AN1 active (active low)
        end else begin
            digit = hex_low;
            an    = 4'b1110;  // AN0 active
        end
    end

    HexTo7Segment hex_to_7seg (
        .hex(digit),
        .seg(seg)
    );
endmodule
