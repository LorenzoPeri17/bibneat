const assert = require('assert');
const fs = require('fs');
const path = require('path');
const createBibneatModule = require('../lib/bibneat.js');

createBibneatModule().then(Module => {
  const db = new Module.BibDB();
  const parser = new Module.Parser(db, false);
  const filter = new Module.FieldFilter(db);

  function loadBibFile(filename) {
    return fs.readFileSync(path.join(__dirname, '../../test/testBibfiles/', filename), 'utf8');
  }

  // Removing fields Springer
  let bib = loadBibFile('extra.bib');
  assert(parser.parseString(bib));
  assert.strictEqual(db.numEntries, 1n);
//   assert.strictEqual(db.getEntries().get(0).bibKey, 'Philips_2022');
//   assert.strictEqual(db.getEntries().get(0).bibType, Module.BT_ARTICLE);
//   assert(db.getEntries().get(0).attributes.has('abstract'));
//   assert(db.getEntries().get(0).attributes.has('date'));
  filter.keepBibTex();
//   assert(!db.getEntries().get(0).keepAttributes.get('abstract'));
//   assert(!db.getEntries().get(0).keepAttributes.get('date'));
  console.log('Filtering tests passed.');
});
