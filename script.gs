var ss = SpreadsheetApp.openById('SHEET-ID');
var sheet = ss.getSheetByName('Sheet1');
var timezone = "Asia/Jakarta";

function doGet(e){
  Logger.log( JSON.stringify(e) );
  if (e.parameter == 'undefined') {
    return ContentService.createTextOutput("Received data is undefined");
  }

  var Curr_Date = new Date();
  var Curr_Time = Utilities.formatDate(Curr_Date, timezone, 'HH:mm:ss');
  var name = stripQuotes(e.parameters.name);

  var nextRow = sheet.getLastRow() + 1;
  sheet.getRange("A" + nextRow).setValue(Curr_Date);
  sheet.getRange("B" + nextRow).setValue(Curr_Time);
  sheet.getRange("C" + nextRow).setValue(name);

  return ContentService.createTextOutput("Card holder name is stored in column C");
}

function stripQuotes( value ) {
  return value.toString().replace(/^["']|['"]$/g, "");
}

function doPost(e) {
  var val = e.parameter.value;

  if (e.parameter.value !== undefined){
    var range = sheet.getRange('A2');
    range.setValue(val);
  }
}
