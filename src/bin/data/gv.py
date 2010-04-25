#!/bin/sh

from googlevoice import Voice
from subprocess import call
import os
from os.path import basename

log = open('/opt/nynex/logs/gvoice.log', 'a')

download_dir = '/tmp/nynex'
sample_dir = '/opt/nynex/samples'

voice = Voice()
log.write(str(voice.login()))

for message in voice.voicemail().messages:
    path=message.download(download_dir)
    log.write(str(path)+"\n")
    target=sample_dir + "/" + basename(path) + ".aiff"
    call(["afconvert", "-f", "AIFF", "-d", "BEI16", "-c", "2", "--src-complexity", "bats", "--src-quality", "127", path, target])
    log.write(str(target)+"\n")
    print(target)
    log.write(str(message.delete()))

