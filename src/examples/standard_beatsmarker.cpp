/*
 * Copyright (C) 2006-2016  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include "pch.h"
#include <iostream>
#include <fstream>
//#include <libswresample/swresample.h>
//#include <libavcodec/avcodec.h>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>
#include "credit_libav.h"
using namespace std;
using namespace essentia;
using namespace standard;

int main(int argc, char* argv[]) {

#if 1
  if (argc != 3) {
    cout << "Error: incorrect number of arguments." << endl;
    cout << "Usage: " << argv[0] << " audio_input output_file" << endl;
    creditLibAV();
    exit(1);
  }

  string audioFilename = argv[1];
  string outputFilename = argv[2];
#else
    string audioFilename = "H:/SpeechData/_music/01 Sweet Home Alabama.mp3";
    string outputFilename = "H:/SpeechData/_music/Sweet Home Alabama BEATS.mp3";
#endif

  // register the algorithms in the factory(ies)
  essentia::init();

  Pool pool;

  /////// PARAMS //////////////
  Real sampleRate = 44100.0;

  AlgorithmFactory& factory = AlgorithmFactory::instance();

  Algorithm* audioLoader = factory.create("MonoLoader",
                                          "filename", audioFilename,
                                          "sampleRate", sampleRate);

  Algorithm* beatTracker = factory.create("BeatTrackerMultiFeature");
  Algorithm* superFlux   = factory.create("SuperFluxExtractor");

  vector<Real> audio;
  vector<Real> beats;
  Real confidence;

  audioLoader->output("audio").set(audio);
  audioLoader->compute();

  beatTracker->input("signal").set(audio);
  beatTracker->output("ticks").set(beats);
  beatTracker->output("confidence").set(confidence);
  beatTracker->compute();

  superFlux->input("signal").set(audio);
  superFlux->output("onsets").set(beats);
  superFlux->compute();


  vector<Real> audioOutput;

  Algorithm* audioWriter = factory.create("MonoWriter",
                                          "filename", outputFilename,
                                          "sampleRate", sampleRate);

  Algorithm* beatsMarker = factory.create("AudioOnsetsMarker",
                                          "onsets", beats,
                                          "type", "beep");

  beatsMarker->input("signal").set(audio);
  beatsMarker->output("signal").set(audioOutput);

  audioWriter->input("audio").set(audioOutput);

  beatsMarker->compute();


  audioWriter->compute();

  delete audioLoader;
  delete beatsMarker;
  delete audioWriter;

  essentia::shutdown();

  return 0;
}
