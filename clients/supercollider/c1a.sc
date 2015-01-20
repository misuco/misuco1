// this is a scratch-file which contains some supercollider-experiments
// that have been used to develop misuco
//
// (c) 2011 by Claudio Zopfi
//
// Licensed under GNU/GPL (http://www.gnu.org/licenses/gpl.txt)

SynthDef("c1a1",{ arg out=0, pan=0, trig=0.0, freq=500, amp=1, par1=1, par2=1;
freq = Lag.kr(freq,0.1);
e= EnvGen.kr(Env.asr(2, 1, 2.5), amp, doneAction:2);
Out.ar(out,Pan2.ar(RLPF.ar(WhiteNoise.ar(e),par1*5000,par2),
pan));
}).send(s);

SynthDef("c1a1",{ arg out=0, pan=0, trig=0.0, freq=500, amp=1, par1=1, par2=1;
freq = Lag.kr(freq,0.1);
e= EnvGen.kr(Env.asr(2, 1, 2.5), amp, doneAction:2);
Out.ar(out,Pan2.ar(RLPF.ar(Saw.ar([freq,freq*2],e),par1*5000,par2),
pan));
}).send(s);

(
// define a noise pulse
SynthDef.new("c1a1", { arg freq = 1200, rate = 2;
var osc, trg;
trg = Decay2.ar(Impulse.ar(rate,0,0.3), 0.01, 0.3);
osc = {WhiteNoise.ar(trg)}.dup;
Out.ar(0, osc); // send output to audio bus zero.
}).send(s);
)

SynthDef.new("c1a1", { arg freq = 440, amp = 0, out = 0, par1=1, par2=1;
e= EnvGen.kr(Env.asr(2, 1, 2.5), amp, doneAction:2);
Out.ar(out, BPF.ar(WhiteNoise.ar(e)*0.5), freq+par1*10000, par2*4, 0.2 ); }).send(s);

SynthDef.new("c1a1", { arg freq = 440, amp = 0, out = 0, par1=1, par2=1;
e= EnvGen.kr(Env.asr(1, 1, 2.5), amp, doneAction:2);
Out.ar(out, BPF.ar(Saw.ar(freq*par1+0.5, 1)*e*0.5),freq*3*par1, par2 ,0.4); }).send(s);

SynthDef.new("c1a1", { arg freq = 440, amp = 0, out = 0;
e= EnvGen.kr(Env.asr(1, 1, 2.5), amp, doneAction:2);
Out.ar(out, Saw.ar(freq, 1)*e*0.5); }).send(s);

SynthDef.new("c1a1", { arg freq = 440, amp = 0, out = 0, par1 = 0, par2 =0;
e= EnvGen.kr(Env.asr(1, 1, 2.5), amp, doneAction:2);
Out.ar(out, (SinOsc.ar(freq, 0, 1, 0)+(SinOsc.ar(freq*(par1+1), 0, 1, 0)*par2))*e*0.5); }).send(s);

// stereo
SynthDef.new("c1a1", { arg freq = 440, amp = 0, out = 0, par1 = 0, par2 =0;
e= EnvGen.kr(Env.asr(1, 1, 2.5), amp, doneAction:2);
Out.ar(out, (SinOsc.ar(freq, 0, 1, 0)+(SinOsc.ar(freq*(par1+1), 0, 1, 0)*par2))*[e,e]*0.5); }).send(s);

// stereo with lpf
SynthDef("c1a1",{ arg out=0, pan=0, sustain=0.5, freq=500, amp=0, par1=0.5, par2=0.5;
var env;
env = EnvGen.kr(Env.perc(0.01, sustain, 0.2), doneAction:0, gate:amp);
Out.ar(out,Pan2.ar(RLPF.ar(Saw.ar(Lag.kr(freq,0.1),env),par1*10000,par2)*amp,
pan));
}).load(s);


//
// stereo saw with lpf nund adsr
//
// ar: audio rate
// kr: kontrol rate
//
SynthDef("c1a1",{ arg out=0, pan=0, sustain=0.5, freq=500, amp=0, par1=0.5, par2=0.5;
var env;
env = EnvGen.kr(Env.adsr, amp, doneAction:2);
Out.ar(out,Pan2.ar(RLPF.ar(Saw.ar(Lag.kr([freq,freq*2],0.1),env),par1*1000,par2)*amp,
pan));
}).load(s);


//
// stereo pulse with lpf nund adsr
//
// ar: audio rate
// kr: kontrol rate
//
SynthDef("c1a1",{
	arg freq=500, amp=0, par1=0.5, par2=0.5;
	var env;
	env = EnvGen.kr(Env.adsr, amp, doneAction:2);
	Out.ar(
		0,RLPF.ar(
			Pulse.ar([freq,freq*2],0.5,env/4,
			WhiteNoise.ar(env/2))
			,par1*3000,par2)
		);
	}
).load(s);

