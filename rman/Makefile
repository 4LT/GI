build: image.tif Makefile

view: image.tif Makefile
	open image.tif

.PHONY: build view

brick.slo: brick.sl noises.h patterns.h
	shader brick.sl

wood2.slo: wood2.sl material.h
	shader wood2.sl

plastic.slo: plastic.sl
	shader plastic.sl

image.tif: rit.rib brick.slo wood2.slo plastic.slo
	prman rit.rib
