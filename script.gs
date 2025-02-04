var ss = SpreadsheetApp.openById('ID'); 
var sheet = ss.getSheetByName('Siswa');
var timezone = "Asia/Jakarta"; 

function doGet(e){
  var batasWaktu = '08:00:00';
  Logger.log( JSON.stringify(e) );
  if (e.parameter == 'undefined') {
    return ContentService.createTextOutput("Received data is undefined");
  }

  var Curr_Date = new Date();
  var Curr_Time = Utilities.formatDate(Curr_Date, timezone, 'HH:mm:ss');
  var nama = e.parameters.nama ? stripQuotes(e.parameter.nama) : '';
  var kelas = e.parameters.kelas ? stripQuotes(e.parameter.kelas) : '';
  var jurusan = e.parameters.jurusan ? stripQuotes(e.parameter.jurusan) : '';
  var status = (Curr_Time > batasWaktu) ? "Terlambat" : "Tepat waktu";

  var nextRow = sheet.getLastRow() + 1;
  sheet.getRange("A" + nextRow).setValue(Curr_Date);
  sheet.getRange("B" + nextRow).setValue(Curr_Time);
  sheet.getRange("C" + nextRow).setValue(nama);
  sheet.getRange("D" + nextRow).setValue(kelas);
  sheet.getRange("E" + nextRow).setValue(jurusan);
  sheet.getRange("F" + nextRow).setValue(status);

  return ContentService.createTextOutput("Data terkirim");
}

function stripQuotes( value ) {
  return value.toString().replace(/^["']|['"]$/g, "");
}
