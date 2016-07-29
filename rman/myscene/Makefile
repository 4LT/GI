build: image.tif Makefile

view: image.tif Makefile
	/Applications/GIMP.app/Contents/MacOS/GIMP image.tif

.PHONY: build view

matte.slo: matte.sl
	shader matte.sl

rim.slo: rim.sl
	shader rim.sl

constant.slo: constant.sl
	shader constant.sl

reflect.slo: reflect.sl
	shader reflect.sl

image.tif: scene.rib matte.slo rim.slo constant.slo reflect.slo
	prman scene.rib
