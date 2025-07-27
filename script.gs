var ss = SpreadsheetApp.openById('1oJASDVr_XvtvhX8DXpK08sYVQrX-c-gPiXN2cFPgNYA'); 
var timezone = "Asia/Jakarta"; 

function doGet(e) {
  var batasWaktu = '07:40:00';
  Logger.log(JSON.stringify(e));
  if (!e.parameter || e.parameter.nama === undefined) {
    return ContentService.createTextOutput("Received data is undefined");
  }

  var currDate = new Date();
  var currDateStr = Utilities.formatDate(currDate, timezone, 'yyyy-MM-dd');
  var currTime = Utilities.formatDate(currDate, timezone, 'HH:mm:ss');
  
  var sheetName = e.parameter.sheet ? stripQuotes(e.parameter.sheet) : '';
  var nama = e.parameter.nama ? stripQuotes(e.parameter.nama) : '';
  var kelas = e.parameter.kelas ? stripQuotes(e.parameter.kelas) : '';
  var jurusan = e.parameter.jurusan ? stripQuotes(e.parameter.jurusan) : '';
  var status = (currTime > batasWaktu) ? "Terlambat" : "Tepat waktu";

  var sheet = ss.getSheetByName(sheetName);

  var data = sheet.getDataRange().getValues();
  var found = false;

  for (var i = 1; i < data.length; i++) {
    var rowDate = Utilities.formatDate(new Date(data[i][0]), timezone, 'yyyy-MM-dd');
    var rowNama = data[i][3];
    
    if (rowDate === currDateStr && rowNama === nama) {
      // Found existing record
      if (data[i][2] === "") {
        sheet.getRange(i + 1, 3).setValue(currTime); 
        found = true;
        break;
      } else {
        found = true;
        break;
      }
    }
  }

  if (!found) {
    var nextRow = sheet.getLastRow() + 1;
    sheet.getRange("A" + nextRow).setValue(currDate);          
    sheet.getRange("B" + nextRow).setValue(currTime);          
    sheet.getRange("C" + nextRow).setValue("");              
    sheet.getRange("D" + nextRow).setValue(nama);             
    sheet.getRange("E" + nextRow).setValue(kelas);           
    sheet.getRange("F" + nextRow).setValue(jurusan);            
    sheet.getRange("G" + nextRow).setValue(status);     
  }

  return ContentService.createTextOutput("Data terkirim");
}

function stripQuotes(value) {
  return value.toString().replace(/^["']|['"]$/g, "");
}
