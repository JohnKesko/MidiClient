#include <stdio.h>
#include <CoreMIDI/CoreMIDI.h>

void HandleMIDIEventPacket(const MIDIEventPacket *packet) 
{
    printf("Packet timeStamp: %llu, wordCount: %u\n", packet->timeStamp, packet->wordCount);

    for (int i = 0; i < packet->wordCount; ++i) 
    {
        uint32_t word = packet->words[i];
        printf("Raw word: %08X\n", word);

        // Extract Message Type from the most significant 4 bits
        uint8_t messageType = (word >> 28) & 0x0F;
        
        // Check if this is a MIDI 1.0 Channel Voice Message
        if (messageType == 0x2) 
        {
            uint8_t group = (word >> 24) & 0x0F;
            uint8_t status = (word >> 16) & 0xFF; // Status includes MIDI message type and channel
            uint8_t noteNumber = (word >> 8) & 0xFF;
            uint8_t velocity = word & 0xFF;

            // Interpret status for specific MIDI 1.0 messages
            uint8_t midiMessage = status & 0xF0;
            uint8_t midiChannel = status & 0x0F;

            // Note On with velocity > 0
            if (midiMessage == 0x90 && velocity > 0) 
            {
                printf("Note On: Group %d, Channel %d, Note %d, Velocity %d\n", group, midiChannel, noteNumber, velocity);
            }
            // Note Off or Note On with velocity = 0 
            else if ((midiMessage == 0x90 && velocity == 0) || midiMessage == 0x80)
            {
                printf("Note Off: Group %d, Channel %d, Note %d\n", group, midiChannel, noteNumber);
            }

            // Additional MIDI 1.0 Channel Voice Message handling can be added here
        }
        
        // Handling for other Message Types can be implemented similarly
    }
}

int main() 
{
    MIDIClientRef midiClient;
    MIDIPortRef inputPort;
    OSStatus status;

    status = MIDIClientCreate(CFSTR("MIDI client"), NULL, NULL, &midiClient);
    if (status != noErr) 
    {
        printf("Error creating MIDI client: %d\n", status);
        return -1;
    }

    MIDIReceiveBlock receiveBlock = ^(const MIDIEventList *evtlist, void *srcConnRefCon) 
    {
        const MIDIEventPacket* packet = &evtlist->packet[0];

        // Handle MIDI 1.0 events
        if (evtlist->protocol == kMIDIProtocol_1_0)
        {
            printf("MIDI 1.0\n");
            for (int i = 0; i < evtlist->numPackets; ++i) 
            {
                HandleMIDIEventPacket(packet);
                packet = MIDIEventPacketNext(packet);
            }
        }
        // Handle MIDI 2.0 events
        else if (evtlist->protocol == kMIDIProtocol_2_0)
        {
            printf("MIDI 2.0\n");
        }
    };

    status = MIDIInputPortCreateWithProtocol(midiClient, CFSTR("Input port"), kMIDIProtocol_1_0, &inputPort, receiveBlock);
    if (status != noErr) 
    {
        printf("Error creating MIDI input port: %d\n", status);
        MIDIClientDispose(midiClient);
        return -1;
    }

    ItemCount sourceCount = MIDIGetNumberOfSources();
    printf("Connecting to %lu MIDI sources.\n", (unsigned long)sourceCount);
    for (ItemCount i = 0; i < sourceCount; ++i) 
    {
        MIDIEndpointRef source = MIDIGetSource(i);
        status = MIDIPortConnectSource(inputPort, source, NULL);
        if (status != noErr) 
        {
            printf("Error connecting MIDI source %lu: %d\n", (unsigned long)i, status);
        }
    }

    printf("Listening for MIDI messages. Press Ctrl+C to quit.\n");
    CFRunLoopRun();

    MIDIClientDispose(midiClient);
    return 0;
}
