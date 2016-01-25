var fs= require('fs');
var path = require('path');
var chai = require('chai');
var del = require('del');
var testResultDir = 'test_result';
chai.should();
var assert = chai.assert;
var putTestResult = (n, _buf)=>{
  var buf = new Buffer(_buf);
  fs.writeFileSync(path.join(testResultDir, n), buf,{encoding:null});
  assert(buf.length > 0);
  return buf;
};
var myModule = require('../build/Release/gip');
describe('icon create test',function(){
  before((clbk)=>{
    del(testResultDir+'\\*').then(function(){
      console.log('done delete file');
      clbk();
    });
  });
  describe('large icon test',()=>{
    it('notepad.exe',()=>{
      var ret = myModule.getLargeIcon('C:\\Windows\\notepad.exe')
      var buf = putTestResult('notepad.png',ret);
    });
    it('winamp.exe',()=>{
      var ret = myModule.getLargeIcon("C:\\Program Files (x86)\\Winamp\\winamp.exe");
      var buf = putTestResult('winamp.png',ret);
    });
    it('js',()=>{
      var ret = myModule.getLargeIcon(path.join(__dirname, 'test.js'));
      var buf = putTestResult('js.png', ret);
    });
    it('folder by relative',()=>{
      var ret = myModule.getLargeIcon(".");
      var buf = putTestResult('dirByRelative.png', ret);
    });
    it('folder by absolute',()=>{
      var ret = myModule.getLargeIcon("C:\\Develop\\gip");
      var buf = putTestResult('dirbyAbsolute.png', ret);
    });
    it('folder in data',()=>{
      var ret = myModule.getLargeIcon("C:\\Users\\yoshiyuki.m\\Desktop\\my_scripts\\img\\UMEMARO 整体淫 えろえろエステコース\\000.jpg");
      var buf = putTestResult('jpg.png', ret);
    });    
  });
  describe('small icon test',()=>{
    it('winamp.exe',()=>{
      var ret = myModule.getSmallIcon("C:\\Program Files (x86)\\Winamp\\winamp.exe");
      var buf = putTestResult('winamp.small.png',ret);
    });
    it('notepad.exe',()=>{
      var ret = myModule.getLargeIcon('C:\\Windows\\notepad.exe')
      var buf = putTestResult('notepad.small.png',ret);
    });
    it('js',()=>{
      var ret = myModule.getSmallIcon(path.join(__dirname, 'test.js'));
      var buf = putTestResult('js.small.png', ret);
    });
    it('folder by relative',()=>{
      var ret = myModule.getSmallIcon(".");
      var buf = putTestResult('dirByRelative.small.png', ret);
    });
    it('folder by absolute',()=>{
      var ret = myModule.getSmallIcon("C:\\Develop\\gip");
      var buf = putTestResult('dirbyAbsolute.small.png', ret);
    });
    it('folder in data',()=>{
      var ret = myModule.getSmallIcon("C:\\Users\\yoshiyuki.m\\Desktop\\my_scripts\\img\\UMEMARO 整体淫 えろえろエステコース\\000.jpg");
      var buf = putTestResult('jpg.small.png', ret);
    });    
  });
});

