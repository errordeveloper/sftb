`include "sftb.vh"

module audio_output(output reg signed [31:0] audio, input clock);

always @(posedge clock)
  $sftb_fetch_sample(audio);

endmodule

module audio_output_test;

wire signed [31:0] audio;
reg clock;

audio_output audio_output_device ( .clock(clock), .audio(audio) );

initial begin

// perhaps eventually we can improve this and open a playlist
//initial $sftb_open_input_file("/tmp/sunday.aiff");
//initial $sftb_open_input_file("./data/mono16@22050.f7620.aif");
$sftb_open_input_file;

$dumpfile(`OUTPUT);
$dumpvars;

repeat (`CYCLES) begin
	$sftb_fetch_sample(audio);
	#(`PERIOD/2) clock <= 1'b0;
	#(`PERIOD/2) clock <= 1'b1;
end

$sftb_close_input_file;

$finish;

end

endmodule
