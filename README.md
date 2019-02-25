# node-scws

## Install 
npm install https://github.com/iamlaobie/node-scws/archive/master.tar.gz

## How to use?
```

//default worker
var scws = require ("node-scws");
scws.setDict("xdb file path");

//other worker
scws.createWorker();
scws.setDict("The other xdb file path");
```
