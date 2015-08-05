var serialport = require("serialport");
var SerialPort = serialport.SerialPort;

var serialPort = new SerialPort("COM4", {
  baudrate: 38400,
  parser: serialport.parsers.readline("\n")
});

serialPort.on("open", function () {
  console.log('open');
  serialPort.on('data', function(data) {
    console.log(data);
  });
});
