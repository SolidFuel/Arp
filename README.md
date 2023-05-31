# Starp
Stable Random Arpeggiator VST

Most random arpeggiators do not give you the same pattern every time through.

This one does.

You can get it to randomize the velocity as well. That pattern will also be stable.

## Building

This project uses git submodules, so you will need use the `--recurse-submodules` option.
After that, it is a pretty straight forward cmake project

```sh
git clone --recurse-submodules https://github.com/mhhollomon/Starp.git
cd Starp
cmake -S. -Bbuild
cmake --buid build
```

## Technology
----

- [TinySHA1](https://github.com/mohaps/TinySHA1/)
- [JUCE](https://juce.com/)
