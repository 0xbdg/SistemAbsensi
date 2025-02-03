var ss = SpreadsheetApp.openById('1J-5dN056GYmCUCjqARQiZszE2kejEnt7WLkRnYBQWzk'); 
var sheet = ss.getSheetByName('Siswa');
var timezone = "Asia/Jakarta"; 

function doGet(e){
  Logger.log( JSON.stringify(e) );
  if (e.parameter == 'undefined') {
    return ContentService.createTextOutput("Received data is undefined");
  }

  var Curr_Date = new Date();
  var Curr_Time = Utilities.formatDate(Curr_Date, timezone, 'HH:mm:ss');
  var nama = e.parameters.nama ? stripQuotes(e.parameter.nama) : '';
  var kelas = e.parameters.kelas ? stripQuotes(e.parameter.kelas) : '';
  var jurusan = e.parameters.jurusan ? stripQuotes(e.parameter.jurusan) : '';

  var nextRow = sheet.getLastRow() + 1;
  sheet.getRange("A" + nextRow).setValue(Curr_Date);
  sheet.getRange("B" + nextRow).setValue(Curr_Time);
  sheet.getRange("C" + nextRow).setValue(nama);
  sheet.getRange("D" + nextRow).setValue(kelas);
  sheet.getRange("E" + nextRow).setValue(jurusan);

  return ContentService.createTextOutput("Data terkirim");
}

function stripQuotes( value ) {
  return value.toString().replace(/^["']|['"]$/g, "");
}
