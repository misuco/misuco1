//
//  mobilesynthViewController.m
//  mobilesynth
//
//  Created by Allen Porter on 12/7/08.
//  Copyright thebends 2008. All rights reserved.
//

#include "mobilesynthviewcontrollerrc1.hpp"
#import "mobilesynthViewControllerRc1.h"
#import "ArpeggioView.h"
#import "AudioOutput.h"
#include "synth/controller.h"
#include "synth/envelope.h"
#include "synth/modulation.h"
#include "synth/oscillator.h"
#include "synth/filter.h"
#include "synth/parameter.h"

namespace mobilesynthview
{
  struct WidgetImpl
  {
    mobilesynthViewControllerRc1* wrapped;
  };
  Widget::Widget() :
    impl(new WidgetImpl)
  {
    impl->wrapped = [[mobilesynthViewControllerRc1 alloc] init];
  }
  Widget::~Widget()
  {
    if (impl)
      [impl->wrapped release];
    delete impl;
  }
    void Widget::noteOn(int n, float f)
    {
        [impl->wrapped noteOn:n :f];
    }
    void Widget::noteOff(int n)
    {
        [impl->wrapped noteOff:n];
    }
    void Widget::pc(int n)
    {
        [impl->wrapped pc:n];
    }
}

@implementation mobilesynthViewControllerRc1

#define degreesToRadians(x) (M_PI * x / 180.0)

// Use A above Middle C as the reference frequency
static const float kNotesPerOctave = 12.0;
static const float kMiddleAFrequency = 440.0;
static const int kMiddleANote = 49;

static float GetFrequencyForNote(int note) {
  return kMiddleAFrequency * powf(2, (note - kMiddleANote) / kNotesPerOctave);
}

- (id)init {
    self = [super init];
    
    controller_ = new synth::Controller;
    /*
    controller_->filter_envelope()->set_attack(400);
    controller_->filter_envelope()->set_decay(100);
    controller_->filter_envelope()->set_sustain(1);
    controller_->filter_envelope()->set_release(100);
    
    controller_->volume_envelope()->set_attack(10);
    controller_->volume_envelope()->set_decay(100);
    controller_->volume_envelope()->set_sustain(0.5);
    controller_->volume_envelope()->set_release(40000);
    */
    
    controller_->set_modulation_amount(0.7);
    controller_->set_modulation_frequency(0.3);
    controller_->set_modulation_source(synth::Controller::LFO_SRC_TRIANGLE);
    controller_->set_modulation_destination(synth::Controller::LFO_DEST_FILTER);
    
    //controller_->set_osc2_shift(0);
    //controller_->set_osc_sync(0);
    
    // OSC 1
    // controller_->set_osc1_level(0.5);
    controller_->set_osc1_wave_type(synth::Oscillator::SAWTOOTH);
    //controller_->set_osc1_octave(synth::Controller::OCTAVE_1);
    
    // OSC 2
    //controller_->set_osc2_level(0);
    //controller_->set_osc2_wave_type(synth::Oscillator::TRIANGLE);
    //controller_->set_osc2_octave(synth::Controller::OCTAVE_1);
    
    //controller_->set_glide_samples(0);
    
    controller_->set_filter_cutoff(2000);
    controller_->set_filter_resonance(0.9);
    
/*    controller_->set_arpeggio_enabled(0);
    controller_->set_arpeggio_octaves(1);
    controller_->set_arpeggio_samples(1);
    controller_->set_arpeggio_step(synth::Arpeggio::UP);
    */
    controller_->set_sample_rate(44100);
    
    // Format preferred by the iphone (Fixed 8.24)
    outputFormat.mSampleRate = 44100.0;
    outputFormat.mFormatID = kAudioFormatLinearPCM;
    outputFormat.mFormatFlags  = kAudioFormatFlagsAudioUnitCanonical;
    outputFormat.mBytesPerPacket = sizeof(AudioUnitSampleType);
    outputFormat.mFramesPerPacket = 1;
    outputFormat.mBytesPerFrame = sizeof(AudioUnitSampleType);
    outputFormat.mChannelsPerFrame = 1;
    outputFormat.mBitsPerChannel = 8 * sizeof(AudioUnitSampleType);
    outputFormat.mReserved = 0;
    
    output = [[AudioOutput alloc] initWithAudioFormat:&outputFormat];
    [output setSampleDelegate:self];
    [output start];  // immediately invokes our callback to generate samples
    return self;
}

- (void)noteOn:(int)note :(float)freq {
  controller_->NoteOn(note, freq);
}

- (void)noteOff:(int)note {
    controller_->NoteOff(note);
}

- (void)pc:(int)prog {
  controller_->set_osc1_wave_type_int(prog%5);
}

- (void)syncControls {
  @synchronized(self) {
  }
}

- (OSStatus)generateSamples:(AudioBufferList*)buffers {
  assert(controller_);
  assert(buffers->mNumberBuffers == 1);  // mono output  
  AudioBuffer* outputBuffer = &buffers->mBuffers[0];
  SInt32* data = (SInt32*)outputBuffer->mData;
    /*
  if (controller_->released()) {
    // Silence
    memset(data, 0, outputBuffer->mDataByteSize);
    return noErr;
  }
     */
  int samples = outputBuffer->mDataByteSize / sizeof(SInt32);
  float buffer[samples];
  controller_->GetFloatSamples(buffer, samples);
  for (int i = 0; i < samples; ++i) {
    data[i] = buffer[i] * 16777216L;
  }
  return noErr;
}

// Setup the scrolable control panel
- (void)loadControlViews { 

}

- (void)startLoadAnimations {
  //
  // Attempt some visual cues that will hopefully let the user notice that they
  // can scroll the control view and keyboard view.
  //
  
  // Start at the bottm and scroll to the top
}

- (void)viewDidLoad {
  [super viewDidLoad];

    
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
  return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

- (void)didReceiveMemoryWarning {
  [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
  // Release anything that's not essential, such as cached data
}

- (void)dealloc {
  [output dealloc];
  delete controller_;
  [super dealloc];
}

- (void)syncPageControl {
  // Switch the indicator when more than 50% of the previous/next page is visible
}

// At the end of scroll animation, reset the boolean used when scrolls originate from the UIPageControl
- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView {

}

- (IBAction)changePage:(id)sender {
}


@end
