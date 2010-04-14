#!/bin/sh

from googlevoice import Voice

download_dir = '/tmp/nynex'

voice = Voice()
voice.login()

for message in voice.voicemail().messages:
    message.download(download_dir)
    message.delete()
