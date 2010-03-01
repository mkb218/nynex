/*
 *  evolvertest.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 2/2/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#include "evolvertest.h"
#include "composition.h"
#include "ga/ga.h"
#include "audiofiles.h"
#include <iostream>

using namespace std;
using namespace nynex;

int main(int argc, char **argv) {
    // test audio file reader
    FileReaderWriter frw;
    FileReaderWriter::ConversionParameters params;
    params.input.filePath = "/Users/makane/code/nynex/samples/unfields01_01_-_light_ups.wav";
    params.output.filePath = "/Users/makane/code/nynex/output/testfrw.wav";
    params.output.fileType = kAudioFileWAVEType;
    params.output.dataFormat = CAStreamBasicDescription(44100., kAudioFormatLinearPCM, 4, 1, 4, 2, 16, kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked );
    if (!SanityCheck(params.output.dataFormat)) {
        return 1;
    }
    params.output.channels = 2;
    params.output.bitRate = -1;
    params.output.codecQuality = -1;
    params.output.srcQuality = 127;
    params.output.srcComplexity = 'bats';
    params.output.strategy = -1;
    params.output.primeMethod = -1;
    params.output.channelLayoutTag = kAudioChannelLayoutTag_Stereo;
    try {
        frw.Prepare(params);
        FileReaderWriter::ReadBuffer * buf = frw.GetNextReadBuffer();
        cout << "i read a buffer" << endl;
        delete buf;
        FileReaderWriter::ReadBuffer outbuf;
        outbuf.readBuffer = CABufferList::New("readbuf", params.output.dataFormat);
        outbuf.readBuffer->AllocateBuffers(44100*4);
        outbuf.readPtrs = CABufferList::New("readptrs", params.output.dataFormat);
        outbuf.nFrames = 44100;
        void *outdata = malloc(44100*4);
        outbuf.readBuffer->SetBytes(44100*4, outdata);
        outbuf.readPtrs->SetFrom(outbuf.readBuffer);
        frw.Output(params.output.filePath);
        frw.WriteFromBuffer(&outbuf);
        
        
    } catch (CAXException & x) {
        char moo[12];
        cout << "failure: " << x.mOperation << " " << x.FormatError(moo) << endl;
        return 1;
    }
    
    
/*    SampleBank & bank = SampleBank::getInstance();
    bank.setChannels(2);
    bank.setSampleRate(44100);
    bank.setSampleSize(2);
    bank.setSampleDir("/Users/makane/code/nynex/subset");
    Composition c;
    bank.initComposition(c);
    c.bounceToFile("/Users/makane/code/nynex/output/whatwhat.mp3");*/
    return 0;
}