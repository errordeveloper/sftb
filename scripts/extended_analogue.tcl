######################################
# These settings may need adjustment!
######################################

proc audio_trace {} {
gtkwave::/Edit/Data_Format/Signed_Decimal
gtkwave::/Edit/Data_Format/Analog/Interpolated
}

set audio_width 16

set Z "-20"

######################################

set P "GTKWAVE |"

# set T "audio_output_test"

namespace eval gtkwave {


  proc which_level {m} { global P

	  llength [split $m "."]
  }



  # For now let's stick to fixed
  # word lenght, i.e. [31:0]
  proc match_audio {m} { global P
	  set a "31:0"
	  if { -1 !=  [string first "\[$a\]" $m] } then {
	  puts "  (:audio)"
	  return 1 } else { return 0 }
  }

  ## I might try:
  # if { [string lenght $m] == [expr [string first $c $m] + [string last $c $m]] } {}

  # It's quite reasonable to match
  # any signal which has "cl" in it
  # e.g.'aucl' or 'sndclk' or any ..
  proc match_clock {m} {
	  set c "cl"
	  if { -1 != [string first $c $m] } then {
	  puts "  (:clock)"
	  return 1 } else { return 0 }
  }
  
  set dumpfile [ getDumpFileName ]
  set dumptype [ getDumpType ]
  
  set n [ getNumFacs ]
  
  # forceOpenTreeNode $T
  
  puts "$P Loaded $dumptype waveform file '$dumpfile'"
  puts "$P Found a total of $n signals"
  
  set ausig [list]
  set clsig [list]
  
  for { set i 0 } { $i < $n } { incr i } {
  
    set f [ getFacName $i ]

    set v [ which_level $f ]
  
    puts -nonewline "$P\t( #$i @$v ):: $f"

    if { $v > 2 } {

      puts "\n$P\t Not going below the top level!"

    } else {
  
      if { [match_audio $f] } then { lappend ausig "$f" }
      if { [match_clock $f] } then { lappend clsig "$f" }

    }
  
  }
  
  puts "number of audio signals found: [llength $ausig]"
  
  puts "number of clock signals found: [llength $clsig]"
  
  addSignalsFromList "$ausig"
  addSignalsFromList "$clsig"

  highlightSignalsFromList "$ausig"

  audio_trace
  
  unhighlightSignalsFromList "$ausig"

  for { set l 0 } { $l < [llength $ausig] } { incr l } {

    highlightSignalsFromList "[lindex $ausig $l]"

    puts "$P Extending #$l: [lindex $ausig $l]"

    for { set b 0 } { $b < $audio_width } { incr b } {

      /Edit/Insert_Analog_Height_Extension

    }

    unhighlightSignalsFromList "[lindex $ausig $l]"

  }

  setZoomFactor $Z

}

## NOTES:
## 
## Anything we get from gtkwave::getFacName
## is goint to be a signal, i.e. form: 
##
## top
## top.test
## top.test.audio[31:0]
## top.test.clock
## top.test.timer
## top.test.timer.clgen
##
## we will only get:
##
## top.test.audio[31:0]
## top.test.clock
## top.test.timer.clgen
##
## Hence to parse the structure we only
## need to count the dots

## 00:48 Irssi: >>> Join to #tcl was synced in 2 secs
## 00:49 < error_dev> hi.
## 00:49 < error_dev> I am using an application (namely GTKwave)
## 00:49 < error_dev> which embeds tcl
## 00:49 < error_dev> well, it has a dotted obeject structure handled by string 
## 00:50 < error_dev> like "top.hello"
## 00:50 < error_dev> "top.hello.obj1"
## 00:50              ijchain stevel has become available
## 00:50 < error_dev> "top.hello.obj2.prop1"
## 00:50 < error_dev> and stuff like that
## 00:51 < error_dev> how I could rebuild this into a nested list or something of that nature
## 00:52 < error_dev> ... I mean what the most natural way to store this dotted string tree into a proper Tcl tree ?
## 00:58 <@  ijchain> <kbk> Pretty easy with dicts...
## 00:59              >>> BigBen_!~BigBen@77-253-54-89.adsl.inetia.pl
## 01:00 <@  ijchain> <kbk> % dict set a {*}[split top.hello .] {}
## 01:00 <@  ijchain> <kbk> top {hello {}}
## 01:00 <@  ijchain> <kbk> % dict set a {*}[split top.hello.obj1 .] {}
## 01:00 <@  ijchain> <kbk> top {hello {obj1 {}}}
## 01:00 <@  ijchain> <kbk> % dict set a {*}[split top.hello.obj2.prop1 .] {}
## 01:00 <@  ijchain> <kbk> top {hello {obj1 {} obj2 {prop1 {}}}}
## 01:00              <   xiata`home!~Xiata@pool-68-238-225-89.phil.east.verizon.net [Ping timeout: 252 seconds]
## 01:04              >>> xiata`home!~Xiata@pool-71-255-109-139.phlapa.east.verizon.net
## 01:04 < error_dev> kbk: ah, I forgot about the dicts!!!
## 01:05 < error_dev> kbk: thanks ;)
