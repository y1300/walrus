# WALRUS
"Classification using an Artificial Neural Network running on an Intel Curie/Genuino 101"
Starting from Walrus's BEng Individual Project, Chen moves forward to explore ways to reduce the memory consumption, so as to increase the size of the network on the same memory to improve classification accuracy.

## Dependencies

An Intel Curie.
Bluetooth Low Energy (BLE) capable Android phone

- [Arduino IDE (v1.6 or later)](https://www.arduino.cc/en/Main/Software)
- [Curie IMU (library)](https://www.arduino.cc/en/Reference/CurieIMU)
- [ArduinoANN](http://robotics.hobbizine.com/arduinoann.html)
- [Python 3.4](https://www.python.org/downloads/) (start to change to Python 3)
- [GCC 6](https://gcc.gnu.org/gcc-6/)
- [Catch](https://github.com/philsquared/Catch)

Plus more to come, undoubtedly.

## Setting up git hooks

To install version-controlled git hooks, run the following script:

    ./.init-hooks

## Training the Classifier

### Logging data from the Curie

To log data from the curie, upload the sketch `logger.ino`, then run `./.log-data` 

### Normalising data for training

The normalisation script has multiple options. Once you've logged enough data, either run `./.normalise-data file` for each file, or run `./.normalise-data -r directory` to normalise all the files in the given directory.
