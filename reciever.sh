#!
gst-launch-1.0 udpsrc address = localhost port = 5004 ! "application/x-rtp,media=(string)video,clock-rate=(int)90000,encoding-name=(string)JPEG" ! rtpjpegdepay ! jpegdec ! videoconvert !ximagesink