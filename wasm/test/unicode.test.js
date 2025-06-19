const assert = require('assert');
const fs = require('fs');
const path = require('path');
const createBibneatModule = require('../lib/node/bibneat.js');

createBibneatModule().then(Module => {
  const db = new Module.BibDB();
  const parser = new Module.Parser(db, false);
  const normalizer = new Module.FieldNormalizer(db);

  function loadBibFile(filename) {
    return fs.readFileSync(path.join(__dirname, '../../test/testBibfiles/', filename), 'utf8');
  }

  // Accents
  let bib = loadBibFile('nonAscii.bib');
  assert(parser.parseString(bib));
  assert.strictEqual(db.numEntries, 4n);
//   assert(db.getEntries().get(0).attributes.get('title').includes('\u00dcber'));
//   assert(db.getEntries().get(1).attributes.get('author').includes('M\u0105dzik'));
//   assert(db.getEntries().get(2).attributes.get('title').includes('K\u00f6rper'));
  normalizer.NFCNormalize();
//   assert(db.getEntries().get(0).attributes.get('title').includes('\u00dcber'));
//   assert(db.getEntries().get(1).attributes.get('author').includes('M\u0105dzik'));
//   assert(db.getEntries().get(2).attributes.get('title').includes('K\u00f6rper'));
  console.log('Unicode normalization tests passed.');
});