//
// stereo pulse with lpf nund adsr
//
// ar: audio rate
// kr: kontrol rate
//
SynthDef("c1a1",{
	arg freq=500, amp=0, par1=0.5, par2=0.5;
	var env;
	env = EnvGen.kr(Env.adsr, amp, doneAction:2);
	Out.ar(0,
		Pulse.ar(freq,0.5,env/4,
		Pulse.ar(freq*1.3348,par1,env/4,0,
		Pulse.ar(freq*1.4983,par2,env/4,0
	))));
	}
).load(s);

//
// Pro53: Fat Bright Synth
//
SynthDef("c1a1",{
	arg freq=500, amp=0, par1=0.5, par2=0.5;
	var env;
	env = EnvGen.kr(Env.adsr(5,0.2,1,0.5), amp, doneAction:2);
	Out.ar(0,
		RLPF.ar(
			Pulse.ar(freq,0.5,env,0)+Pulse.ar(freq*1.01,0.5,env,0)
		,par2*3000,par1,0,0)
	);
}).load(s);

SynthDef("c1a1",{arg freq=500, amp=0, par1=0.5, par2=0.5;var env;env = EnvGen.kr(Env.adsr(5,0.2,1,0.5), amp, doneAction:2);Out.ar(0,RLPF.ar(Pulse.ar(freq,0.5,env,0)+Pulse.ar(freq*1.01,0.5,env,0),par2*3000,par1,0,0));}).load(s);

SynthDef("c1a1",{
	arg freq=500, amp=0, par1=0.5, par2=0.5;
	var env;
	env = EnvGen.kr(Env.adsr(5,0.2,1,0.5), amp, doneAction:2);
	Out.ar(0,
		Pulse.ar(freq,0.5,env,0)+Pulse.ar(freq*1.01,0.5,env,0)
	);
}).load(s);


//
// My Epiano
//
SynthDef("m0",{
	arg freq=500, amp=0, par1=0.5;
	var env;
	env = EnvGen.kr(Env.adsr(0.1,0.2,0.2,0.5), amp, doneAction:2);
	Out.ar(0,
		SinOsc.ar(freq,0,env*0.5*SinOsc.ar(6,0,par1,1),0)
	);
	}
).load(s);

SynthDef("m1",{
	arg freq=500, amp=0, par1=0.5;
	var env;
	env = EnvGen.kr(Env.adsr(0.1,0.2,0.2,0.5), amp, doneAction:2);
	Out.ar(0,
		Pulse.ar(freq,par1,env,0);
	);
	}
).load(s);


SynthDef("c1a1",{
	arg freq=500, amp=0, par1=0.5, par2=0.5;
	var env;
	env = EnvGen.kr(Env.adsr(0.1,0.2,0.2,0.5), amp, doneAction:2);
	Out.ar(0,
		SinOsc.ar(freq,0,env*0.5,0)
	);
	}
).load(s);

// Load an audio file
s.sendMsg(\b_allocRead, 10, "/home/c1audio/Desktop/alto-aaa-F-1-d4.wav");
s.sendMsg(\b_allocRead, 10, "/home/c1audio/Desktop/string.wav");

// Now define the granular synth
SynthDef("c1a1",{
	arg freq=500, amp=0, par1=0.5, par2=0.5;
	var env;
	env = EnvGen.kr(Env.adsr(5,0.2,1,0.5), amp, doneAction:2);
	Out.ar(0,
		TGrains.ar(
		2, 			// num chan
		Impulse.ar(1000), 	// trigger
		10,			// buf_num
		freq/200,		// rate
		2,			// center pos	[cp]=s
		par2,			// duration	[d]=s
		0, 			// pan		-1..1
		env,			// amp
		4)			// interpolation
	);
	}
).load(s);



x=Synth.new("c1a1");

x.set("freq", 660, "amp" , 0);
x.set("freq", 1700, "amp", 1);
x.set("freq", 17000, "amp", 0);

s.quit;

s.start;
s.status;


mat thi as
5   7  8
live tom meier
5   7  5
at dock 18 in 20 11 zurich
6  7  9  10 11 13 14
ma ri o  pu rk hat hof er
7  8  9  10 11 12 13 14
c  1  a  u  d  i  o
6  7  9  10 11 13 14
dr mad and ti ni -   in  the place to be
7  6   7   8  9  10  9   11   10   11 12
and res
5   7



