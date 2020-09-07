#ifndef PIPELINE_H
#define PIPELINE_H

Image rgb_to_grayscale(Config * c ,Image img);
Image apply_lap_filter(Config * c,Image img);
uint8_t laplace(uint8_t *p,int i,int j,Image*img,Config*c);
Image apply_binary(Config * c,Image img);
Image rate(Config * c,Image img);

#endif