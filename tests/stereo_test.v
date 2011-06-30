`include "sftb.vh"

module stereo_output(
	output reg signed [31:0] audio1,
	output reg signed [31:0] audio2,
	input clock);

always @(posedge clock)
  $sftb_fetch_sample(audio1, audio2);

endmodule

module audio_output_test;

wire signed [31:0] audio1;
wire signed [31:0] audio2;

reg clock;

stereo_output audio_output_device ( .clock(clock), .audio1(audio1), .audio2(audio2) );

initial begin

$sftb_open_input_file("/tmp/sunday.aiff");

$dumpfile(`OUTPUT);
$dumpvars;

repeat (`CYCLES) begin
	$sftb_fetch_sample(audio1, audio2);
	#(`PERIOD/2) clock <= 1'b0;
	#(`PERIOD/2) clock <= 1'b1;
end

$sftb_close_input_file;

$finish;

end

endmodule
