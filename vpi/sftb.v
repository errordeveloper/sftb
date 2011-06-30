`ifndef PERIOD
`define PERIOD 10
`endif

`ifndef CYCLES
`define CYCLES 1000000
`endif

`ifndef OUTPUT
`define OUTPUT "audio_output_test.vcd"
`endif

module audio_output(output reg signed [31:0] audio, input clock);

always @(posedge clock)
  $sftb_fetch_sample(audio);

endmodule

module audio_output_test;

wire signed [31:0] audio;
reg clock;

audio_output audio_output_device ( .clock(clock), .audio(audio) );

wire signed [31:0] test1;
wire signed [31:0] test2;
wire signed [31:0] test3;
wire signed [31:0] test4;

initial begin

// perhaps eventually we can improve this and open a playlist
$sftb_open_input_file("/tmp/sunday.aiff");
//$sftb_open_input_file("./data/mono16@22050.f7620.aif");
//$sftb_open_input_file;

$dumpfile(`OUTPUT);
$dumpvars;

repeat (`CYCLES) begin
	$sftb_fetch_sample(audio, test1, test2, test3);
	//$sftb_fetch_sample(audio, 0, test1, test2, clock, test3, test4, 0);
	#(`PERIOD/2) clock <= 1'b0;
	#(`PERIOD/2) clock <= 1'b1;
end

$sftb_close_input_file;

$finish;

end

endmodule
