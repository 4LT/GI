build: image.tif Makefile

view: image.tif Makefile
	open image.tif

.PHONY: build view

matte.slo: matte.sl
	shader matte.sl

rim.slo: rim.sl
	shader rim.sl

constant.slo: constant.sl
	shader constant.sl

reflect.slo: reflect.sl
	shader reflect.sl

imager_spec.slo: imager_spec.sl
	shader imager_spec.sl

image.tif: scene.rib matte.slo rim.slo constant.slo reflect.slo imager_spec.slo
	prman scene.rib
