echo 18 | sudo tee /sys/class/gpio/export
echo out | sudo tee /sys/class/gpio/gpio18/direction
echo 1 | sudo tee sys/class/gpio/gpio18/value
echo 0 | sudo tee sys/class/gpio/gpio18/value
