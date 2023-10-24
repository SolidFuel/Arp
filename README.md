# Starp
Stable Random Arpeggiator VST

Most random arpeggiators do not give you the same pattern every time through.

This one does.

You can get it to randomize the velocity as well. That pattern will also be stable.

**NOTE**: This plugin does not work *except when the play-head is moving*. 
I plan to address this in a future version.

## Building

Build should work for Windows and Linux.
There is tooling for MacOS, but I don't think it works.

```sh
git clone https://github.com/mhhollomon/Starp.git
cd Starp
cmake -S. -Bbuild
cmake --build build
```

## Install From Release

These instructions assume you are installingfrom the provide release builds.
If you build it yourself, the instruction will be similar. The needed files
will under the `${BUILD}/Source/Starp_artefacts/Release/VST3`

### Windows
*Note* The release builds only work on 64bit windows.

Unzip the file Starp-win-vx.x.x.zip. Place the resulting `Starp.vst3`
file into `C:\Program Files\Common Files\VST3`

### Linux 
*Note* The release builds only work on x86_64 architecture.

Unzip the file Starp-linux-vx.x.x.zip. Place the resulting `Starp.vst3`
directory into `~/.vst3`

### MacOS
???

## Usage in DAW

If a DAW does not appear below, it has not been tested.

### Reaper
Put it in the Track FX list ahead of the synth.

### Ableton Live
Live does not recognize MIDI effects. So, to use this, you will need two tracks.
1. The track with the MIDI and _Starp_.
1. The track with the synth.

Set the input of the Synth track to be the track with _Starp_ . In the second
drop down, choose _Starp_ itself (not Pre FX or Post Fx) as the input.

### Studio One
Studio One does not allow you to use 3rd party MIDI effects in the MIDI slots.
Set up is similar to Ableton Live

## Technology
----

- [TinySHA1](https://github.com/mohaps/TinySHA1/)
- [JUCE](https://juce.com/)
