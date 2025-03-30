`timescale 1ns / 1ps

module uart_rx_top(
    input  wire clk,
    input  wire rst,
    input  wire rx,
    output wire [6:0] seg,
    output wire [3:0] an,
    output wire [7:0] led
);

    wire tick;
    wire rx_done;
    wire [7:0] data_out;
    reg  [7:0] display_data = 8'd0;

    // Baud rate generator (115200 bps with oversampling)
    baud_rate_generator baud_gen (
        .clk(clk),
        .rst(rst),
        .tick_out(tick)
    );

    // UART Receiver
    uart_receiver receiver (
        .clk(clk),
        .rst(rst),
        .tick_in(tick),
        .rx_data(rx),
        .data_out(data_out),
        .rx_done(rx_done)
    );

    // Update display data only when a byte is received
    always @(posedge clk) begin
        if (rst)
            display_data <= 8'd0;
        else if (rx_done)
            display_data <= data_out;
    end

    // Display the byte as 2 HEX digits on AN0 and AN1
    BinaryTo7Segment display (
        .clk(clk),
        .binary_in(display_data),
        .seg(seg),
        .an(an)
    );

    assign led = display_data;

endmodule
