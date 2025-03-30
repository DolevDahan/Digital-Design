//`timescale 1ns / 1ps

//module uart_receiver(
//    input wire clk,
//    input wire rst,
//    input wire tick_in,
//    input wire rx_data,
//    output reg [7:0] data_out,
//    output reg rx_done
//    );

//    reg [1:0] state;
//    parameter IDLE  = 2'b00;
//    parameter START = 2'b01;
//    parameter READ  = 2'b10;
//    parameter STOP  = 2'b11;
    
//    reg [2:0] data_counter;
//    reg [3:0] sample_counter;
//    reg [7:0] shift_register;
    
//always @(posedge clk or posedge rst) begin
//    if (rst) begin
//        state <= IDLE;
//        data_out <= 0;
//        rx_done <= 0;
//        data_counter <= 0;
//        shift_register <= 0;
//        sample_counter <= 0;
//    end
//    else 
//        begin
//        case (state)
//            IDLE: 
//                begin
//                rx_done <= 0;
//                shift_register <= 0;
//                data_counter <= 0;
//                sample_counter <= 0;
//                if (rx_data == 0)
//                    state <= START;
//                end
            
//            START:
//                begin
//                if (tick_in) 
//                    begin
//                    if (sample_counter == 7) 
//                        begin
//                        if (rx_data == 0) 
//                            begin
//                            state <= READ;
//                            sample_counter <= 0;
//                            end 
//                        else 
//                            state <= IDLE;
//                    end
//                    else 
//                        sample_counter <= sample_counter + 1'b1;

//                    end
//                end
                
//            READ: 
//                begin
//                if (tick_in) 
//                    begin
//                    if (sample_counter == 15) 
//                        begin 
//                        shift_register <= {rx_data, shift_register[7:1]};
//                        sample_counter <= 0;
//                        if (data_counter == 7)
//                            state <= STOP;
//                        else
//                            data_counter <= data_counter + 1'b1;
//                        end
//                    else
//                        sample_counter <= sample_counter + 1'b1;
//                    end
//                end
            
//            STOP: 
//                begin
//                if (tick_in) 
//                    begin
//                    if (sample_counter == 15) 
//                        begin
//                        data_out <= shift_register;
//                        rx_done <= 1;
//                        state <= IDLE;
//                        end
//                    else 
//                        sample_counter <= sample_counter + 1'b1;
//                    end
//                end
            
//            default: 
//                begin
//                state <= IDLE;
//                end
//        endcase 
//    end
//end

//endmodule
`timescale 1ns / 1ps

module uart_receiver(
    input wire clk,
    input wire rst,
    input wire tick_in,
    input wire rx_data,
    output reg [7:0] data_out,
    output reg rx_done
);

    reg [1:0] state;
    parameter IDLE  = 2'b00;
    parameter START = 2'b01;
    parameter READ  = 2'b10;
    parameter STOP  = 2'b11;

    reg [2:0] data_counter;
    reg [3:0] sample_counter;
    reg [7:0] shift_register;

    always @(posedge clk or posedge rst) begin
        if (rst) begin
            state <= IDLE;
            data_out <= 0;
            rx_done <= 0;
            data_counter <= 0;
            shift_register <= 0;
            sample_counter <= 0;
        end else begin
            case (state)
                IDLE: begin
                    rx_done <= 0;
                    shift_register <= 0;
                    data_counter <= 0;
                    sample_counter <= 0;
                    if (rx_data == 0)
                        state <= START;
                end
                START: begin
                    if (tick_in) begin
                        if (sample_counter == 7) begin
                            if (rx_data == 0) begin
                                state <= READ;
                                sample_counter <= 0;
                            end else
                                state <= IDLE;
                        end else
                            sample_counter <= sample_counter + 1;
                    end
                end
                READ: begin
                    if (tick_in) begin
                        if (sample_counter == 15) begin
                            shift_register <= {rx_data, shift_register[7:1]};
                            sample_counter <= 0;
                            if (data_counter == 7)
                                state <= STOP;
                            else
                                data_counter <= data_counter + 1;
                        end else
                            sample_counter <= sample_counter + 1;
                    end
                end
                STOP: begin
                    if (tick_in) begin
                        if (sample_counter == 15) begin
                            data_out <= shift_register;
                            rx_done <= 1;
                            state <= IDLE;
                        end else
                            sample_counter <= sample_counter + 1;
                    end
                end
                default: state <= IDLE;
            endcase
        end
    end
endmodule
