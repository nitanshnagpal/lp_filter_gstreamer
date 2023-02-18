# Gstreamer Plugin

Plugin Type - Low Pass Filter <br />
Cutoff Frequency - 10 Khz <br />
No of channels supported - {1, 2} <br />
Signal Type - Audio only <br />

# Filter Description

This is a FIR Low pass filter, filter is designed using Python Programming language. Blackman Window is used to reduce 
ripples in filter.

![filter](https://user-images.githubusercontent.com/36511520/219869184-53ca64d8-9f12-46d6-ab1e-f0851a97d61f.png)

# Demo Application

A demo application is provided which shows how to use the plugin, In demo application it accepts mp3 file and then it 
plays after filtering it. <br />

The pipeline is build as follows - <br />
<br />
**filesrc - mpegaudioparse - mpg123audiodec - audioresample - audioconvert - my_filter - audioconvert - audioresample - pulsesink** <br /><br />
my_filter plugin is using 48 Khz sampling rate and F64LE format, that's why we're using audioresample and audioconvert before and after my_filter element in pipeline.
