sudo spt-get update
sudo apt-get -y install tightvncserver
sudo apt-get -y avahi-daemon
sudo apt-get -y install libi2c-dev
sudo apt-get -y install git
git clone git://git.drogon.net/wiringPi
cd wiringPi/
./build
cd ..
sudo apt-get -y install ruby-dev
sudp apt-get -y install --upgrade gcc
sudo gem install fluentd
sudo apt-get -y install cmake
git clone https://github.com/msgpack/msgpack-c.git
cd msgpack-c
cmake .
make
sudo make install
cd ..
git clone https://github.com/m-mizutani/libfluent.git
cd libfluent/
cmake .
make
sudo make install
cd ..
sudo /sbin/ldconfig -v
