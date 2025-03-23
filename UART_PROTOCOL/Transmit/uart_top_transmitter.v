`timescale 1ns / 1ps


module uart_top_transmitter(
    input wire clk,             // 100MHz system clock from Basys3
    input wire rst,             // Reset (BTN0)
    input wire btnC,            // Center button to transmit
    input wire [7:0] sw,        // 8 switches to select the byte to send
    output wire tx             // UART TX line to PC
);

    wire tx_start_pulse;
    wire baud_tick;

    // === 1. Debounce button + generate rising edge pulse ===
    debaunce_btn debouncer (
        .clk(clk),
        .rst(rst),
        .raw_button(btnC),
        .tx_start(tx_start_pulse)  // 1-clock pulse when btnC is pressed
    );

    // === 2. Baud rate generator (oversampled tick, e.g., 16x) ===
    baud_rate_generator baud_gen (
        .system_clk(clk),
        .rst(rst),
        .tick_out(baud_tick)
    );

    // === 3. UART Transmitter ===
    uart_transmitter transmitter (
        .system_clk(clk),
        .rst(rst),
        .tick_in(baud_tick),
        .tx_start(tx_start_pulse),
        .data_in(sw),         // Data to send is set via switches
        .tx_data(tx)
    );

endmodule
