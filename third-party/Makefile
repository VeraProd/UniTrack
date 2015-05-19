# Author: Dmitry Kukovinets (d1021976@gmail.com)

# Makefile for third-party parts of this program.


.PHONY: all download update
.SILENT: all download update


all:
	# json: nothing to do


download:
	git clone https://github.com/nlohmann/json


update:
	cd json && git pull && cd ..
