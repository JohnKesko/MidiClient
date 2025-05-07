## Midi client for macOS using CoreMIDI

### How to run?

1. Clone the repo:

```
git clone https://github.com/JohnKesko/MidiClient.git
```

2. cd MidiClient
3. make
4. Run the app: ./midiapp
4. Play on your midi keyboard to see notes playing (no sound though)

This is the bare minimum to handle MIDI on macOS.
Because of that, we need to include both CoreMIDI and CoreFoundation during compilation.

See the makefile.
