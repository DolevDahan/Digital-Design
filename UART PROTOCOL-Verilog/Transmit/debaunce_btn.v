`timescale 1ns / 1ps

module debaunce_btn(
    input clk,                // 100MHz clock (Basys3)
    input rst,                // Reset
    input raw_button,         // Noisy button input
    output reg tx_start       // 1-cycle pulse on rising edge of clean button
);

    reg [19:0] count;
    reg ff_0, ff_1;
    reg stable;
    reg prev_stable;

    always @(posedge clk) begin
        if (rst) begin
            ff_0 <= 0;
            ff_1 <= 0;
            stable <= 0;
            prev_stable <= 0;
            count <= 0;
            tx_start <= 0;
        end else begin
            // Synchronize raw input to system clock
            ff_0 <= raw_button;
            ff_1 <= ff_0;

            // Debounce logic
            if (ff_1 == stable) begin
                count <= 0; // Input is stable, reset counter
            end else begin
                count <= count + 1;
                if (count == 1_000_000) begin // ≈10ms @ 100MHz
                    stable <= ff_1; // Accept new stable value
                    count <= 0;
                end
            end

            // Rising edge detection on debounced signal
            tx_start <= (stable && !prev_stable);
            prev_stable <= stable;
        end
    end
endmodule
