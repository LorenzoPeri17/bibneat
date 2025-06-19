const assert = require('assert');
const fs = require('fs');
const path = require('path');
const createBibneatModule = require('../lib/node/bibneat.js');

createBibneatModule().then(Module => {
  const db = new Module.BibDB();
  const parser = new Module.Parser(db, false);
  const api = new Module.ApiCaller(db, 0, 0, false);

  function loadBibFile(filename) {
    return fs.readFileSync(path.join(__dirname, '../../test/testBibfiles/', filename), 'utf8');
  }

  // Arxiv IDs
  let bib = loadBibFile('arxiv.bib');
  assert(parser.parseString(bib));
  assert.strictEqual(db.numEntries, 3n);
  // You may want to add more detailed checks here if you expose getArXivId/getPrepArXivFromEntry in JS
  // For now, just check parsing and entry count
  console.log('API offline tests passed.');
});
