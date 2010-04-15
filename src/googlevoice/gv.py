#!/bin/sh

from googlevoice import Voice
from subprocess import call

download_dir = '/tmp/nynex'
sample_dir = '/opt/nynex/samples'

voice = Voice()
voice.login()

for message in voice.voicemail().messages:
    path=message.download(download_dir)
    call(["afconvert", path, sample_dir + os.path.basename(path)]);
    message.delete()

