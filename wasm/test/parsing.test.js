const assert = require('assert');
const fs = require('fs');
const path = require('path');
const createBibneatModule = require('../lib/bibneat.js');

createBibneatModule().then(Module => {
  const db = new Module.BibDB();
  const parser = new Module.Parser(db, false);

  // Helper to load bib files as strings
  function loadBibFile(filename) {
    return fs.readFileSync(path.join(__dirname, '../../test/testBibfiles/', filename), 'utf8');
  }

  // Single-entry file
  let bib = loadBibFile('single.bib');
  assert(parser.parseString(bib));
  assert.strictEqual(db.numEntries, 1n);
//   assert.strictEqual(db.getEntries().get(0).bibKey, 'Newman_1942');
//   assert.strictEqual(db.getEntries().get(0).bibType, Module.BT_ARTICLE);

  // Multiple-entry file
  db.clear()
  bib = loadBibFile('multiple.bib');
  assert(parser.parseString(bib));
  assert.strictEqual(db.numEntries, 3n);
//   assert.strictEqual(db.getEntries().get(0).bibKey, 'Talbot_1834');
//   assert.strictEqual(db.getEntries().get(1).bibKey, 'Dirac_1925');
//   assert.strictEqual(db.getEntries().get(2).bibKey, 'Kubo_1966');

  // Curly braces and one-word attributes
  db.clear()
  bib = loadBibFile('groups.bib');
  assert(parser.parseString(bib));
//   assert.strictEqual(db.getEntries().get(0).attributes.get('year'), '1984');
//   assert.strictEqual(db.getEntries().get(0).attributes.get('publisher'), '{Cambridge University} Press');

  // Entries without @ are junk
  db.clear()
  bib = loadBibFile('bad.bib');
  assert(parser.parseString(bib));
  assert.strictEqual(db.numEntries, 2n);
//   assert.strictEqual(db.getEntries().get(0).bibKey, 'Talbot_1834');
//   assert.strictEqual(db.getEntries().get(1).bibKey, 'Kubo_1966');

  console.log('Parsing tests passed.');
});
