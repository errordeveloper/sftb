module audio_output(output reg [31:0] x, input c);

initial $sftb_open_input_file;

always @(posedge c)
 // x <= $sftb_fetch_sample;
 assign x = 32'b11111111111111111111111111111111 ;

endmodule

module audio_output_test;

wire [31:0] audio_data;
reg clock;

audio_output audio_output_device ( .c(clock), .x(audio_data) );


initial begin

$dumpvars;

repeat (100000) begin
	$display($sftb_fetch_sample);
	#10 clock <= 1'b0;
	#10 clock <= 1'b1;
end

$finish;

end

endmodule
