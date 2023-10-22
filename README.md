# Starp
Stable Random Arpeggiator VST

Most random arpeggiators do not give you the same pattern every time through.

This one does.

You can get it to randomize the velocity as well. That pattern will also be stable.

**NOTE**: This plugin does not work *except when the play-head is moving*. 
I plan to address this in a future version.

## Building

Build should work for Windows and Linux (I haven't tested on Linux)
There is tooling for MacOS, but I don't think it works.

```sh
git clone https://github.com/mhhollomon/Starp.git
cd Starp
cmake -S. -Bbuild
cmake --build build
```

## Install

### Windows
Copy the build results (or the version downloaded) into `C:\Program Files\Common Files\VST3`

### Linux 
???

### MacOS
???

## Usage in DAW

If a DAW does not appear below, it has not been tested.

### Reaper
Put it in the Track FX list ahead of the synth. easy-peasy.

### Ableton Live
Live does not recognize midi-effects. So, to use this, you will need two tracks.
1. The track with the midi and _Starp_.
1. The track with the synth.

Route the MIDI output of the first track to the second.

### Studio One
Studio One does not allow you to use 3rd party midi effects in the MIDI slots.
Set up is similar to Ableton Live

## Technology
----

- [TinySHA1](https://github.com/mohaps/TinySHA1/)
- [JUCE](https://juce.com/)
