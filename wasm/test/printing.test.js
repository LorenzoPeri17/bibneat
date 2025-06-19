const assert = require('assert');
const fs = require('fs');
const path = require('path');
const createBibneatModule = require('../lib/node/bibneat.js');

createBibneatModule().then(Module => {
  const db = new Module.BibDB();
  const parser = new Module.Parser(db, false);
  const printer = new Module.Printer(db);

  // Helper to load bib files as strings
  function loadBibFile(filename) {
    return fs.readFileSync(path.join(__dirname, '../../test/testBibfiles/', filename), 'utf8');
  }

  // Single-entry file
  let bib = loadBibFile('single.bib');
  assert(parser.parseString(bib));
  assert.strictEqual(db.numEntries, 1n);

  const expected = "@article{Newman_1942,\n" + 
    "  title={On a String Problem of Dirac},\n" +
    "  author={Newman, M. H. A.},\n" +
    "  year={1942},\n" +
    "  journal={Journal of the London Mathematical Society},\n" +
    "  volume={s1-17},\n" +
    "  doi={10.1112/jlms/s1-17.3.173},\n" +
    "  issn={1469-7750},\n" +
    "  pages={173-177},\n" +
    "  language={en},\n" +
    "  number={3}\n" +
    "}\n\n"

  const output = printer.toString();
  assert.equal(output, expected);

  console.log('Printing tests passed.');
});