var serialport = require("serialport");
var fs = require("fs");

var SerialPort = serialport.SerialPort;

var serialPort = new SerialPort("COM4", {
  baudrate: 38400,
  parser: serialport.parsers.readline("\n")
});

var log_file = fs.createWriteStream('log.txt','w');

serialPort.on("open", function () {
  console.log('open');

  serialPort.on('data', function(data) {
    console.log(data);
    log_file.write(data);
  });

});
