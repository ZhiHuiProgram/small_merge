export PATH=/home/orangepi/yuanxueshe/garbagesort/camera/mjpg-streamer/mjpg-streamer-experimental:$PATH
sudo mjpg_streamer -i "/home/orangepi/yuanxueshe/garbagesort/camera/mjpg-streamer/mjpg-streamer-experimental/input_uvc.so -d /dev/video0 -u -f 30" -o "/home/orangepi/yuanxueshe/garbagesort/camera/mjpg-streamer/mjpg-streamer-experimental/output_http.so -w /home/orangepi/yuanxueshe/garbagesort/camera/mjpg-streamer/mjpg-streamer-experimental/www"

