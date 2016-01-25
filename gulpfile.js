Error.stackTraceLimit = 100;
var gulp = require('gulp');
var util = require('util');
var path = require('path');
var fmt =util.format.bind(util);
var exec= require('child-process-promise').exec;
var output_dir = 'node_build';
gulp.task('clean_dir', clbk =>{
  var del = require('del');
  del(path.join(output_dir,'**')).then(function(){
    clbk();
  });
});
gulp.task('build',['clean_dir'],(clbk)=>{
  var configure = "node-gyp configure";
  var cmds={
    "el0.36.0_ia32":"node-gyp rebuild --target=0.36.0 --arch=ia32 --dist-url=https://atom.io/download/atom-shell",
    "el0.36.0_x64":"node-gyp rebuild --target=0.36.0 --arch=x64 --dist-url=https://atom.io/download/atom-shell",
    "node5.2.0_ia32":"node-gyp rebuild --target=5.2.0 --arch=ia32",
    "node5.2.0_x64":"node-gyp rebuild --target=5.2.0 --arch=x64"
  };
  var rename = require('gulp-rename');
  var module_path= 'build/Release/gip.node';
  var funcs = [];
  for(var _type in cmds){!function(){
    var cmd = cmds[_type];
    var type = _type;
    var fnc =  function(){
      return exec(cmd).then(function(){
        var strm = gulp.src(module_path)
          .pipe(rename('gip.'+type+'.node'))
          .pipe(gulp.dest(output_dir));
        return new Promise(function(resolve){
          strm.on('finish',function(){
            console.log(type, 'done');
            resolve();
          });
        });
      });
    };
    funcs.push(fnc);
  }()};
  funcs.push(clbk);
  funcs.reduce(function(ret, next){
    return ret.then(next);
  },Promise.resolve());
});

gulp.task('upload_gd',['build'],(clbk)=>{
  console.log('start upload');
  var upload_dir = 'C:/Users/yoshiyuki.m/Google ドライブ';
  gulp.src('node_build/**')
    .pipe(gulp.dest(upload_dir))
    .on('finish',function(){
      console.log('google drive upload done');
      clbk();
    });
});