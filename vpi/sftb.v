module audio_output(output reg [31:0] x, input c);

// perhaps eventually we can improve this and open a playlist
//initial $sftb_open_input_file("/tmp/sunday.aiff");
initial $sftb_open_input_file("./data/mono16@22050.f7620.aif");

always @(posedge c)
 // x <= $sftb_fetch_sample;
 assign x = 32'b11111111111111111111111111111111 ;

endmodule

module audio_output_test;

wire signed [31:0] audio_data;
reg clock;

//audio_output audio_output_device ( .c(clock), .x(audio_data) );


wire signed [31:0] audio_wire[31:0];

initial begin

$dumpvars;

repeat (1000000) begin
//repeat (10) begin
	//$sftb_fetch_sample(audio_data, 0, "clock", audio_data[20:15], clock, audio_wire);
	$sftb_fetch_sample(audio_data); //, 0, "clock", audio_data, audio_data, audio_wire);
	#10 clock <= 1'b0;
	#10 clock <= 1'b1;
end

$sftb_close_input_file;

$finish;

end

endmodule
