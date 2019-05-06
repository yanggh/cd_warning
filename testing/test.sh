#!/bin/bash

for var in {1..20}
do
    snmptrap -v 2c -c public 192.168.43.217  aaa 1.3.6.1.4.1.2345 SNMPv2-MIB::sysLocation.0 s "just heresadfasdfasdfasdfasjjdfhasjkdfhajskdfhkasjdhfkljasdhfjklashdkfjhaskjdlfhalsjkfklsdhfklajshdfjkashdjfkhasjdfhajksdfhjaksdhfjasdfajskdf;alsdjf;asdfja;sdjfasfasdfasdfhasjdfhasjdfhlajskdhfjkasdfkajsdfkjaskdjfaksdj你好好爱护哦啊"
done
