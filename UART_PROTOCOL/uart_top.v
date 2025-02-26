`timescale 1ns / 1ps

module uart_top(
    input wire system_clk,
    input wire rst,
    input wire rx_data,
    output wire [7:0] data_out,
    output wire rx_done,
    output wire [1:0] flag_state  // נוסיף חיבור ל-flag
);

    wire uart_tick;
    wire uart_half_tick;

    // מחולל קצב השידור (Baud Rate Generator)
    baud_rate_generator baud_gen (
        .system_clk(system_clk),
        .rst(rst),
        .uart_tick(uart_tick),
        .uart_half_tick(uart_half_tick)
    );

    // מקלט UART
    uart_receiver receiver (
        .system_clk(system_clk),
        .rst(rst),
        .uart_tick(uart_tick),
        .uart_half_tick(uart_half_tick),
        .rx_data(rx_data),
        .data_out(data_out),
        .rx_done(rx_done),
        .flag_state(flag_state)  // מחבר את ה-flag החוצה
    );

endmodule
