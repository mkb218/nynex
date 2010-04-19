#!/bin/sh

from googlevoice import Voice
from subprocess import call

download_dir = '/tmp/nynex'
sample_dir = '/opt/nynex/samples'

voice = Voice()
voice.login()

for message in voice.voicemail().messages:
    path=message.download(download_dir)
    target=sample_dir + os.path.basename(path) + ".aiff"
    call(["afconvert", "-f", "AIFF", "-d", "BEI16", "-c", "2", "--src_complexity", "bats", path, target])
    print(target)
    message.delete()

