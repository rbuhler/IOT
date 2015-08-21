// ---------------------------------------------------------------
var fs = require("fs");
var moment = require("moment");
var file_name = require("string");

var now = moment();
var formatted = now.format( 'YYYY_MM_DD_HH_mm_ss' );

   file_name = "";
   file_name += formatted;
   file_name += '.txt';

var log_file = fs.createWriteStream( file_name,'w' );
    log_file.write("Blah");

// ---------------------------------------------------------------
  console.log( "Date and Time 1 : " + formatted );
  console.log( "Data and Time 2 : " + new Date().getTime() );
